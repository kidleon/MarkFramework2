#include "pch.h"
#include "memory_block_pool.h"
#include "spin_lock.h"
#include <atomic>
#include <new>


namespace mark
{
	// ----------------------------------------------------------------------------
	// memory_block_pool — page/block 2단 인덱싱 + generation 기반 stale 검출 풀.
	//
	// 핸들 레이아웃: { page_index(u16) | block_index(u8) | generation(u8) } = 4 bytes.
	//   - 최대 512 페이지 × 256 블록 = 131,072 슬롯.
	//   - generation==0은 항상 invalid (zero-init 호환).
	//
	// 자유 관리:
	//   - 페이지 내부: 블록 단위 intrusive free list (next_free[i]가 다음 자유 블록 인덱스).
	//   - 풀 전체: "최소 1개 자유 블록을 가진 페이지" 만 partial 리스트로 묶어 둠.
	//     acquire는 partial_head에서만 꺼내므로 빈 슬롯 탐색이 O(1).
	//   - 페이지는 lazy 할당. partial_head가 비면 새 페이지 추가.
	//
	// 페이지 메모리 레이아웃 (단일 할당):
	//   [블록 데이터: block_size × N] [next_free: u16 × N] [generations: atomic<u8> × N]
	//   sys_alloc 호출 1회, 같은 풀(mi_malloc 페이지) 안에 자주 같이 접근되는
	//   메타데이터(next_free / generations)가 모이도록 배치.
	//
	// 동시성:
	//   - acquire / release / 내부 free list / partial 리스트: spin_lock_t 보호.
	//   - 새 페이지의 sys_alloc 및 메타데이터 초기화는 **락 외부**에서 수행 후
	//     install_prepared_page 단계만 락 안에서 진행. 페이지 신규 생성 latency가
	//     다른 스레드의 acquire/release를 막지 않음.
	//   - is_memory_block_alive / get_memory_block_memory는 **lock-free**.
	//     · page_count는 std::atomic<uint16_t> (writer release-store / reader acquire-load).
	//     · 슬롯별 generation은 std::atomic<uint8_t>.
	//     · install_prepared_page의 page_count release-store가 모든 메타데이터
	//       publication을 담당 (Pointer Publication 패턴).
	//
	// generation 정책:
	//   - 페이지 준비 시 모든 슬롯 stored_gen = 1 (handle 비교용으로 항상 비-제로).
	//   - acquire: handle.generation = stored_gen (그대로 복사).
	//   - release: stored_gen += 1 (0 wrap-around 시 1로 보정).
	// ----------------------------------------------------------------------------

	namespace
	{
		constexpr uint16_t INVALID_INDEX = 0xFFFFu;

		struct memory_block_page_t
		{
			void*                  memory;            // 단일 할당의 base (= 블록 데이터 시작 주소)
			std::atomic<uint8_t>*  generations;       // 슬롯별 generation
			uint16_t*              next_free;         // intrusive free list 링크
			uint16_t               free_head;
			uint16_t               free_count;
			uint16_t               partial_next;
			uint8_t                in_partial_list;
			uint8_t                _pad;
		};

		struct memory_block_pool_t
		{
			spin_lock_t            lock;
			uint32_t               block_size;
			uint16_t               blocks_per_page;
			uint16_t               partial_head;
			std::atomic<uint16_t>  page_count;        // 페이지 publication 채널: writer release / reader acquire
			uint16_t               _pad;
			memory_block_page_t    pages[MEMORY_BLOCK_POOL_MAX_PAGES];
		};

		struct page_layout_t
		{
			size_t next_free_offset;
			size_t generations_offset;
			size_t total_bytes;
		};

		static page_layout_t compute_page_layout(uint32_t block_size, uint16_t blocks_per_page)
		{
			page_layout_t L;
			const size_t mem_bytes = static_cast<size_t>(block_size) * blocks_per_page;
			L.next_free_offset   = ALIGN_UP(mem_bytes, alignof(uint16_t));
			const size_t after_nf = L.next_free_offset + static_cast<size_t>(blocks_per_page) * sizeof(uint16_t);
			L.generations_offset = ALIGN_UP(after_nf, alignof(std::atomic<uint8_t>));
			L.total_bytes        = L.generations_offset + static_cast<size_t>(blocks_per_page) * sizeof(std::atomic<uint8_t>);
			return L;
		}

