#pragma once

#include <cstddef>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <limits>
#include <cassert>
#include "fixed_pool.h"


// ----------------------------------------------------------------------------
// mtl::fixed_slist<T, N, EnableOverflow, OverflowAllocator>
//
// std::forward_list 호환 인터페이스의 고정 용량 단방향 연결 리스트. 노드 메모리는
// 내부 풀에서 가져온다. fixed_list 대비 노드당 포인터가 절반(next만) 이므로 같은
// 노드 수에 대해 메모리 풋프린트가 작고, 포인터 갱신 비용도 적다.
//
// 사용 용도:
//   - 단방향 순회로 충분한 자료구조 (옵저버/콜백 체인, 이벤트 핸들러 리스트).
//   - 노드 크기 절약이 중요한 경우 (작은 T — prev 포인터가 데이터보다 큰 경우).
//   - free list / hash chaining bucket / 단방향 큐 등 자료구조 내부 용도.
//   - 후입선출(LIFO) 패턴: push_front / pop_front 만으로 충분.
//
// 안 쓰는 게 좋은 경우:
//   - 역방향 순회 / back() / push_back / pop_back 이 필요 → fixed_list.
//     (fixed_slist에서는 모두 O(n) 이거나 미지원.)
//   - 임의 접근(operator[])이 필요 → fixed_vector.
//   - 순회가 주된 작업이고 데이터가 크지 않다 → fixed_vector가 캐시 친화적으로
//     훨씬 빠르다.
//
// 특성:
//   - 노드 메모리는 mtl::fixed_node_allocator (내부 풀)에서 가져온다.
//   - EnableOverflow=true (기본): 풀이 가득 차면 OverflowAllocator(기본 sys_malloc)
//     로 추가 노드 할당. "보통은 빠르고, 가끔 안전하게" 동작.
//   - EnableOverflow=false: 풀이 가득 차면 assert.
//   - push_front / pop_front / insert_after / emplace_after / erase_after 모두 O(1).
//   - back() / push_back / pop_back / resize 미지원 (구조상 O(n) — 의도적 제외).
//
// 사용 예:
//   mtl::fixed_slist<int, 16> sl;
//   sl.push_front(1);
//   sl.push_front(0);            // 결과: [0, 1]
//   auto it = sl.insert_after(sl.before_begin(), 42);   // 결과: [42, 0, 1]
//   for (int v : sl) { ... }
//   if (sl.has_overflowed()) { /* 풀 크기 튜닝 */ }
//
// 주의:
//   - 노드 풀은 자기 자신의 버퍼 주소에 묶여 있어 swap이 O(this+other).
//   - 복사/이동은 원소 단위로 이뤄진다.
//   - splice_after 미지원 (cross-pool 노드 이동이 안전하지 않아 의도적으로 제외).
//   - end() 는 nullptr 센티넬. before_begin() 은 sentinel 자체를 가리킨다 —
//     dereference 불가, ++ 로 begin() 도달.
// ----------------------------------------------------------------------------

namespace mtl
{
	template <
		typename T,
		std::size_t N,
		bool        EnableOverflow = true,
		typename    OverflowAllocator = default_overflow_allocator
	>
	class fixed_slist
	{
	private:
		struct node_base
		{
			node_base* next;
		};

		struct node : node_base
		{
			alignas(T) std::byte storage[sizeof(T)];

			T* value_ptr() noexcept { return reinterpret_cast<T*>(storage); }
			const T* value_ptr() const noexcept { return reinterpret_cast<const T*>(storage); }
		};

		using allocator_type_internal = fixed_node_allocator<
			sizeof(node),
			N,
			alignof(node),
			0,
			EnableOverflow,
			OverflowAllocator>;

	public:
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		static constexpr size_type kCapacity = N;
		static constexpr bool      kEnableOverflow = EnableOverflow;

		// --------------------------------------------------------------------
		// Iterator (forward)
		// --------------------------------------------------------------------
		template <bool IsConst>
		class iterator_t
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using reference = std::conditional_t<IsConst, const T&, T&>;
			using pointer = std::conditional_t<IsConst, const T*, T*>;

			iterator_t() noexcept : m_node(nullptr) {}
			explicit iterator_t(node_base* n) noexcept : m_node(n) {}

			// non-const → const 변환만 허용
			template <bool C = IsConst, typename = std::enable_if_t<C>>
			iterator_t(const iterator_t<false>& other) noexcept : m_node(other.m_node) {}

