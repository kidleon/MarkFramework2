#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cassert>
#include <functional>


// ----------------------------------------------------------------------------
// mtl::intrusive_list<T>
//
// 노드(prev/next 포인터)가 원소 자체에 임베드된 양방향 연결 리스트.
// 컨테이너는 메모리를 할당/소유하지 않는다 — T 객체의 수명은 호출자 책임.
//
// 사용 용도:
//   - 객체가 여러 자료구조에 동시 등록되거나, 동적 할당 없이 link/unlink만
//     반복되는 경우. (활성 액터 목록, 씬 그래프, GPU 리소스 라이프타임 트래커
//     등 — 객체 본체는 풀에서 한 번만 살고, 다양한 리스트에 자유롭게 등록.)
//   - 노드 포인터만으로 O(1) 분리가 필요한 경우 — 이터레이터를 들고 다닐
//     필요가 없다. T의 주소만 알면 unlink 가능.
//   - "이 리스트는 노드 소유권을 가지지 않는다"는 의미를 타입에서 명확히
//     드러내고 싶을 때.
//
// 안 쓰는 게 좋은 경우:
//   - 원소의 수명을 컨테이너가 관리해주길 원할 때 → fixed_list 사용.
//   - 한 객체가 동시에 여러 리스트의 멤버여야 하지만 prev/next가 하나뿐일
//     때. (현재 버전은 1개 리스트 멤버십만 지원. 다중 멤버십은 사용자가
//     원하는 만큼 intrusive_list_node를 멤버/베이스로 추가하고 별도 Tag 기반
//     컨테이너를 만드는 식으로 확장 — 향후 후보.)
//
// 사용 예:
//   struct task : mtl::intrusive_list_node { int priority; };
//
//   mtl::intrusive_list<task> active;
//   task a, b, c;
//   active.push_back(a);
//   active.push_back(b);
//   active.push_back(c);
//
//   active.unlink(b);                     // 이터레이터 없이 O(1) 분리
//   for (auto& t : active) { ... }
//
// 특성:
//   - 메모리 할당 없음. 노드 메모리는 호출자가 관리.
//   - 복사 = 삭제 (delete). 두 인스턴스가 같은 노드를 공유하는 위험을 차단.
//   - 이동은 sentinel-fixup을 동반한 swap 의미.
//   - O(1) push/pop/insert/erase/unlink.
//   - splice도 단일/전체는 O(1), 구간 splice만 O(거리) (size 카운터 갱신용).
//
// 주의:
//   - T는 mtl::intrusive_list_node를 **public 상속**해야 한다 (static_assert).
//   - 동일 노드를 이미 어떤 리스트의 멤버인 상태에서 다시 push_*하면 UB.
//     디버그 빌드에서는 is_linked 체크로 즉시 assert.
//   - 컨테이너 파괴 시 남아있는 노드들의 link는 자동으로 끊긴다(clear). 그
//     이후 노드를 그대로 다시 사용해도 안전.
// ----------------------------------------------------------------------------

namespace mtl
{
	// ------------------------------------------------------------------------
	// intrusive_list_node
	//
	// 컨테이너의 멤버가 되고 싶은 타입이 public 상속해야 하는 노드. 두 개의
	// 포인터로 구성된 단순 본체.
	//
	// 복사/이동 의미: 노드는 주소가 곧 정체성이다. 복사 시 새 객체는 어떤
	// 리스트에도 속하지 않은 상태로 시작하고, 대입은 자신의 link 상태에
	// 영향을 주지 않는다(no-op). 이 의미 덕분에 사용자 정의 T가 기본 복사/이동
	// 연산을 자유롭게 가질 수 있다.
	// ------------------------------------------------------------------------
	struct intrusive_list_node
	{
		intrusive_list_node* m_next;
		intrusive_list_node* m_prev;

		intrusive_list_node() noexcept
			: m_next(nullptr), m_prev(nullptr) {}

		intrusive_list_node(const intrusive_list_node&) noexcept
			: m_next(nullptr), m_prev(nullptr) {}

		intrusive_list_node(intrusive_list_node&&) noexcept
			: m_next(nullptr), m_prev(nullptr) {}

