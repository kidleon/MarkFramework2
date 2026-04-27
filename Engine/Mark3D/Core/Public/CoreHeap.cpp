#include "pch.h"
#include "CoreHeap.h"
#include "SpinLock.h"


#if defined(__MEMORY_TRACKING_ENABLED__)
#include "MemoryTracker.h"
#define MEM_ON_ALLOCATE(ptr, bytes, alignment, loc) mark::memory_tracker::get().on_allocate(ptr, bytes, alignment, loc)
#define MEM_ON_DEALLOCATE(ptr) mark::memory_tracker::get().on_deallocate(ptr)
#else
#define MEM_ON_ALLOCATE(ptr, bytes, alignment, loc) ((void)0)
#define MEM_ON_DEALLOCATE(ptr) ((void)0)
#endif // __MEMORY_TRACKING_ENABLED__

namespace mark
{
	constexpr size_t DEFAULT_ALIGNMENT = alignof(std::max_align_t);

	/*
	class sync_pool_memory_resource final : public std::pmr::memory_resource
	{
		spin_lock_t m_spin_lock;
		std::pmr::unsynchronized_pool_resource m_pool;

	public:
		explicit sync_pool_memory_resource(const std::pmr::pool_options& options = std::pmr::pool_options())
			: m_pool(options, std::pmr::get_default_resource())
		{
			init_spin_lock(&m_spin_lock);
		}

	private:
		inline void* do_allocate(size_t bytes, size_t alignment) final
		{
			AUTO_SPIN_LOCK auto_lock(&m_spin_lock);
			return m_pool.allocate(bytes, alignment);
		}

		inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
			AUTO_SPIN_LOCK auto_lock(&m_spin_lock);
			m_pool.deallocate(ptr, bytes, alignment);
		}

		inline bool do_is_equal(const memory_resource& other) const noexcept final
		{
			return this == &other;
		}
	};
	*/


#if defined(__cpp_lib_hardware_interference_size)
	inline constexpr std::size_t LFPMR_CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
#else
	inline constexpr std::size_t LFPMR_CACHE_LINE_SIZE = 64;
#endif

	class lockfree_pool_resource : public std::pmr::memory_resource
	{
	public:
		static constexpr std::size_t SIZE_CLASS_COUNT = 9;

		/**
		* @brief 미리 정의된 size class 목록 (byte 단위). 2배수 ladder 구조.
		*/
		//static constexpr std::array<std::size_t, SIZE_CLASS_COUNT> SIZE_CLASSES = {
		static constexpr std::size_t SIZE_CLASSES[SIZE_CLASS_COUNT] = {
			16, 32, 64, 128, 256, 512, 1024, 2048, 4096
		};

		/**
		* @brief 풀이 비었을 때 upstream에서 한 번에 받아올 청크 크기 (byte).
		* 청크는 size class 단위로 잘려 free-list에 채워짐.
		*/
		static constexpr std::size_t CHUNK_SIZE = 64 * 1024;

		static constexpr std::size_t MAX_POOLED_BYTES = SIZE_CLASSES[SIZE_CLASS_COUNT - 1];

		/**
		* @brief 풀이 처리 가능한 최대 정렬. 이를 초과하는 정렬 요청은 upstream으로 직접 위임됨.
		*/
		static constexpr std::size_t MAX_POOLED_ALIGNMENT = alignof(std::max_align_t);

	private:
		struct free_node;

		/**
		* @brief 풀에 들어있는 빈 슬롯의 첫 word는 다음 노드 포인터로 사용됨.
		* @details std::atomic 객체를 직접 두지 않고, 슬롯 메모리의 첫 sizeof(void*) byte를
		* std::atomic_ref로 감싸 atomic 연산을 수행함. 이 방식은 사용자가 메모리를 어떻게 사용했든 (raw bytes 등)
		* 풀로 돌아온 시점에 atomic 의미론을 부여할 수 있어 placement new 호출이 불필요.
		*/
		using free_link_t = free_node*;

		struct free_node
		{
			free_link_t next;
		};