		// 락 외부에서 호출. 단일 sys_alloc + 메타데이터 초기화. install 전까지는 pool 상태를 건드리지 않음.
		static bool prepare_new_page(
			uint32_t block_size, uint16_t blocks_per_page,
			memory_block_page_t* page_out)
		{
			const page_layout_t L = compute_page_layout(block_size, blocks_per_page);

			uint8_t* base = static_cast<uint8_t*>(
				CORE_SYS_ALLOC_ALIGNED(L.total_bytes, alignof(std::max_align_t))
			);
			if (!base)
				return false;

			page_out->memory      = base;
			page_out->next_free   = reinterpret_cast<uint16_t*>(base + L.next_free_offset);
			page_out->generations = reinterpret_cast<std::atomic<uint8_t>*>(base + L.generations_offset);

			// 자유 리스트: 0 → 1 → ... → N-1 → INVALID.
			for (size_t i = 0; i + 1 < blocks_per_page; ++i)
				page_out->next_free[i] = static_cast<uint16_t>(i + 1);
			page_out->next_free[blocks_per_page - 1] = INVALID_INDEX;

			// 모든 슬롯 generation = 1로 placement-new 초기화.
			// (handle.generation==0이 invalid이므로 stored_gen은 1부터 시작.)
			for (size_t i = 0; i < blocks_per_page; ++i)
				::new (page_out->generations + i) std::atomic<uint8_t>(1);

			page_out->free_head       = 0;
			page_out->free_count      = blocks_per_page;
			page_out->partial_next    = INVALID_INDEX;
			page_out->in_partial_list = 0;
			page_out->_pad            = 0;
			return true;
		}

		static void remove_partial_head(memory_block_pool_t* pool)
		{
			const uint16_t head = pool->partial_head;
			if (head == INVALID_INDEX)
				return;
			memory_block_page_t* page = &pool->pages[head];
			pool->partial_head = page->partial_next;
			page->partial_next = INVALID_INDEX;
			page->in_partial_list = 0;
		}

		static void push_partial_head(memory_block_pool_t* pool, uint16_t page_idx)
		{
			memory_block_page_t* page = &pool->pages[page_idx];
			page->partial_next    = pool->partial_head;
			page->in_partial_list = 1;
			pool->partial_head    = page_idx;
		}

		// 락 안에서 호출. 준비된 페이지를 page_count 슬롯에 설치하고 partial 리스트 head로 push.
		// page_count.store(release)가 모든 이전 메타데이터 쓰기에 대한 publication fence 역할.
		static bool install_prepared_page(
			memory_block_pool_t* pool,
			const memory_block_page_t* prepared,
			uint16_t* out_page_index)
		{
			const uint16_t cur_count = pool->page_count.load(std::memory_order_relaxed);
			if (cur_count >= MEMORY_BLOCK_POOL_MAX_PAGES)
				return false;

			memory_block_page_t* slot = &pool->pages[cur_count];

			slot->memory          = prepared->memory;
			slot->next_free       = prepared->next_free;
			slot->generations     = prepared->generations;
			slot->free_head       = prepared->free_head;
			slot->free_count      = prepared->free_count;
			slot->partial_next    = pool->partial_head;
			slot->in_partial_list = 1;
			slot->_pad            = 0;

			pool->partial_head = cur_count;

			// lock-free reader의 acquire-load와 페어가 되는 release-store.
			pool->page_count.store(static_cast<uint16_t>(cur_count + 1), std::memory_order_release);

			*out_page_index = cur_count;
			return true;
		}

		// 락 안에서 호출 — partial_head 페이지에서 블록 1개 pop. partial_head != INVALID_INDEX 가정.
		static memory_block_id pop_block_from_partial_head(memory_block_pool_t* pool)
		{
			memory_block_id id = { 0, 0, 0 };

			const uint16_t page_idx = pool->partial_head;
			memory_block_page_t* page = &pool->pages[page_idx];

			const uint16_t block_idx = page->free_head;
			page->free_head = page->next_free[block_idx];
			page->free_count--;

			if (page->free_count == 0)
				remove_partial_head(pool);

			id.page_index  = page_idx;
			id.block_index = static_cast<uint8_t>(block_idx);
			// 락 안이라 relaxed로 충분 — 같은 페이지의 release 쓰기는 락이 직렬화.
			id.generation  = page->generations[block_idx].load(std::memory_order_relaxed);
			return id;
		}

	} // anonymous namespace


	HANDLE create_memory_block_pool(size_t block_size, size_t blocks_per_page)
	{
		if (block_size == 0)
			return nullptr;

		if (blocks_per_page == 0 || blocks_per_page > MEMORY_BLOCK_POOL_MAX_BLOCKS_PER_PAGE)
			return nullptr;

		memory_block_pool_t* pool =
			static_cast<memory_block_pool_t*>(CORE_SYS_CALLOC(sizeof(memory_block_pool_t)));
		if (!pool)
			return nullptr;

		init_spin_lock(&pool->lock);
		pool->block_size      = static_cast<uint32_t>(block_size);
		pool->blocks_per_page = static_cast<uint16_t>(blocks_per_page);
		pool->partial_head    = INVALID_INDEX;
		pool->page_count.store(0, std::memory_order_relaxed);

		return static_cast<HANDLE>(pool);
	}

