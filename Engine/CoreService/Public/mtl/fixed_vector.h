#pragma once
#include <cstddef>
#include <new>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <cassert>


#ifndef MTL_FIXED_MAX_OBJECT_BYTES
#define MTL_FIXED_MAX_OBJECT_BYTES (64u * 1024u)  // 64 KB
#endif


// ----------------------------------------------------------------------------
// mtl::fixed_vector<T, N, Alignment> — 인플레이스 버퍼를 가진 고정 용량 벡터.
//
// 사용 용도:
//   - 함수 지역/멤버로 두는 짧은 수명의 컬렉션 (수십~수백 개).
//   - 매 프레임 잠깐 모았다 비우는 임시 컨테이너
//     (수집한 충돌체, 가시 객체, 컬링 결과 등).
//   - 동적 할당이 부담스러운 핫 패스에서 std::vector 대체.
//   - 컴포넌트 작은 리스트 (Player의 buff 리스트, Mesh의 sub-mesh 인덱스 등).
//   - 임의 접근이 필요하면서 N이 예측 가능한 경우.
//
// 안 쓰는 게 좋은 경우:
//   - 크기 상한을 예측하기 어려운 경우 → std::vector.
//   - N * sizeof(T)가 64KB 이상으로 큰 경우 (스택 오버플로 위험.
//     MTL_FIXED_MAX_OBJECT_BYTES static_assert가 막아줌).
//   - 빈번한 중간 삽입/삭제가 필요 → fixed_list 고려.
//
// 특성:
//   - 연속 메모리, 캐시 친화.
//   - push_back / emplace_back / pop_back : O(1).
//   - 중간 insert / erase                  : O(N).
//   - erase_unsorted                       : O(1) (순서 보존 안 함, 게임코드 핵심 패턴).
//   - 동적 할당 없음.
//
// 사용 예:
//   mtl::fixed_vector<int, 16> v;
//   v.push_back(1);
//   v.emplace_back(2);
//   for (int n : v) { ... }
// ----------------------------------------------------------------------------

namespace mtl
{
	template <typename T, size_t N, size_t Alignment = alignof(T)>
	class fixed_vector
	{
	public:
		static_assert(N > 0, "fixed_vector is a fixed-size container, N must be greater than 0.");
		static_assert(Alignment >= alignof(T), "fixed_vector: Alignment must be at least alignof(T).");
		static_assert((Alignment& (Alignment - 1)) == 0,
			"fixed_vector: Alignment must be a power of 2.");

		static_assert(sizeof(T)* N <= MTL_FIXED_MAX_OBJECT_BYTES,
			"fixed_vector: The size of the object exceeds MTL_FIXED_MAX_OBJECT_BYTES.");

		using value_type = T;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const T*;

		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		static constexpr size_type kCapacity = N;
		static constexpr size_type kAlignment = Alignment;

	private:
		alignas(Alignment) std::byte m_buffer[sizeof(T) * N];

		pointer m_end;

		pointer buffer_ptr() noexcept { return reinterpret_cast<pointer>(m_buffer); }
		const_pointer buffer_ptr() const noexcept { return reinterpret_cast<const_pointer>(m_buffer); }