		intrusive_list_node& operator=(const intrusive_list_node&) noexcept { return *this; }
		intrusive_list_node& operator=(intrusive_list_node&&)      noexcept { return *this; }
	};

	namespace detail
	{
		template <typename T>
		inline constexpr bool is_intrusive_list_compatible_v =
			std::is_base_of_v<intrusive_list_node, T>;
	}


	template <typename T = intrusive_list_node>
	class intrusive_list
	{
		static_assert(detail::is_intrusive_list_compatible_v<T>,
			"intrusive_list<T>: T는 mtl::intrusive_list_node를 public 상속해야 합니다");

	public:
		using value_type      = T;
		using size_type       = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference       = T&;
		using const_reference = const T&;
		using pointer         = T*;
		using const_pointer   = const T*;

		// --------------------------------------------------------------------
		// Iterator (bidirectional)
		// --------------------------------------------------------------------
		template <bool IsConst>
		class iterator_t
		{
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type        = T;
			using difference_type   = std::ptrdiff_t;
			using reference         = std::conditional_t<IsConst, const T&, T&>;
			using pointer           = std::conditional_t<IsConst, const T*, T*>;

			iterator_t() noexcept : m_node(nullptr) {}
			explicit iterator_t(intrusive_list_node* n) noexcept : m_node(n) {}

			// non-const → const 변환만 허용
			template <bool C = IsConst, typename = std::enable_if_t<C>>
			iterator_t(const iterator_t<false>& other) noexcept : m_node(other.m_node) {}

			reference operator*()  const noexcept { return *static_cast<T*>(m_node); }
			pointer   operator->() const noexcept { return  static_cast<T*>(m_node); }

			iterator_t& operator++() noexcept { m_node = m_node->m_next; return *this; }
			iterator_t  operator++(int) noexcept { iterator_t t(*this); ++(*this); return t; }
			iterator_t& operator--() noexcept { m_node = m_node->m_prev; return *this; }
			iterator_t  operator--(int) noexcept { iterator_t t(*this); --(*this); return t; }

			friend bool operator==(const iterator_t& a, const iterator_t& b) noexcept { return a.m_node == b.m_node; }
			friend bool operator!=(const iterator_t& a, const iterator_t& b) noexcept { return a.m_node != b.m_node; }

		private:
			intrusive_list_node* m_node;
			friend class intrusive_list;
			template <bool> friend class iterator_t;
		};

		using iterator               = iterator_t<false>;
		using const_iterator         = iterator_t<true>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		// --------------------------------------------------------------------
		// 생성/소멸 (복사 없음 / 이동 = swap)
		// --------------------------------------------------------------------
		intrusive_list() noexcept { init_sentinel(); }

		intrusive_list(const intrusive_list&)            = delete;
		intrusive_list& operator=(const intrusive_list&) = delete;

		intrusive_list(intrusive_list&& other) noexcept
		{
			init_sentinel();
			swap(other);
		}

		intrusive_list& operator=(intrusive_list&& other) noexcept
		{
			if (this != &other) {
				clear();
				swap(other);
			}
			return *this;
		}

		~intrusive_list() noexcept { clear(); }

		// --------------------------------------------------------------------
		// iterators
		// --------------------------------------------------------------------
		iterator               begin()         noexcept { return iterator(m_sentinel.m_next); }
		const_iterator         begin()   const noexcept { return const_iterator(sentinel_ptr()->m_next); }
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
		bool      empty() const noexcept { return m_sentinel.m_next == &m_sentinel; }

		// --------------------------------------------------------------------
		// element access
		// --------------------------------------------------------------------
		reference       front()       noexcept { assert(!empty()); return *static_cast<T*>(m_sentinel.m_next); }
		const_reference front() const noexcept { assert(!empty()); return *static_cast<const T*>(m_sentinel.m_next); }
		reference       back()        noexcept { assert(!empty()); return *static_cast<T*>(m_sentinel.m_prev); }
		const_reference back()  const noexcept { assert(!empty()); return *static_cast<const T*>(m_sentinel.m_prev); }

		// --------------------------------------------------------------------
		// 노드 ↔ 이터레이터 변환 / 멤버십 질의
		// --------------------------------------------------------------------