		[[nodiscard]] static std::atomic_ref<free_link_t> link_ref(free_node* n) noexcept
		{
			return std::atomic_ref<free_link_t>(n->next);
		}

		/**
		* @brief tagged_ptr는 ABA 방지용. 하위 48bit 포인터, 상위 16bit 태그.
		*/
		struct tagged_ptr
		{
			std::uintptr_t raw{ 0 };

			static constexpr std::uintptr_t PTR_MASK = 0x0000'FFFF'FFFF'FFFFULL;
			static constexpr std::uintptr_t TAG_SHIFT = 48;

			[[nodiscard]] inline free_node* ptr() const noexcept
			{
				return reinterpret_cast<free_node*>(raw & PTR_MASK);
			}

			[[nodiscard]] inline std::uint16_t tag() const noexcept
			{
				return static_cast<std::uint16_t>(raw >> TAG_SHIFT);
			}

			[[nodiscard]] static inline tagged_ptr make(free_node* p, std::uint16_t t) noexcept
			{
				tagged_ptr r;
				r.raw = (reinterpret_cast<std::uintptr_t>(p) & PTR_MASK)
					| (static_cast<std::uintptr_t>(t) << TAG_SHIFT);
				return r;
			}
		};

		static_assert(sizeof(tagged_ptr) == sizeof(std::uintptr_t), "tagged_ptr must fit in single word");

		/**
		* @brief 단일 size class의 lock-free free-list (Treiber stack).
		*/
		struct alignas(LFPMR_CACHE_LINE_SIZE) size_class_pool
		{
			std::atomic<tagged_ptr> top{ tagged_ptr{} };
			std::atomic<std::size_t> free_count{ 0 };
		};

		struct chunk_header
		{
			chunk_header* next;
			std::size_t bytes;
			std::size_t alignment;
		};

		//std::array<size_class_pool, SIZE_CLASS_COUNT> _pools;
		size_class_pool _pools[SIZE_CLASS_COUNT];

		alignas(LFPMR_CACHE_LINE_SIZE) std::atomic<chunk_header*> _chunks{ nullptr };

		std::pmr::memory_resource* _upstream;

	public:
		/**
		* @brief 생성자.
		* @param upstream 풀 미스 시 위임할 상위 resource. nullptr이면 std::pmr::get_default_resource() 사용.
		*/
		explicit lockfree_pool_resource(std::pmr::memory_resource* upstream = nullptr) noexcept
			: _upstream(upstream ? upstream : std::pmr::get_default_resource())
		{
		}

		~lockfree_pool_resource() override
		{
			chunk_header* c = _chunks.load(std::memory_order_relaxed);
			while (c)
			{
				chunk_header* next = c->next;
				_upstream->deallocate(c, c->bytes, c->alignment);
				c = next;
			}
		}

		lockfree_pool_resource(const lockfree_pool_resource&) = delete;
		lockfree_pool_resource& operator=(const lockfree_pool_resource&) = delete;
		lockfree_pool_resource(lockfree_pool_resource&&) = delete;
		lockfree_pool_resource& operator=(lockfree_pool_resource&&) = delete;

		[[nodiscard]] std::pmr::memory_resource* upstream_resource() const noexcept
		{
			return _upstream;
		}

		/**
		* @brief 디버그/통계용. 특정 size class의 현재 free-list 크기 반환.
		*/
		[[nodiscard]] std::size_t pool_size(std::size_t size_class_index) const noexcept
		{
			if (size_class_index >= SIZE_CLASS_COUNT) return 0;
			return _pools[size_class_index].free_count.load(std::memory_order_relaxed);
		}

	protected:
		void* do_allocate(std::size_t bytes, std::size_t alignment) override
		{
			if (bytes > MAX_POOLED_BYTES || alignment > MAX_POOLED_ALIGNMENT)
			{
				return _upstream->allocate(bytes, alignment);
			}

			const std::size_t idx = size_class_index(bytes, alignment);
			void* p = pop_from_pool(idx);
			if (p) return p;

			refill_pool(idx);

			p = pop_from_pool(idx);
			if (p) return p;

			return _upstream->allocate(bytes, alignment);
		}