		static void destroy_range(pointer first, pointer last) noexcept
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				for (; first != last; ++first)
					first->~T();
			}
		}

	public:
		fixed_vector() noexcept
			: m_end(reinterpret_cast<pointer>(m_buffer))
		{
		}

		fixed_vector(size_type n, const T& value)
			: m_end(reinterpret_cast<pointer>(m_buffer))
		{
			assign(n, value);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_vector(InputIt first, InputIt last)
			: m_end(reinterpret_cast<pointer>(m_buffer))
		{
			assign(first, last);
		}

		fixed_vector(std::initializer_list<T> il)
			: m_end(reinterpret_cast<pointer>(m_buffer))
		{
			assign(il.begin(), il.end());
		}

		fixed_vector(const fixed_vector& other)
			: m_end(reinterpret_cast<pointer>(m_buffer))
		{
			assign(other.begin(), other.end());
		}

		fixed_vector(fixed_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
			: m_end(reinterpret_cast<pointer>(m_buffer))
		{
			m_end = std::uninitialized_move(other.begin(), other.end(), buffer_ptr());
			other.clear();
		}

		~fixed_vector()
		{
			destroy_range(buffer_ptr(), m_end);
		}

		fixed_vector& operator=(const fixed_vector& other)
		{
			if (this != &other) {
				assign(other.begin(), other.end());
			}
			return *this;
		}

		fixed_vector& operator=(fixed_vector&& other)
			noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>)
		{
			if (this == &other) return *this;
			clear();
			m_end = std::uninitialized_move(other.begin(), other.end(), buffer_ptr());
			other.clear();
			return *this;
		}

		fixed_vector& operator=(std::initializer_list<T> il)
		{
			assign(il.begin(), il.end());
			return *this;
		}

		void assign(size_type n, const T& value)
		{
			assert(n <= N && "fixed_vector::assign: n이 용량 N을 초과");
			clear();
			std::uninitialized_fill_n(buffer_ptr(), n, value);
			m_end = buffer_ptr() + n;
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		void assign(InputIt first, InputIt last)
		{
			clear();

			if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
				typename std::iterator_traits<InputIt>::iterator_category>)
			{
				const size_type n = static_cast<size_type>(std::distance(first, last));
				assert(n <= N && "fixed_vector::assign: overflow");
				m_end = std::uninitialized_copy(first, last, buffer_ptr());
			}
			else
			{
				for (; first != last; ++first)
					push_back(*first);
			}
		}

		void assign(std::initializer_list<T> il)
		{
			assign(il.begin(), il.end());
		}

		iterator        begin()        noexcept { return buffer_ptr(); }
		const_iterator  begin()  const noexcept { return buffer_ptr(); }
		const_iterator  cbegin() const noexcept { return buffer_ptr(); }

		iterator        end()          noexcept { return m_end; }
		const_iterator  end()    const noexcept { return m_end; }
		const_iterator  cend()   const noexcept { return m_end; }

		reverse_iterator        rbegin()        noexcept { return reverse_iterator(m_end); }
		const_reverse_iterator  rbegin()  const noexcept { return const_reverse_iterator(m_end); }
		const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator(m_end); }

		reverse_iterator        rend()          noexcept { return reverse_iterator(buffer_ptr()); }
		const_reverse_iterator  rend()    const noexcept { return const_reverse_iterator(buffer_ptr()); }
		const_reverse_iterator  crend()   const noexcept { return const_reverse_iterator(buffer_ptr()); }

		size_type size() const noexcept
		{
			return static_cast<size_type>(m_end - buffer_ptr());
		}

		static constexpr size_type capacity() noexcept { return N; }
		static constexpr size_type max_size() noexcept { return N; }

		bool empty() const noexcept { return m_end == buffer_ptr(); }
		bool full()  const noexcept { return size() == N; }  // 고정 컨테이너에 유용

		reference       operator[](size_type i)       noexcept { assert(i < size()); return buffer_ptr()[i]; }
		const_reference operator[](size_type i) const noexcept { assert(i < size()); return buffer_ptr()[i]; }

		reference at(size_type i)
		{
			assert(i < size() && "fixed_vector::at: index out of range");
			return buffer_ptr()[i];
		}

		const_reference at(size_type i) const
		{
			assert(i < size() && "fixed_vector::at: index out of range");
			return buffer_ptr()[i];
		}

		reference       front()       noexcept { assert(!empty()); return buffer_ptr()[0]; }
		const_reference front() const noexcept { assert(!empty()); return buffer_ptr()[0]; }
		reference       back()        noexcept { assert(!empty()); return *(m_end - 1); }
		const_reference back()  const noexcept { assert(!empty()); return *(m_end - 1); }

		pointer         data()        noexcept { return buffer_ptr(); }
		const_pointer   data()  const noexcept { return buffer_ptr(); }

		void clear() noexcept
		{
			destroy_range(buffer_ptr(), m_end);
			m_end = buffer_ptr();
		}

		int32_t find_index(const T& value) const noexcept
		{
			for (size_type i = 0; i < size(); ++i)
			{
				if (buffer_ptr()[i] == value)
					return static_cast<int32_t>(i);
			}

			return -1; // not found
		}

		void push_back(const T& value)
		{
			assert(size() < N && "fixed_vector::push_back: 용량 초과");
			::new (static_cast<void*>(m_end)) T(value);
			++m_end;
		}

		bool try_push_back(const T& value)
		{
			if (size() >= N) return false;
			::new (static_cast<void*>(m_end)) T(value);
			++m_end;
			return true;
		}

		void push_back(T&& value)
		{
			assert(size() < N && "fixed_vector::push_back: 용량 초과");
			::new (static_cast<void*>(m_end)) T(std::move(value));
			++m_end;
		}

		bool try_push_back(T&& value)
		{
			if (size() >= N) return false;
			::new (static_cast<void*>(m_end)) T(std::move(value));
			++m_end;
			return true;
		}

		// 임시 객체 생성을 피하는 in-place 구성 (게임 코드에서 핵심)
		template <typename... Args>
		reference emplace_back(Args&&... args)
		{
			assert(size() < N && "fixed_vector::emplace_back: 용량 초과");
			pointer p = m_end;
			::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			++m_end;
			return *p;
		}

		template <typename... Args>
		bool try_emplace_back(Args&&... args)
		{
			if (size() >= N) return false;
			::new (static_cast<void*>(m_end)) T(std::forward<Args>(args)...);
			++m_end;
			return true;
		}

		void pop_back() noexcept
		{
			assert(!empty());
			--m_end;
			if constexpr (!std::is_trivially_destructible_v<T>) {
				m_end->~T();
			}
		}

		iterator insert(const_iterator pos, const T& value) { return emplace(pos, value); }
		iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }

		iterator insert(const_iterator pos, size_type count, const T& value)
		{
			assert(pos >= buffer_ptr() && pos <= m_end);
			assert(size() + count <= N && "fixed_vector::insert: 용량 초과");

			pointer p = const_cast<pointer>(pos);
			if (count == 0) return p;

			const size_type tail = static_cast<size_type>(m_end - p);

			if (count <= tail)
			{
				std::uninitialized_move(m_end - count, m_end, m_end);
				std::move_backward(p, m_end - count, m_end);
				std::fill_n(p, count, value);
			}
			else
			{
				std::uninitialized_move(p, m_end, p + count);
				std::fill_n(p, tail, value);
				std::uninitialized_fill_n(p + tail, count - tail, value);
			}
			m_end += count;
			return p;
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		iterator insert(const_iterator pos, InputIt first, InputIt last)
		{
			assert(pos >= buffer_ptr() && pos <= m_end);
			pointer p = const_cast<pointer>(pos);

			if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
				typename std::iterator_traits<InputIt>::iterator_category>)
			{
				const size_type count = static_cast<size_type>(std::distance(first, last));
				assert(size() + count <= N && "fixed_vector::insert: 용량 초과");
				if (count == 0) return p;

				const size_type tail = static_cast<size_type>(m_end - p);

				if (count <= tail)
				{
					std::uninitialized_move(m_end - count, m_end, m_end);
					std::move_backward(p, m_end - count, m_end);
					std::copy(first, last, p);
				}
				else
				{
					std::uninitialized_move(p, m_end, p + count);
					InputIt mid = first;
					std::advance(mid, tail);
					std::copy(first, mid, p);
					std::uninitialized_copy(mid, last, p + tail);
				}
				m_end += count;
				return p;
			}
			else
			{
				const size_type offset = static_cast<size_type>(p - buffer_ptr());
				size_type inserted = 0;
				for (; first != last; ++first, ++inserted)
				{
					assert(size() < N && "fixed_vector::insert: 용량 초과");
					emplace(buffer_ptr() + offset + inserted, *first);
				}
				return buffer_ptr() + offset;
			}
		}

		iterator insert(const_iterator pos, std::initializer_list<T> il)
		{
			return insert(pos, il.begin(), il.end());
		}

		template <typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			assert(pos >= buffer_ptr() && pos <= m_end);
			assert(size() < N && "fixed_vector::emplace: 용량 초과");

			pointer p = const_cast<pointer>(pos);

			if (p == m_end)
			{
				::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			}
			else
			{
				::new (static_cast<void*>(m_end)) T(std::move(*(m_end - 1)));
				std::move_backward(p, m_end - 1, m_end);
				p->~T();
				::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			}
			++m_end;
			return p;
		}

		iterator erase(const_iterator pos)
		{
			assert(pos >= buffer_ptr() && pos < m_end);
			pointer p = const_cast<pointer>(pos);
			std::move(p + 1, m_end, p);
			--m_end;
			if constexpr (!std::is_trivially_destructible_v<T>) {
				m_end->~T();
			}
			return p;
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first >= buffer_ptr() && last <= m_end && first <= last);
			pointer pf = const_cast<pointer>(first);
			pointer pl = const_cast<pointer>(last);
			if (pf == pl) return pf;

			pointer new_end = std::move(pl, m_end, pf);
			destroy_range(new_end, m_end);
			m_end = new_end;
			return pf;
		}

		iterator erase_unsorted(const_iterator pos)
		{
			assert(pos >= buffer_ptr() && pos < m_end);
			pointer p = const_cast<pointer>(pos);
			if (p != m_end - 1) {
				*p = std::move(*(m_end - 1));
			}
			--m_end;
			if constexpr (!std::is_trivially_destructible_v<T>) {
				m_end->~T();
			}
			return p;
		}

		void resize(size_type new_size)
		{
			assert(new_size <= N && "fixed_vector::resize: 용량 초과");
			pointer p = buffer_ptr() + new_size;
			if (new_size > size())
				std::uninitialized_value_construct(m_end, p);
			else
				destroy_range(p, m_end);
			m_end = p;
		}

		void resize(size_type new_size, const T& value)
		{
			assert(new_size <= N && "fixed_vector::resize: 용량 초과");
			pointer p = buffer_ptr() + new_size;
			if (new_size > size())
				std::uninitialized_fill(m_end, p, value);
			else
				destroy_range(p, m_end);
			m_end = p;
		}

		void swap(fixed_vector& other)
			noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_swappable_v<T>)
		{
			if (this == &other) return;

			const size_type sa = size();
			const size_type sb = other.size();
			const size_type common = (sa < sb) ? sa : sb;

			std::swap_ranges(buffer_ptr(), buffer_ptr() + common, other.buffer_ptr());

			if (sa < sb)
			{
				std::uninitialized_move(
					other.buffer_ptr() + common,
					other.m_end,
					m_end
				);

				destroy_range(other.buffer_ptr() + common, other.m_end);
			}
			else if (sb < sa)
			{
				std::uninitialized_move(
					buffer_ptr() + common,
					m_end,
					other.m_end
				);
				destroy_range(buffer_ptr() + common, m_end);
			}

			m_end = buffer_ptr() + sb;
			other.m_end = other.buffer_ptr() + sa;
		}
	};

	// 비교 연산자: N, Alignment가 달라도 원소 타입만 같으면 비교 가능
	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator==(const fixed_vector<T, N1, A1>& a, const fixed_vector<T, N2, A2>& b)
	{
		return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator!=(const fixed_vector<T, N1, A1>& a, const fixed_vector<T, N2, A2>& b)
	{
		return !(a == b);
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator<(const fixed_vector<T, N1, A1>& a, const fixed_vector<T, N2, A2>& b)
	{
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator>(const fixed_vector<T, N1, A1>& a, const fixed_vector<T, N2, A2>& b)
	{
		return b < a;
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator<=(const fixed_vector<T, N1, A1>& a, const fixed_vector<T, N2, A2>& b)
	{
		return !(b < a);
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator>=(const fixed_vector<T, N1, A1>& a, const fixed_vector<T, N2, A2>& b)
	{
		return !(a < b);
	}

	template <typename T, size_t N, size_t A>
	void swap(fixed_vector<T, N, A>& a, fixed_vector<T, N, A>& b) noexcept(noexcept(a.swap(b)))
	{
		a.swap(b);
	}

} // namespace mtl