	void destroy_memory_block_pool(HANDLE handle)
	{
		memory_block_pool_t* pool = static_cast<memory_block_pool_t*>(handle);
		if (!pool)
			return;

		// 호출자가 동시성 책임을 짐: 다른 스레드가 풀을 사용 중일 때 destroy 호출은 UB.
		const uint16_t count = pool->page_count.load(std::memory_order_acquire);
		for (uint16_t i = 0; i < count; ++i)
		{
			memory_block_page_t* page = &pool->pages[i];
			// std::atomic<uint8_t>는 trivially destructible이라 명시적 소멸 필요 없음.
			// memory / next_free / generations 모두 단일 할당이므로 sys_free 한 번.
			CORE_SYS_FREE(page->memory);
			page->generations = nullptr;
			page->next_free   = nullptr;
		}

		sys_free(pool);
	}

	memory_block_id acquire_memory_block(HANDLE handle)
	{
		memory_block_id id = { 0, 0, 0 };

		memory_block_pool_t* pool = static_cast<memory_block_pool_t*>(handle);
		if (!pool)
			return id;

		// Fast path: 락 잠깐 잡고 partial 리스트에서 즉시 pop.
		{
			AUTO_SPIN_LOCK guard(&pool->lock);

			if (pool->partial_head != INVALID_INDEX)
				return pop_block_from_partial_head(pool);

			// partial 비었고 슬롯도 다 찼으면 즉시 실패 (락 외부 sys_alloc 회피).
			if (pool->page_count.load(std::memory_order_relaxed) >= MEMORY_BLOCK_POOL_MAX_PAGES)
				return id;
		}

		// Slow path: 락 외부에서 새 페이지 메모리/메타데이터 준비.
		// 여러 스레드가 동시에 들어오면 잉여 페이지가 install 될 수 있으나,
		// 잉여 페이지는 partial 리스트에 머무르며 이후 acquire에서 그대로 재사용됨.
		memory_block_page_t prepared;
		if (!prepare_new_page(pool->block_size, pool->blocks_per_page, &prepared))
			return id;

		bool installed = false;
		{
			AUTO_SPIN_LOCK guard(&pool->lock);

			uint16_t page_idx;
			if (install_prepared_page(pool, &prepared, &page_idx))
			{
				installed = true;
				id = pop_block_from_partial_head(pool);
			}
		}

		if (!installed)
			sys_free(prepared.memory); // 페이지 슬롯 한계 도달 → 준비한 메모리 반납.

		return id;
	}

	void release_memory_block(HANDLE handle, memory_block_id id)
	{
		if (id.generation == 0)
			return;

		memory_block_pool_t* pool = static_cast<memory_block_pool_t*>(handle);
		if (!pool)
			return;

		AUTO_SPIN_LOCK guard(&pool->lock);

		const uint16_t count = pool->page_count.load(std::memory_order_relaxed);
		if (id.page_index >= count)
			return;
		if (id.block_index >= pool->blocks_per_page)
			return;

		memory_block_page_t* page = &pool->pages[id.page_index];

		// stale 핸들 / 이중 해제 차단.
		const uint8_t cur_gen = page->generations[id.block_index].load(std::memory_order_relaxed);
		if (cur_gen != id.generation)
			return;

		page->next_free[id.block_index] = page->free_head;
		page->free_head = id.block_index;

		if (!page->in_partial_list)
			push_partial_head(pool, id.page_index);

		page->free_count++;

		// generation bump. 0으로 wrap되면 1로 보정해 invalid 핸들과 구분 유지.
		// release-store: lock-free reader(is_alive/get_memory)가 즉시 stale 인식.
		uint8_t next_gen = static_cast<uint8_t>(cur_gen + 1);
		if (next_gen == 0)
			next_gen = 1;
		page->generations[id.block_index].store(next_gen, std::memory_order_release);
	}

	void* get_memory_block_memory(HANDLE handle, memory_block_id id)
	{
		if (id.generation == 0)
			return nullptr;

		memory_block_pool_t* pool = static_cast<memory_block_pool_t*>(handle);
		if (!pool)
			return nullptr;

		// Lock-free read path.
		// page_count.load(acquire)가 install_prepared_page의 release-store와 페어를 이뤄
		// pages[id.page_index]의 모든 메타데이터를 valid 상태로 보장.
		const uint16_t count = pool->page_count.load(std::memory_order_acquire);
		if (id.page_index >= count)
			return nullptr;
		if (id.block_index >= pool->blocks_per_page)
			return nullptr;

		const memory_block_page_t* page = &pool->pages[id.page_index];
		const uint8_t gen = page->generations[id.block_index].load(std::memory_order_acquire);
		if (gen != id.generation)
			return nullptr;

		uint8_t* base = static_cast<uint8_t*>(page->memory);
		return base + static_cast<size_t>(id.block_index) * pool->block_size;
	}

	bool is_memory_block_alive(HANDLE handle, memory_block_id id)
	{
		if (id.generation == 0)
			return false;

		memory_block_pool_t* pool = static_cast<memory_block_pool_t*>(handle);
		if (!pool)
			return false;

		const uint16_t count = pool->page_count.load(std::memory_order_acquire);
		if (id.page_index >= count)
			return false;
		if (id.block_index >= pool->blocks_per_page)
			return false;

		return pool->pages[id.page_index].generations[id.block_index]
			.load(std::memory_order_acquire) == id.generation;
	}
}