		void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
		{
			if (!p) return;

			if (bytes > MAX_POOLED_BYTES || alignment > MAX_POOLED_ALIGNMENT)
			{
				_upstream->deallocate(p, bytes, alignment);
				return;
			}

			const std::size_t idx = size_class_index(bytes, alignment);
			push_to_pool(idx, p);
		}

		[[nodiscard]] bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
		{
			return this == &other;
		}

	private:
		/**
		* @brief 요청된 (bytes, alignment)에 맞는 size class index를 반환.
		* alignment 요건도 만족해야 하므로 size와 alignment 중 큰 값을 기준으로 매핑.
		*/
		[[nodiscard]] static std::size_t size_class_index(std::size_t bytes, std::size_t alignment) noexcept
		{
			std::size_t needed = bytes;
			if (alignment > needed) needed = alignment;
			if (needed < SIZE_CLASSES[0]) needed = SIZE_CLASSES[0];

			for (std::size_t i = 0; i < SIZE_CLASS_COUNT; ++i)
			{
				if (SIZE_CLASSES[i] >= needed) return i;
			}
			return SIZE_CLASS_COUNT - 1;
		}

		/**
		* @brief Treiber stack pop. 풀이 비면 nullptr.
		*/
		void* pop_from_pool(std::size_t idx) noexcept
		{
			size_class_pool& pool = _pools[idx];
			tagged_ptr old_top = pool.top.load(std::memory_order_acquire);

			for (;;)
			{
				free_node* head = old_top.ptr();
				if (!head) return nullptr;

				free_node* nxt = link_ref(head).load(std::memory_order_acquire);

				tagged_ptr new_top = tagged_ptr::make(nxt, static_cast<std::uint16_t>(old_top.tag() + 1));

				if (pool.top.compare_exchange_weak(
					old_top, new_top,
					std::memory_order_acq_rel,
					std::memory_order_acquire))
				{
					pool.free_count.fetch_sub(1, std::memory_order_relaxed);
					return static_cast<void*>(head);
				}
			}
		}

		void push_to_pool(std::size_t idx, void* mem) noexcept
		{
			size_class_pool& pool = _pools[idx];
			free_node* node = static_cast<free_node*>(mem);

			tagged_ptr old_top = pool.top.load(std::memory_order_relaxed);

			for (;;)
			{
				link_ref(node).store(old_top.ptr(), std::memory_order_relaxed);

				tagged_ptr new_top = tagged_ptr::make(node, static_cast<std::uint16_t>(old_top.tag() + 1));

				if (pool.top.compare_exchange_weak(
					old_top, new_top,
					std::memory_order_release,
					std::memory_order_relaxed))
				{
					pool.free_count.fetch_add(1, std::memory_order_relaxed);
					return;
				}
			}
		}

		/**
		* @brief 청크를 upstream에서 받아와 size class 단위로 잘라 free-list에 채움.
		* 청크 자체는 _chunks 리스트에 등록되어 소멸 시 해제됨.
		*/
		void refill_pool(std::size_t idx)
		{
			const std::size_t slot_size = SIZE_CLASSES[idx];
			const std::size_t alignment = slot_size;

			std::size_t chunk_bytes = CHUNK_SIZE;
			if (chunk_bytes < slot_size + sizeof(chunk_header) + alignment)
			{
				chunk_bytes = slot_size + sizeof(chunk_header) + alignment;
			}

			void* raw = _upstream->allocate(chunk_bytes, alignment);

			chunk_header* hdr = static_cast<chunk_header*>(raw);
			hdr->bytes = chunk_bytes;
			hdr->alignment = alignment;

			chunk_header* old_head = _chunks.load(std::memory_order_relaxed);
			do
			{
				hdr->next = old_head;
			}
			while (!_chunks.compare_exchange_weak(
				old_head,
				hdr,
				std::memory_order_release,
				std::memory_order_relaxed
			));

			std::byte* base = static_cast<std::byte*>(raw);
			std::size_t header_offset = sizeof(chunk_header);
			std::size_t mis = header_offset % alignment;
			if (mis != 0) header_offset += (alignment - mis);

			std::byte* cur = base + header_offset;
			std::byte* end = base + chunk_bytes;

			while (cur + slot_size <= end)
			{
				push_to_pool(idx, cur);
				cur += slot_size;
			}
		}
	};