		// 노드의 주소만으로 해당 위치의 iterator를 얻는다. O(1).
		// 노드가 어떤 리스트에도 등록되어 있지 않으면 assert.
		iterator iterator_to(reference v) noexcept
		{
			assert(is_linked(v) && "intrusive_list::iterator_to: 노드가 어떤 리스트에도 속해있지 않음");
			return iterator(static_cast<intrusive_list_node*>(&v));
		}

		const_iterator iterator_to(const_reference v) const noexcept
		{
			assert(is_linked(v) && "intrusive_list::iterator_to: 노드가 어떤 리스트에도 속해있지 않음");
			return const_iterator(const_cast<intrusive_list_node*>(
				static_cast<const intrusive_list_node*>(&v)));
		}

		// 노드가 어떤 리스트에 등록되어 있는지 — 정확히 어느 리스트인지는 알
		// 수 없고, 등록 여부만 알 수 있다 (O(1)). 컨테이너 인스턴스 없이도
		// 호출 가능한 static 헬퍼.
		static bool is_linked(const_reference v) noexcept
		{
			return static_cast<const intrusive_list_node*>(&v)->m_next != nullptr;
		}

		// 노드가 *this*의 멤버인지 선형 탐색. 디버그/검증용. O(n).
		bool contains(const_reference v) const noexcept
		{
			const intrusive_list_node* needle = &v;
			for (const intrusive_list_node* p = m_sentinel.m_next; p != &m_sentinel; p = p->m_next)
				if (p == needle) return true;
			return false;
		}

		// --------------------------------------------------------------------
		// modifiers (값을 참조로 받고, 메모리 할당 없음)
		// --------------------------------------------------------------------
		void clear() noexcept
		{
			intrusive_list_node* cur = m_sentinel.m_next;
			while (cur != &m_sentinel)
			{
				intrusive_list_node* next = cur->m_next;
				cur->m_next = nullptr;
				cur->m_prev = nullptr;
				cur = next;
			}
			init_sentinel();
			m_size = 0;
		}

		void push_back(reference v) noexcept
		{
			assert(!is_linked(v) && "intrusive_list::push_back: 노드가 이미 다른 리스트의 멤버");
			link_before(&m_sentinel, &v);
			++m_size;
		}

		void push_front(reference v) noexcept
		{
			assert(!is_linked(v) && "intrusive_list::push_front: 노드가 이미 다른 리스트의 멤버");
			link_before(m_sentinel.m_next, &v);
			++m_size;
		}

		void pop_back()  noexcept { assert(!empty()); erase(iterator(m_sentinel.m_prev)); }
		void pop_front() noexcept { assert(!empty()); erase(iterator(m_sentinel.m_next)); }

		iterator insert(const_iterator pos, reference v) noexcept
		{
			assert(!is_linked(v) && "intrusive_list::insert: 노드가 이미 다른 리스트의 멤버");
			intrusive_list_node* n = &v;
			link_before(pos.m_node, n);
			++m_size;
			return iterator(n);
		}

		iterator erase(const_iterator pos) noexcept
		{
			assert(pos.m_node != &m_sentinel && "intrusive_list::erase: end() 삭제 불가");
			intrusive_list_node* target = pos.m_node;
			intrusive_list_node* next   = target->m_next;
			unlink_node(target);
			--m_size;
			return iterator(next);
		}

		iterator erase(const_iterator first, const_iterator last) noexcept
		{
			while (first != last) first = erase(first);
			return iterator(last.m_node);
		}

		// 노드를 직접 분리한다. *this*의 멤버여야 하며, 그래야 size 카운터가
		// 정확히 갱신된다 (다른 리스트의 노드를 unlink하면 size 카운터가 어긋남).
		void unlink(reference v) noexcept
		{
			assert(is_linked(v) && "intrusive_list::unlink: 노드가 어떤 리스트에도 속해있지 않음");
			erase(iterator_to(v));
		}

