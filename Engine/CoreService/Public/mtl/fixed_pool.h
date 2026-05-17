#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>
#include <cassert>
#include "allocator_malloc.h"


// ----------------------------------------------------------------------------
// mtl::fixed_pool / fixed_pool_with_overflow / fixed_node_allocator
//
// 동일한 크기의 노드를 매우 빠르게 할당/해제하는 free-list 기반 메모리 풀.
//
// 사용 용도:
//   - 노드 기반 컨테이너(fixed_list 등)의 백엔드 메모리.
//   - 게임 객체의 고정 크기 인스턴스 풀 (총알, 파티클, 데미지 숫자, 사운드 이벤트).
//   - 매 프레임 다수 생성/삭제되는 동일 크기 객체 (이펙트, 짧은 수명 액터).
//   - mi_malloc 호출 자체를 피하고 싶은 핫 패스.
//
// 안 쓰는 게 좋은 경우:
//   - 가변 크기 객체 할당 → mallocator 사용.
//   - 노드 수의 상한이 매우 큰데 동시 사용량은 적은 경우 (낭비).
//   - 다른 객체 수명에 종속된 객체 (배경 메모리가 함께 비워질 위험).
//
// 특성:
//   - O(1) allocate / deallocate (free list 단일 포인터 갱신).
//   - 외부 버퍼 주입형 — 스택/정적/힙 어디서나 사용 가능.
//   - peak_size / current_size 추적 가능.
//   - fixed_pool_with_overflow : 풀이 가득 차면 OverflowAllocator로 fallback.
//   - fixed_node_allocator     : 위 둘을 컴파일 타임 N개 노드로 래핑한 어댑터.
//   - 모든 풀 타입은 비복사/비이동 (자유 리스트 헤드가 버퍼 주소에 묶여 있음).
//
// 사용 예 (직접 사용):
//   alignas(MyNode) std::byte buf[sizeof(MyNode) * 64];
//   mtl::fixed_pool pool(buf, sizeof(buf), sizeof(MyNode), alignof(MyNode));
//   void* p = pool.allocate();
//   ::new (p) MyNode{...};
//   static_cast<MyNode*>(p)->~MyNode();
//   pool.deallocate(p);
//
// 사용 예 (어댑터):
//   mtl::fixed_node_allocator<sizeof(MyNode), 64, alignof(MyNode)> alloc{buf};
// ----------------------------------------------------------------------------


namespace mtl
{
	template <std::size_t Size, std::size_t Alignment>
	struct aligned_buffer
	{
		alignas(Alignment) std::byte data[Size];

		void* ptr() noexcept { return data; }
		const void* ptr() const noexcept { return data; }

		static constexpr std::size_t size() noexcept { return Size; }
		static constexpr std::size_t alignment() noexcept { return Alignment; }

	};

	class default_overflow_allocator
	{
	public:
		void* allocate(
			std::size_t n,
			std::size_t alignment = alignof(std::max_align_t)
		)
		{
			return sys_malloc(n, alignment);
		}

		void deallocate(
			void* p,
			std::size_t n,
			std::size_t alignment = alignof(std::max_align_t)
		) noexcept
		{
			sys_free(p);
		}
	};

	class fixed_pool_base
	{
	public:
		struct Link { Link* next; };

		fixed_pool_base() noexcept = default;

		// 풀은 자유 리스트 헤드와 카운터를 소유하므로 복사/이동을 금지.
		// 복사 시 두 풀이 같은 노드를 관리하게 되어 더블 프리/메모리 손상 발생.
		fixed_pool_base(const fixed_pool_base&) = delete;
		fixed_pool_base(fixed_pool_base&&) = delete;
		fixed_pool_base& operator=(const fixed_pool_base&) = delete;
		fixed_pool_base& operator=(fixed_pool_base&&) = delete;

		void init(
			void* memory,
			std::size_t memory_size,
			std::size_t node_size,
			std::size_t alignment,
			std::size_t alignment_offset = 0
		) noexcept
		{
			assert(memory != nullptr);
			assert(node_size >= sizeof(Link));
			assert(alignment > 0 && (alignment & (alignment - 1)) == 0);

			const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(memory);
			const std::uintptr_t aligned = (base + alignment_offset + alignment - 1) & ~(alignment - 1);
			const std::uintptr_t first_node = aligned - alignment_offset;
			const std::uintptr_t end = base + memory_size;

			const std::size_t available = (end > first_node) ? static_cast<std::size_t>(end - first_node) : 0;
			const std::size_t count = available / node_size;

			m_head = nullptr;
			m_next = reinterpret_cast<Link*>(first_node);
			m_capacity = reinterpret_cast<Link*>(first_node + count * node_size);
			m_node_size = node_size;
			m_current_size = 0;
			m_peak_size = 0;
		}

