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
// mtl::fixed_list<T, N, EnableOverflow, OverflowAllocator>
//
// std::list 호환 인터페이스의 고정 용량 이중 연결 리스트. 노드 메모리는 내부 풀에서.
//
// 사용 용도:
//   - 중간 삽입/삭제가 빈번한 워크플로 (이벤트 큐, 활성 액터 리스트, undo 스택).
//   - 이터레이터/참조 안정성이 필요한 경우 (다른 원소 추가/삭제가 무효화하지 않음).
//   - 옵저버/콜백 등록 리스트 — 삽입 순서 보존 + 중간 제거.
//   - 작업 큐 (job, command) — 처리 중 새 작업 추가 가능.
//
// 안 쓰는 게 좋은 경우:
//   - 순회가 주된 작업 → fixed_vector가 캐시 친화적으로 훨씬 빠름.
//   - 임의 접근(operator[])이 필요한 경우.
//   - N이 큰데 노드 크기가 작아 prev/next 포인터 오버헤드가 데이터보다 큰 경우.
//
// 특성:
//   - 노드 메모리는 mtl::fixed_node_allocator (내부 풀)에서 가져온다.
//   - EnableOverflow=true (기본): 풀이 가득 차면 OverflowAllocator(기본 sys_malloc)
//     로 추가 노드 할당. "보통은 빠르고, 가끔 안전하게" 동작.
//   - EnableOverflow=false: 풀이 가득 차면 assert.
//   - 삽입/삭제 O(1), 임의 접근 불가.
//
// 사용 예:
//   mtl::fixed_list<int, 16> list;
//   list.push_back(1);
//   list.emplace_front(0);
//   for (int v : list) { ... }
//   if (list.has_overflowed()) { /* 풀 크기 튜닝 */ }
//
// 주의:
//   - 노드 풀은 자기 자신의 버퍼 주소에 묶여 있어 swap이 O(N).
//   - 복사/이동은 원소 단위로 이뤄진다.
//   - splice 미지원 (cross-pool 노드 이동이 안전하지 않아 의도적으로 제외).
// ----------------------------------------------------------------------------

namespace mtl
{
	template <
		typename T,
		std::size_t N,
		bool        EnableOverflow = true,
		typename    OverflowAllocator = default_overflow_allocator
	>
	class fixed_list
	{
	private:
		struct node_base
		{
			node_base* prev;
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
		// Iterator (bidirectional)
		// --------------------------------------------------------------------
		template <bool IsConst>
		class iterator_t
		{
		public:
			using iterator_category = std::bidirectional_iterator_tag;
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
			iterator_t& operator--() noexcept { m_node = m_node->prev; return *this; }
			iterator_t  operator--(int) noexcept { iterator_t t(*this); --(*this); return t; }

			friend bool operator==(const iterator_t& a, const iterator_t& b) noexcept { return a.m_node == b.m_node; }
			friend bool operator!=(const iterator_t& a, const iterator_t& b) noexcept { return a.m_node != b.m_node; }

		private:
			node_base* m_node;
			friend class fixed_list;
			template <bool> friend class iterator_t;
		};

		using iterator = iterator_t<false>;
		using const_iterator = iterator_t<true>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		// --------------------------------------------------------------------
		// 생성/소멸
		// --------------------------------------------------------------------
		fixed_list()
			: m_allocator(m_buffer)
		{
			init_sentinel();
		}

		explicit fixed_list(size_type count)
			: fixed_list()
		{
			for (size_type i = 0; i < count; ++i) emplace_back();
		}