	class usync_pool_memory_resource final : public std::pmr::unsynchronized_pool_resource
	{
	public:
		using std::pmr::unsynchronized_pool_resource::unsynchronized_pool_resource; // 부모 클래스의 생성자 상속
	};

	class temp_pool_memory_resource final : public std::pmr::memory_resource
	{
	public:
		explicit temp_pool_memory_resource(size_t buffer_size)
			: m_buffer((char*)::malloc(buffer_size))
			, m_buffer_size(buffer_size)
			, m_used(0)
			, m_owns_buffer(true)
		{
			if (!m_buffer) [[unlikely]]
				throw std::bad_alloc();
			MEM_ON_ALLOCATE(m_buffer, buffer_size, DEFAULT_ALIGNMENT, std::source_location::current());
		}

		explicit temp_pool_memory_resource(void* buffer, size_t buffer_size)
			: m_buffer((char*)buffer)
			, m_buffer_size(buffer_size)
			, m_used(0)
			, m_owns_buffer(false)
		{
		}

		virtual ~temp_pool_memory_resource()
		{
			if (m_owns_buffer && m_buffer)
			{
				MEM_ON_DEALLOCATE(m_buffer);
				::free(m_buffer);
				m_buffer = nullptr;
			}
		}

		inline void release()
		{
			m_used = 0;
		}

	private:
		inline void* do_allocate(size_t bytes, size_t alignment) final
		{
			if (!m_buffer) [[unlikely]]
			{
				assert(false && "Temp pool buffer is not initialized");
				return nullptr;
			}

			// 현재 포인터를 alignment 경계에 맞게 올림
			auto current = reinterpret_cast<std::uintptr_t>(m_buffer + m_used);
			auto aligned = (current + alignment - 1) & ~(alignment - 1);
			auto offset = aligned - reinterpret_cast<std::uintptr_t>(m_buffer);

			if (offset + bytes > m_buffer_size)
			{
				assert(false && "Temp pool buffer overflow");
				return nullptr;
			}

			m_used = offset + bytes;

			return reinterpret_cast<void*>(aligned);
		}
			
		inline void do_deallocate(void* ptr, size_t bytes, size_t alignment) final
		{
			// bump-style arena allocator이므로 개별 deallocate는 의도적으로 무시.
			// 메모리는 release() 호출 또는 객체 파괴 시 한꺼번에 회수된다.
			// PMR 인터페이스 규약상 빈 구현이 허용된다.
			(void)ptr; (void)bytes; (void)alignment;
		}

		inline bool do_is_equal(const memory_resource& other) const noexcept final
		{
			return this == &other;
		}

	private:
		char* m_buffer = nullptr;
		size_t m_buffer_size = 0;
		size_t m_used = 0;
		bool m_owns_buffer = true;

	};

	using sync_pool_memory_resource = lockfree_pool_resource;

	static sync_pool_memory_resource* s_default_sync_pool_memory_resource = nullptr;
	static usync_pool_memory_resource* s_default_usync_pool_memory_resource = nullptr;
	static size_t s_temp_buffer_size = 1024 * 1024 * 32; // 기본값

	struct thread_local_temp_memory_resource
	{
		temp_pool_memory_resource* resource = nullptr;
		~thread_local_temp_memory_resource()
		{
			if (resource)
			{
				delete resource;
				resource = nullptr;
			}
		}

		temp_pool_memory_resource* get()
		{
			if (!resource)
				resource = new temp_pool_memory_resource(s_temp_buffer_size);
			return resource;
		}
	};
	
	thread_local thread_local_temp_memory_resource s_default_temp_memory_resource; // 32MB 임시 버퍼
	std::atomic<bool> is_core_memory_initialized = false;