		bool can_allocate() const noexcept
		{
			return m_head != nullptr || m_next != m_capacity;
		}

		std::size_t node_size() const noexcept { return m_node_size; }
		std::size_t current_size() const noexcept { return m_current_size; }
		std::size_t peak_size() const noexcept { return m_peak_size; }
		std::size_t memory_in_use() const noexcept { return m_current_size * m_node_size; }
		std::size_t peak_memory_in_use() const noexcept { return m_peak_size * m_node_size; }

	protected:
		Link* m_head = nullptr;
		Link* m_next = nullptr;
		Link* m_capacity = nullptr;
		std::size_t m_node_size = 0;
		std::size_t m_current_size = 0;
		std::size_t m_peak_size = 0;

	};


	class fixed_pool : public fixed_pool_base
	{
	public:
		fixed_pool() noexcept = default;

		fixed_pool(
			void* memory,
			std::size_t memory_size,
			std::size_t node_size,
			std::size_t alignment,
			std::size_t alignment_offset = 0) noexcept
		{
			init(memory, memory_size, node_size, alignment, alignment_offset);
		}

		void* allocate() noexcept
		{
			Link* link;
			if (m_head)
			{
				link = m_head;
				m_head = link->next;
			}
			else if (m_next != m_capacity)
			{
				link = m_next;
				m_next = reinterpret_cast<Link*>(reinterpret_cast<std::byte*>(m_next) + m_node_size);
			}
			else
			{
				return nullptr;
			}

			if (++m_current_size > m_peak_size)
				m_peak_size = m_current_size;

			return link;
		}

		void* allocate(std::size_t) noexcept
		{
			return allocate();
		}

		void deallocate(void* p) noexcept
		{
			assert(p != nullptr);
			assert(m_current_size > 0);

			--m_current_size;
			Link* link = static_cast<Link*>(p);
			link->next = m_head;
			m_head = link;
		}
	};



	template <typename OverflowAllocator = default_overflow_allocator>
	class fixed_pool_with_overflow : public fixed_pool_base
	{
	public:
		using overflow_allocator_type = OverflowAllocator;

		fixed_pool_with_overflow() noexcept = default;

		fixed_pool_with_overflow(
			void* memory,
			std::size_t memory_size,
			std::size_t node_size,
			std::size_t alignment,
			std::size_t alignment_offset = 0
		)
		{
			init(memory, memory_size, node_size, alignment, alignment_offset);
		}

		void init(
			void* memory,
			std::size_t memory_size,
			std::size_t node_size,
			std::size_t alignment,
			std::size_t alignment_offset = 0
		)
		{
			m_pool_begin = memory;
			m_pool_end = static_cast<std::byte*>(memory) + memory_size;
			fixed_pool_base::init(memory, memory_size, node_size, alignment, alignment_offset);
		}

		void* allocate()
		{
			return allocate(alignof(std::max_align_t));
		}

		void* allocate(std::size_t alignment)
		{
			Link* link;
			bool from_overflow = false;

			if (m_head)
			{
				link = m_head;
				m_head = link->next;
			}
			else if (m_next != m_capacity)
			{
				link = m_next;
				m_next = reinterpret_cast<Link*>(reinterpret_cast<std::byte*>(m_next) + m_node_size);
			}
			else
			{
				link = static_cast<Link*>(m_overflow_allocator.allocate(m_node_size, alignment));
				from_overflow = true;
			}

			if (link)
			{
				if (++m_current_size > m_peak_size)
					m_peak_size = m_current_size;

				if (from_overflow)
				{
					if (++m_overflow_current > m_overflow_peak)
						m_overflow_peak = m_overflow_current;
				}
			}

			return link;
		}

		// overflow가 있으면 풀이 가득 차도 추가 할당이 가능하므로 항상 true.
		bool can_allocate() const noexcept { return true; }