		fixed_list(size_type count, const T& value)
			: fixed_list()
		{
			for (size_type i = 0; i < count; ++i) push_back(value);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_list(InputIt first, InputIt last)
			: fixed_list()
		{
			for (; first != last; ++first) push_back(*first);
		}

		fixed_list(std::initializer_list<T> il)
			: fixed_list(il.begin(), il.end())
		{
		}

		fixed_list(const fixed_list& other)
			: fixed_list(other.begin(), other.end())
		{
		}

		fixed_list(fixed_list&& other)
			: fixed_list()
		{
			for (auto& v : other) push_back(std::move(v));
			other.clear();
		}

		~fixed_list()
		{
			clear();
		}

		fixed_list& operator=(const fixed_list& other)
		{
			if (this != &other) assign(other.begin(), other.end());
			return *this;
		}

		fixed_list& operator=(fixed_list&& other)
		{
			if (this != &other) {
				clear();
				for (auto& v : other) push_back(std::move(v));
				other.clear();
			}
			return *this;
		}

		fixed_list& operator=(std::initializer_list<T> il)
		{
			assign(il.begin(), il.end());
			return *this;
		}

		// --------------------------------------------------------------------
		// assign
		// --------------------------------------------------------------------
		void assign(size_type count, const T& value)
		{
			clear();
			for (size_type i = 0; i < count; ++i) push_back(value);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		void assign(InputIt first, InputIt last)
		{
			clear();
			for (; first != last; ++first) push_back(*first);
		}

		void assign(std::initializer_list<T> il) { assign(il.begin(), il.end()); }

		// --------------------------------------------------------------------
		// iterators
		// --------------------------------------------------------------------
		iterator               begin()         noexcept { return iterator(m_sentinel.next); }
		const_iterator         begin()   const noexcept { return const_iterator(sentinel_ptr()->next); }
		const_iterator         cbegin()  const noexcept { return begin(); }

		iterator               end()           noexcept { return iterator(&m_sentinel); }
		const_iterator         end()     const noexcept { return const_iterator(sentinel_ptr()); }
		const_iterator         cend()    const noexcept { return end(); }

		reverse_iterator       rbegin()        noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator crbegin() const noexcept { return rbegin(); }

		reverse_iterator       rend()          noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
		const_reverse_iterator crend()   const noexcept { return rend(); }

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
		reference       back()        noexcept { assert(!empty()); return *static_cast<node*>(m_sentinel.prev)->value_ptr(); }
		const_reference back()  const noexcept { assert(!empty()); return *static_cast<const node*>(m_sentinel.prev)->value_ptr(); }

		// --------------------------------------------------------------------
		// modifiers
		// --------------------------------------------------------------------
		void clear() noexcept
		{
			node_base* cur = m_sentinel.next;
			while (cur != &m_sentinel)
			{
				node_base* next = cur->next;
				destroy_node(static_cast<node*>(cur));
				cur = next;
			}
			init_sentinel();
			m_size = 0;
		}

		void push_back(const T& value) { emplace(end(), value); }
		void push_back(T&& value) { emplace(end(), std::move(value)); }
		void push_front(const T& value) { emplace(begin(), value); }
		void push_front(T&& value) { emplace(begin(), std::move(value)); }

		template <typename... Args>
		reference emplace_back(Args&&... args)
		{
			return *emplace(end(), std::forward<Args>(args)...);
		}

		template <typename... Args>
		reference emplace_front(Args&&... args)
		{
			return *emplace(begin(), std::forward<Args>(args)...);
		}

		template <typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			node* n = construct_node(std::forward<Args>(args)...);
			assert(n && "fixed_list: 풀이 가득 차고 overflow도 실패");
			link_before(pos.m_node, n);
			++m_size;
			return iterator(n);
		}

		iterator insert(const_iterator pos, const T& value) { return emplace(pos, value); }
		iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }

		iterator insert(const_iterator pos, size_type count, const T& value)
		{
			if (count == 0) return iterator(pos.m_node);
			iterator first = emplace(pos, value);
			for (size_type i = 1; i < count; ++i) emplace(pos, value);
			return first;
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		iterator insert(const_iterator pos, InputIt first, InputIt last)
		{
			if (first == last) return iterator(pos.m_node);
			iterator result = emplace(pos, *first);
			++first;
			for (; first != last; ++first) emplace(pos, *first);
			return result;
		}

		iterator insert(const_iterator pos, std::initializer_list<T> il)
		{
			return insert(pos, il.begin(), il.end());
		}

		void pop_back()  noexcept { assert(!empty()); erase(iterator(m_sentinel.prev)); }
		void pop_front() noexcept { assert(!empty()); erase(iterator(m_sentinel.next)); }

		iterator erase(const_iterator pos) noexcept
		{
			assert(pos.m_node != &m_sentinel && "fixed_list::erase: end() 삭제 불가");
			node_base* target = pos.m_node;
			node_base* next = target->next;
			unlink(target);
			destroy_node(static_cast<node*>(target));
			--m_size;
			return iterator(next);
		}

		iterator erase(const_iterator first, const_iterator last) noexcept
		{
			while (first != last) first = erase(first);  // iterator→const_iterator 변환
			return iterator(last.m_node);
		}

		void resize(size_type new_size)
		{
			while (m_size < new_size) emplace_back();
			while (m_size > new_size) pop_back();
		}

		void resize(size_type new_size, const T& value)
		{
			while (m_size < new_size) push_back(value);
			while (m_size > new_size) pop_back();
		}

		// 풀이 서로 묶여 있어 std::list와 달리 swap은 O(this->size + other.size)다.
		void swap(fixed_list& other)
		{
			if (this == &other) return;
			fixed_list temp(std::move(*this));
			*this = std::move(other);
			other = std::move(temp);
		}

		// --------------------------------------------------------------------
		// list 유틸리티
		// --------------------------------------------------------------------
		size_type remove(const T& value)
		{
			size_type removed = 0;
			for (auto it = begin(); it != end();)
			{
				if (*it == value) { it = erase(it); ++removed; }
				else ++it;
			}
			return removed;
		}

		template <typename Pred>
		size_type remove_if(Pred pred)
		{
			size_type removed = 0;
			for (auto it = begin(); it != end();)
			{
				if (pred(*it)) { it = erase(it); ++removed; }
				else ++it;
			}
			return removed;
		}

		void reverse() noexcept
		{
			node_base* cur = &m_sentinel;
			do
			{
				node_base* tmp = cur->next;
				cur->next = cur->prev;
				cur->prev = tmp;
				cur = tmp;
			} while (cur != &m_sentinel);
		}

	private:
		// --------------------------------------------------------------------
		// internal helpers
		// --------------------------------------------------------------------
		node_base* sentinel_ptr() const noexcept
		{
			return const_cast<node_base*>(&m_sentinel);
		}

		void init_sentinel() noexcept
		{
			m_sentinel.prev = &m_sentinel;
			m_sentinel.next = &m_sentinel;
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

		// pos 앞에 n을 끼워넣는다 (n의 prev/next 설정 포함).
		void link_before(node_base* pos, node_base* n) noexcept
		{
			n->prev = pos->prev;
			n->next = pos;
			pos->prev->next = n;
			pos->prev = n;
		}

		void unlink(node_base* n) noexcept
		{
			n->prev->next = n->next;
			n->next->prev = n->prev;
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
	bool operator==(const fixed_list<T, N1, EO1, OA1>& a, const fixed_list<T, N2, EO2, OA2>& b)
	{
		if (a.size() != b.size()) return false;
		auto ai = a.begin();
		auto bi = b.begin();
		for (; ai != a.end(); ++ai, ++bi)
			if (!(*ai == *bi)) return false;
		return true;
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator!=(const fixed_list<T, N1, EO1, OA1>& a, const fixed_list<T, N2, EO2, OA2>& b)
	{
		return !(a == b);
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator<(const fixed_list<T, N1, EO1, OA1>& a, const fixed_list<T, N2, EO2, OA2>& b)
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
	bool operator>(const fixed_list<T, N1, EO1, OA1>& a, const fixed_list<T, N2, EO2, OA2>& b)
	{
		return b < a;
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator<=(const fixed_list<T, N1, EO1, OA1>& a, const fixed_list<T, N2, EO2, OA2>& b)
	{
		return !(b < a);
	}

	template <typename T, std::size_t N1, std::size_t N2, bool EO1, bool EO2, typename OA1, typename OA2>
	bool operator>=(const fixed_list<T, N1, EO1, OA1>& a, const fixed_list<T, N2, EO2, OA2>& b)
	{
		return !(a < b);
	}

	template <typename T, std::size_t N, bool EO, typename OA>
	void swap(fixed_list<T, N, EO, OA>& a, fixed_list<T, N, EO, OA>& b)
	{
		a.swap(b);
	}

} // namespace mtl