		// sentinel이 객체에 내장되어 있어 단순 byte-swap 불가. 노드들이 가리키는
		// sentinel 주소를 새 위치로 정정해야 한다.
		void swap(intrusive_list& other) noexcept
		{
			if (this == &other) return;

			const bool this_empty  = empty();
			const bool other_empty = other.empty();

			std::swap(m_sentinel.m_next, other.m_sentinel.m_next);
			std::swap(m_sentinel.m_prev, other.m_sentinel.m_prev);

			if (!other_empty) {
				m_sentinel.m_next->m_prev = &m_sentinel;
				m_sentinel.m_prev->m_next = &m_sentinel;
			} else {
				m_sentinel.m_next = &m_sentinel;
				m_sentinel.m_prev = &m_sentinel;
			}

			if (!this_empty) {
				other.m_sentinel.m_next->m_prev = &other.m_sentinel;
				other.m_sentinel.m_prev->m_next = &other.m_sentinel;
			} else {
				other.m_sentinel.m_next = &other.m_sentinel;
				other.m_sentinel.m_prev = &other.m_sentinel;
			}

			std::swap(m_size, other.m_size);
		}

		// --------------------------------------------------------------------
		// splice — intrusive_list의 핵심 강점 (포인터만 재배선)
		// --------------------------------------------------------------------

		// other의 모든 노드를 pos 앞으로 옮긴다. other는 비게 된다. O(1).
		void splice(const_iterator pos, intrusive_list& other) noexcept
		{
			if (this == &other || other.empty()) return;
			transfer_range(pos.m_node, other.m_sentinel.m_next, &other.m_sentinel);
			m_size       += other.m_size;
			other.m_size  = 0;
		}

		// other의 it 위치 노드 하나를 pos 앞으로 옮긴다. O(1).
		void splice(const_iterator pos, intrusive_list& other, const_iterator it) noexcept
		{
			assert(it.m_node != &other.m_sentinel && "intrusive_list::splice: end() 이동 불가");
			intrusive_list_node* node = it.m_node;
			intrusive_list_node* next = node->m_next;
			if (pos.m_node == node || pos.m_node == next) return;  // 같은 자리 이동 = no-op
			transfer_range(pos.m_node, node, next);
			if (this != &other) {
				++m_size;
				--other.m_size;
			}
		}

		// other의 [first, last) 구간을 pos 앞으로 옮긴다. O(거리).
		// pos가 [first, last) 사이에 있어선 안 된다(UB).
		void splice(const_iterator pos, intrusive_list& other,
		            const_iterator first, const_iterator last) noexcept
		{
			if (first == last) return;
			if (this != &other) {
				const size_type n = static_cast<size_type>(std::distance(first, last));
				transfer_range(pos.m_node, first.m_node, last.m_node);
				m_size       += n;
				other.m_size -= n;
			} else {
				transfer_range(pos.m_node, first.m_node, last.m_node);
			}
		}

		// --------------------------------------------------------------------
		// list 유틸리티
		// --------------------------------------------------------------------
		template <typename Pred>
		size_type remove_if(Pred pred)
		{
			size_type removed = 0;
			for (auto it = begin(); it != end(); )
			{
				if (pred(*it)) { it = erase(it); ++removed; }
				else           { ++it; }
			}
			return removed;
		}

		// 값 비교가 가능한 T에 한해 동작 (노드 정체성이 아니라 *값* 비교).
		template <typename U = T>
		auto remove(const U& v)
			-> decltype(std::declval<const U&>() == std::declval<const U&>(), size_type{})
		{
			return remove_if([&v](const T& x) { return x == v; });
		}

		void reverse() noexcept
		{
			intrusive_list_node* cur = &m_sentinel;
			do
			{
				std::swap(cur->m_next, cur->m_prev);
				cur = cur->m_prev;  // 방금 스왑했으므로 m_prev에 원래의 m_next가 들어 있다
			} while (cur != &m_sentinel);
		}

		// --------------------------------------------------------------------
		// sort / merge — std::list 패턴 (link 재배선만, 노드 주소 불변)
		// --------------------------------------------------------------------

		// 정렬된 두 리스트를 병합한다. 안정적: 동일 키일 때 *this*의 원소가
		// other의 원소보다 앞에 유지. O(n + m). other는 비게 된다.
		// 사전조건: 두 리스트 모두 comp로 비순감소(non-decreasing) 정렬 상태.
		void merge(intrusive_list& other)
		{
			merge(other, std::less<T>{});
		}