		void deallocate(void* p)
		{
			assert(p != nullptr);
			assert(m_current_size > 0);
			--m_current_size;

			if (is_in_pool(p))
			{
				Link* link = static_cast<Link*>(p);
				link->next = m_head;
				m_head = link;
			}
			else
			{
				--m_overflow_current;
				m_overflow_allocator.deallocate(p, m_node_size);
			}
		}

		bool is_in_pool(const void* p) const noexcept {
			return p >= m_pool_begin && p < m_pool_end;
		}

		bool has_overflowed() const noexcept { return m_overflow_peak > 0; }
		std::size_t overflow_current() const noexcept { return m_overflow_current; }
		std::size_t overflow_peak() const noexcept { return m_overflow_peak; }

		overflow_allocator_type& get_overflow_allocator() noexcept { return m_overflow_allocator; }
		const overflow_allocator_type& get_overflow_allocator() const noexcept { return m_overflow_allocator; }
		void set_overflow_allocator(const overflow_allocator_type& a) { m_overflow_allocator = a; }

	private:
		void* m_pool_begin = nullptr;
		void* m_pool_end = nullptr;
		overflow_allocator_type m_overflow_allocator{};
		std::size_t             m_overflow_current = 0;
		std::size_t             m_overflow_peak = 0;
	};


	template <
		std::size_t NodeSize,
		std::size_t NodeCount,
		std::size_t NodeAlignment = alignof(std::max_align_t),
		std::size_t NodeAlignmentOffset = 0,
		bool        EnableOverflow = true,
		typename    OverflowAllocator = default_overflow_allocator
	>
	class fixed_node_allocator
	{
	public:
		using pool_type = std::conditional_t<
			EnableOverflow,
			fixed_pool_with_overflow<OverflowAllocator>,
			fixed_pool>;
		using overflow_allocator_type = OverflowAllocator;

		static constexpr std::size_t kNodeSize = NodeSize;
		static constexpr std::size_t kNodeCount = NodeCount;
		static constexpr std::size_t kNodesSize = NodeCount * NodeSize;
		static constexpr std::size_t kBufferSize = kNodesSize + ((NodeAlignment > 1) ? (NodeSize - 1) : 0) + NodeAlignmentOffset;
		static constexpr std::size_t kNodeAlignment = NodeAlignment;
		static constexpr std::size_t kNodeAlignmentOffset = NodeAlignmentOffset;
		static constexpr bool        kEnableOverflow = EnableOverflow;

		fixed_node_allocator() noexcept = default;

		explicit fixed_node_allocator(void* node_buffer)
			: m_pool(node_buffer, kBufferSize, kNodeSize, kNodeAlignment, kNodeAlignmentOffset)
		{
		}

		void* allocate(std::size_t n)
		{
			assert(n == kNodeSize);
			(void)n;
			return m_pool.allocate();
		}

		void* allocate(std::size_t n, std::size_t alignment)
		{
			assert(n == kNodeSize);
			(void)n;

			if constexpr (EnableOverflow)
			{
				return m_pool.allocate(alignment);
			}
			else
			{
				(void)alignment;
				return m_pool.allocate();
			}
		}

		void deallocate(void* p, std::size_t /*n*/ = 0)
		{
			m_pool.deallocate(p);
		}

		bool        can_allocate() const noexcept { return m_pool.can_allocate(); }
		std::size_t current_size() const noexcept { return m_pool.current_size(); }
		std::size_t peak_size() const noexcept { return m_pool.peak_size(); }
		std::size_t memory_in_use() const noexcept { return m_pool.memory_in_use(); }
		std::size_t peak_memory_in_use() const noexcept { return m_pool.peak_memory_in_use(); }

		pool_type& get_pool() noexcept { return m_pool; }
		const pool_type& get_pool() const noexcept { return m_pool; }

	private:
		pool_type m_pool;
	};


	template <std::size_t NS, std::size_t NC, std::size_t NA, std::size_t NAO, bool EO, typename OA>
	inline bool operator==(const fixed_node_allocator<NS, NC, NA, NAO, EO, OA>& a,
		const fixed_node_allocator<NS, NC, NA, NAO, EO, OA>& b) noexcept
	{
		return &a == &b;
	}

	template <std::size_t NS, std::size_t NC, std::size_t NA, std::size_t NAO, bool EO, typename OA>
	inline bool operator!=(const fixed_node_allocator<NS, NC, NA, NAO, EO, OA>& a,
		const fixed_node_allocator<NS, NC, NA, NAO, EO, OA>& b) noexcept
	{
		return &a != &b;
	}

}