	bool initialize_core_memory(
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	)
	{
		if (is_core_memory_initialized.load()) return false;

		try
		{
			//s_default_sync_pool_memory_resource = new sync_pool_memory_resource({ sync_pool_count_per_chunk, sync_pool_max_size_per_block });
			s_default_sync_pool_memory_resource = new sync_pool_memory_resource(std::pmr::get_default_resource());
			s_default_usync_pool_memory_resource = new usync_pool_memory_resource({ unsync_pool_count_per_chunk, unsync_pool_max_size_per_block });
			s_temp_buffer_size = temp_buffer_size;
			is_core_memory_initialized = true;
		}
		catch (const std::exception& e)
		{
			assert(false && e.what());
			shutdown_core_memory();
			return false;
		}

		return true;
	}

	void shutdown_core_memory()
	{
		if (s_default_usync_pool_memory_resource)
		{
			delete s_default_usync_pool_memory_resource;
			s_default_usync_pool_memory_resource = nullptr;
		}

		if (s_default_sync_pool_memory_resource)
		{
			delete s_default_sync_pool_memory_resource;
			s_default_sync_pool_memory_resource = nullptr;
		}
		is_core_memory_initialized = false;
	}

	void* sys_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* p = std::pmr::get_default_resource()->allocate(bytes, alignment); // 시스템 메모리 리소스에서 할당 (기본 리소스는 system_memory_resource)
		MEM_ON_ALLOCATE(p, bytes, alignment, loc);

		return p;
	}

	void* spool_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* p = s_default_sync_pool_memory_resource->allocate(bytes, alignment);
		MEM_ON_ALLOCATE(p, bytes, alignment, loc);

		return p;
	}
	void* upool_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* p = s_default_usync_pool_memory_resource->allocate(bytes, alignment);
		MEM_ON_ALLOCATE(p, bytes, alignment, loc);

		return p;
	}

	void* sys_alloc(size_t bytes, size_t alignment)
	{
		return std::pmr::get_default_resource()->allocate(bytes, alignment); // 시스템 메모리 리소스에서 할당 (기본 리소스는 system_memory_resource)
	}

	void* spool_alloc(size_t bytes, size_t alignment)
	{
		return s_default_sync_pool_memory_resource->allocate(bytes, alignment);
	}

	void* upool_alloc(size_t bytes, size_t alignment)
	{
		return s_default_usync_pool_memory_resource->allocate(bytes, alignment);
	}

	void* temp_alloc(size_t bytes, size_t alignment)
	{
		return s_default_temp_memory_resource.get()->allocate(bytes, alignment);
	}

	void temp_reset()
	{
		s_default_temp_memory_resource.get()->release();
	}

	void sys_free(void* ptr, size_t bytes, size_t alignment)
	{
		MEM_ON_DEALLOCATE(ptr);

		std::pmr::get_default_resource()->deallocate(ptr, bytes, alignment); // 시스템 메모리 리소스에서 해제 (기본 리소스는 system_memory_resource)
	}

	void spool_free(void* ptr, size_t bytes, size_t alignment)
	{
		MEM_ON_DEALLOCATE(ptr);

		s_default_sync_pool_memory_resource->deallocate(ptr, bytes, alignment);
	}

	void upool_free(void* ptr, size_t bytes, size_t alignment)
	{
		MEM_ON_DEALLOCATE(ptr);

		s_default_usync_pool_memory_resource->deallocate(ptr, bytes, alignment);
	}

	std::pmr::memory_resource* get_default_system_memory_resource_ptr() noexcept
	{
		return std::pmr::get_default_resource(); // 시스템 메모리 리소스 반환 (기본 리소스는 system_memory_resource)
	}

	std::pmr::memory_resource* get_default_spool_memory_resource_ptr() noexcept
	{
		return s_default_sync_pool_memory_resource;
	}

	std::pmr::memory_resource* get_default_usync_pool_memory_resource_ptr() noexcept
	{
		return s_default_usync_pool_memory_resource;
	}

	std::pmr::memory_resource* get_default_temp_memory_resource_ptr() noexcept
	{
		return s_default_temp_memory_resource.get();
	}
}