		template <typename Compare>
		void merge(intrusive_list& other, Compare comp)
		{
			if (this == &other || other.empty()) return;

			iterator it1 = begin();
			iterator it2 = other.begin();
			while (it1 != end() && it2 != other.end())
			{
				if (comp(*it2, *it1)) {
					iterator next2 = std::next(it2);
					splice(it1, other, it2);  // O(1) — 노드 1개를 it1 앞으로
					it2 = next2;
				} else {
					++it1;
				}
			}
			if (it2 != other.end()) {
				// other에 남은 노드들(모두 *this의 마지막 원소 이상)을 통째로 끝에
				splice(end(), other);
			}
		}

		// 안정 merge sort. O(n log n). 추가 메모리는 sentinel 65개분(스택)뿐 —
		// 노드는 옮기지 않고 포인터만 재배선한다.
		void sort()
		{
			sort(std::less<T>{});
		}

		template <typename Compare>
		void sort(Compare comp)
		{
			if (m_size < 2) return;

			// carry: 매 라운드마다 *this에서 한 노드씩 꺼내는 임시 버퍼.
			// tmp[i]: 이미 정렬된 2^(i+1)개 원소를 누적 보관하는 슬롯.
			// 2^64개를 넘는 리스트는 가정하지 않으므로 64면 충분.
			intrusive_list carry;
			intrusive_list tmp[64];
			intrusive_list* fill = &tmp[0];

			do
			{
				carry.splice(carry.begin(), *this, begin());

				intrusive_list* counter = &tmp[0];
				for (; counter != fill && !counter->empty(); ++counter)
				{
					counter->merge(carry, comp);
					carry.swap(*counter);
				}
				carry.swap(*counter);
				if (counter == fill) ++fill;
			} while (!empty());

			for (intrusive_list* counter = &tmp[1]; counter != fill; ++counter)
				counter->merge(*(counter - 1), comp);

			swap(*(fill - 1));
		}

	private:
		// --------------------------------------------------------------------
		// internal helpers
		// --------------------------------------------------------------------
		intrusive_list_node* sentinel_ptr() const noexcept
		{
			return const_cast<intrusive_list_node*>(&m_sentinel);
		}

		void init_sentinel() noexcept
		{
			m_sentinel.m_prev = &m_sentinel;
			m_sentinel.m_next = &m_sentinel;
		}

		// pos 앞에 n을 끼워넣는다.
		static void link_before(intrusive_list_node* pos, intrusive_list_node* n) noexcept
		{
			n->m_prev = pos->m_prev;
			n->m_next = pos;
			pos->m_prev->m_next = n;
			pos->m_prev = n;
		}

		// 노드를 리스트에서 떼어내고 prev/next를 nullptr로 (is_linked invariant).
		static void unlink_node(intrusive_list_node* n) noexcept
		{
			n->m_prev->m_next = n->m_next;
			n->m_next->m_prev = n->m_prev;
			n->m_next = nullptr;
			n->m_prev = nullptr;
		}

		// [first, last) 구간을 pos 앞으로 옮긴다. 포인터 6개 갱신으로 끝.
		static void transfer_range(intrusive_list_node* pos,
		                           intrusive_list_node* first,
		                           intrusive_list_node* last) noexcept
		{
			if (pos == first || first == last) return;
			intrusive_list_node* last_in = last->m_prev;

			// 원래 위치에서 구간 분리
			first->m_prev->m_next = last;
			last->m_prev          = first->m_prev;

			// pos 앞으로 재삽입
			first->m_prev         = pos->m_prev;
			pos->m_prev->m_next   = first;
			last_in->m_next       = pos;
			pos->m_prev           = last_in;
		}

		// --------------------------------------------------------------------
		// data
		// --------------------------------------------------------------------
		intrusive_list_node m_sentinel{};
		size_type           m_size = 0;
	};

	template <typename T>
	void swap(intrusive_list<T>& a, intrusive_list<T>& b) noexcept
	{
		a.swap(b);
	}

} // namespace mtl