			reference operator*()  const noexcept { return *static_cast<node*>(m_node)->value_ptr(); }
			pointer   operator->() const noexcept { return  static_cast<node*>(m_node)->value_ptr(); }

			iterator_t& operator++() noexcept { m_node = m_node->next; return *this; }
			iterator_t  operator++(int) noexcept { iterator_t t(*this); ++(*this); return t; }

			friend bool operator==(const iterator_t& a, const iterator_t& b) noexcept { return a.m_node == b.m_node; }
			friend bool operator!=(const iterator_t& a, const iterator_t& b) noexcept { return a.m_node != b.m_node; }

		private:
			node_base* m_node;
			friend class fixed_slist;
			template <bool> friend class iterator_t;
		};

		using iterator = iterator_t<false>;
		using const_iterator = iterator_t<true>;

		// --------------------------------------------------------------------
		// 생성/소멸
		// --------------------------------------------------------------------
		fixed_slist()
			: m_allocator(m_buffer)
		{
			m_sentinel.next = nullptr;
		}

		explicit fixed_slist(size_type count)
			: fixed_slist()
		{
			iterator tail = before_begin();
			for (size_type i = 0; i < count; ++i) tail = emplace_after(tail);
		}

		fixed_slist(size_type count, const T& value)
			: fixed_slist()
		{
			iterator tail = before_begin();
			for (size_type i = 0; i < count; ++i) tail = emplace_after(tail, value);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_slist(InputIt first, InputIt last)
			: fixed_slist()
		{
			iterator tail = before_begin();
			for (; first != last; ++first) tail = emplace_after(tail, *first);
		}

		fixed_slist(std::initializer_list<T> il)
			: fixed_slist(il.begin(), il.end())
		{
		}

		fixed_slist(const fixed_slist& other)
			: fixed_slist(other.begin(), other.end())
		{
		}

		fixed_slist(fixed_slist&& other)
			: fixed_slist()
		{
			iterator tail = before_begin();
			for (auto& v : other) tail = emplace_after(tail, std::move(v));
			other.clear();
		}

		~fixed_slist()
		{
			clear();
		}

		fixed_slist& operator=(const fixed_slist& other)
		{
			if (this != &other) assign(other.begin(), other.end());
			return *this;
		}

		fixed_slist& operator=(fixed_slist&& other)
		{
			if (this != &other) {
				clear();
				iterator tail = before_begin();
				for (auto& v : other) tail = emplace_after(tail, std::move(v));
				other.clear();
			}
			return *this;
		}

		fixed_slist& operator=(std::initializer_list<T> il)
		{
			assign(il.begin(), il.end());
			return *this;
		}

		// --------------------------------------------------------------------
		// assign
		// --------------------------------------------------------------------
		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		void assign(InputIt first, InputIt last)
		{
			clear();
			iterator tail = before_begin();
			for (; first != last; ++first) tail = emplace_after(tail, *first);
		}

		void assign(std::initializer_list<T> il) { assign(il.begin(), il.end()); }

		// --------------------------------------------------------------------
		// iterators
		// --------------------------------------------------------------------
		iterator       before_begin()        noexcept { return iterator(&m_sentinel); }
		const_iterator before_begin()  const noexcept { return const_iterator(sentinel_ptr()); }
		const_iterator cbefore_begin() const noexcept { return before_begin(); }

		iterator       begin()         noexcept { return iterator(m_sentinel.next); }
		const_iterator begin()   const noexcept { return const_iterator(sentinel_ptr()->next); }
		const_iterator cbegin()  const noexcept { return begin(); }

		iterator       end()           noexcept { return iterator(nullptr); }
		const_iterator end()     const noexcept { return const_iterator(nullptr); }
		const_iterator cend()    const noexcept { return end(); }

		// --------------------------------------------------------------------
		// capacity
		// --------------------------------------------------------------------
		size_type size()  const noexcept { return m_size; }
		bool      empty() const noexcept { return m_size == 0; }

		// 풀이 가득 찼는가? overflow가 켜진 경우라도 true가 되면 이후 삽입은 sys_malloc로 fallback.
		bool      full()  const noexcept { return m_size >= N; }

		static constexpr size_type capacity() noexcept { return N; }
		static constexpr size_type max_size() noexcept
		{
			return EnableOverflow ? (std::numeric_limits<size_type>::max)() : N;
		}

		// 풀 진단: 이번까지 한 번이라도 overflow가 발생했는가?
		bool has_overflowed() const noexcept
		{
			if constexpr (EnableOverflow)
				return m_allocator.get_pool().has_overflowed();
			else
				return false;
		}

		// --------------------------------------------------------------------
		// element access
		// --------------------------------------------------------------------
		reference       front()       noexcept { assert(!empty()); return *static_cast<node*>(m_sentinel.next)->value_ptr(); }
		const_reference front() const noexcept { assert(!empty()); return *static_cast<const node*>(m_sentinel.next)->value_ptr(); }

		// --------------------------------------------------------------------
		// modifiers
		// --------------------------------------------------------------------
		void clear() noexcept
		{
			node_base* cur = m_sentinel.next;
			while (cur)
			{
				node_base* next = cur->next;
				destroy_node(static_cast<node*>(cur));
				cur = next;
			}
			m_sentinel.next = nullptr;
			m_size = 0;
		}

		void push_front(const T& value) { emplace_after(before_begin(), value); }
		void push_front(T&& value) { emplace_after(before_begin(), std::move(value)); }

		bool try_push_front(const T& value) { return try_emplace_after(before_begin(), value); }
		bool try_push_front(T&& value) { return try_emplace_after(before_begin(), std::move(value)); }

		template <typename... Args>
		reference emplace_front(Args&&... args)
		{
			return *emplace_after(before_begin(), std::forward<Args>(args)...);
		}

		template <typename... Args>
		bool try_emplace_front(Args&&... args)
		{
			return try_emplace_after(before_begin(), std::forward<Args>(args)...);
		}

		// pos 뒤에 새 노드를 삽입한다. pos는 before_begin() 또는 임의의 dereference 가능 iterator.
		template <typename... Args>
		iterator emplace_after(const_iterator pos, Args&&... args)
		{
			node* n = construct_node(std::forward<Args>(args)...);
			assert(n && "fixed_slist: 풀이 가득 차고 overflow도 실패");
			if (!n) return iterator(pos.m_node);  // release-safe: 삽입 실패 시 pos 그대로 반환
			n->next = pos.m_node->next;
			pos.m_node->next = n;
			++m_size;
			return iterator(n);
		}

		template <typename... Args>
		bool try_emplace_after(const_iterator pos, Args&&... args)
		{
			node* n = construct_node(std::forward<Args>(args)...);
			if (!n) return false;
			n->next = pos.m_node->next;
			pos.m_node->next = n;
			++m_size;
			return true;
		}

		iterator insert_after(const_iterator pos, const T& value) { return emplace_after(pos, value); }
		iterator insert_after(const_iterator pos, T&& value) { return emplace_after(pos, std::move(value)); }

		// count 개를 pos 뒤에 삽입. 반환값: 마지막으로 삽입된 원소, count==0이면 pos.
		iterator insert_after(const_iterator pos, size_type count, const T& value)
		{
			iterator result(pos.m_node);
			for (size_type i = 0; i < count; ++i)
				result = emplace_after(const_iterator(result.m_node), value);
			return result;
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		iterator insert_after(const_iterator pos, InputIt first, InputIt last)
		{
			iterator result(pos.m_node);
			for (; first != last; ++first)
				result = emplace_after(const_iterator(result.m_node), *first);
			return result;
		}

		iterator insert_after(const_iterator pos, std::initializer_list<T> il)
		{
			return insert_after(pos, il.begin(), il.end());
		}

		void pop_front() noexcept
		{
			assert(!empty());
			erase_after(before_begin());
		}

		// pos 바로 뒤의 원소를 제거. 반환값: 제거된 원소 다음의 iterator.
		iterator erase_after(const_iterator pos) noexcept
		{
			node_base* prev = pos.m_node;
			assert(prev->next && "fixed_slist::erase_after: pos 뒤에 원소가 없음");
			node_base* target = prev->next;
			prev->next = target->next;
			destroy_node(static_cast<node*>(target));
			--m_size;
			return iterator(prev->next);
		}

		// (first, last) 범위 — first 다음부터 last 직전까지 제거. 반환값: last 위치 iterator.
		iterator erase_after(const_iterator first, const_iterator last) noexcept
		{
			node_base* prev = first.m_node;
			node_base* cur = prev->next;
			while (cur != last.m_node)
			{
				node_base* next = cur->next;
				destroy_node(static_cast<node*>(cur));
				--m_size;
				cur = next;
			}
			prev->next = last.m_node;
			return iterator(last.m_node);
		}

		// 풀이 서로 묶여 있어 std::forward_list와 달리 swap은 O(this->size + other.size)다.
		void swap(fixed_slist& other)
		{
			if (this == &other) return;
			fixed_slist temp(std::move(*this));
			*this = std::move(other);
			other = std::move(temp);
		}

		// --------------------------------------------------------------------
		// list 유틸리티
		// --------------------------------------------------------------------
		size_type remove(const T& value)
		{
			size_type removed = 0;
			node_base* prev = &m_sentinel;
			while (prev->next)
			{
				node* cur = static_cast<node*>(prev->next);
				if (*cur->value_ptr() == value)
				{
					prev->next = cur->next;
					destroy_node(cur);
					--m_size;
					++removed;
				}
				else
				{
					prev = cur;
				}
			}
			return removed;
		}

		template <typename Pred>
		size_type remove_if(Pred pred)
		{
			size_type removed = 0;
			node_base* prev = &m_sentinel;
			while (prev->next)
			{
				node* cur = static_cast<node*>(prev->next);
				if (pred(*cur->value_ptr()))
				{
					prev->next = cur->next;
					destroy_node(cur);
					--m_size;
					++removed;
				}
				else
				{
					prev = cur;
				}
			}
			return removed;
		}

		void reverse() noexcept
		{
			node_base* prev = nullptr;
			node_base* cur = m_sentinel.next;
			while (cur)
			{
				node_base* next = cur->next;
				cur->next = prev;
				prev = cur;
				cur = next;
			}
			m_sentinel.next = prev;
		}

	private:
		// --------------------------------------------------------------------
		// internal helpers
		// --------------------------------------------------------------------
		node_base* sentinel_ptr() const noexcept
		{
			return const_cast<node_base*>(&m_sentinel);
		}

		node* allocate_node()
		{
			return static_cast<node*>(m_allocator.allocate(sizeof(node), alignof(node)));
		}

		void deallocate_node(node* n) noexcept
		{
			m_allocator.deallocate(n, sizeof(node));
		}

		template <typename... Args>
		node* construct_node(Args&&... args)
		{
			node* n = allocate_node();
			if (!n) return nullptr;
			::new (n->value_ptr()) T(std::forward<Args>(args)...);
			return n;
		}

		void destroy_node(node* n) noexcept
		{
			n->value_ptr()->~T();
			deallocate_node(n);
		}

		// --------------------------------------------------------------------
		// data
		// --------------------------------------------------------------------
		alignas(node) std::byte    m_buffer[allocator_type_internal::kBufferSize];
		allocator_type_internal    m_allocator;
		node_base                  m_sentinel;
		size_type                  m_size = 0;
	};

	// ----------------------------------------------------------------------------
	// 비교 연산자
	// ----------------------------------------------------------------------------
	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator==(const fixed_slist<T, N1, EO1, OA1>& a, const fixed_slist<T, N2, EO2, OA2>& b)
	{
		if (a.size() != b.size()) return false;
		auto ai = a.begin();
		auto bi = b.begin();
		for (; ai != a.end(); ++ai, ++bi)
			if (!(*ai == *bi)) return false;
		return true;
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator!=(const fixed_slist<T, N1, EO1, OA1>& a, const fixed_slist<T, N2, EO2, OA2>& b)
	{
		return !(a == b);
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator<(const fixed_slist<T, N1, EO1, OA1>& a, const fixed_slist<T, N2, EO2, OA2>& b)
	{
		auto ai = a.begin();
		auto bi = b.begin();
		for (; ai != a.end() && bi != b.end(); ++ai, ++bi)
		{
			if (*ai < *bi) return true;
			if (*bi < *ai) return false;
		}
		return ai == a.end() && bi != b.end();
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator>(const fixed_slist<T, N1, EO1, OA1>& a, const fixed_slist<T, N2, EO2, OA2>& b)
	{
		return b < a;
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator<=(const fixed_slist<T, N1, EO1, OA1>& a, const fixed_slist<T, N2, EO2, OA2>& b)
	{
		return !(b < a);
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator>=(const fixed_slist<T, N1, EO1, OA1>& a, const fixed_slist<T, N2, EO2, OA2>& b)
	{
		return !(a < b);
	}

	template <typename T, std::size_t N, bool EO, typename OA>
	void swap(fixed_slist<T, N, EO, OA>& a, fixed_slist<T, N, EO, OA>& b)
	{
		a.swap(b);
	}

} // namespace mtl
