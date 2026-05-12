#pragma once
#include <cstddef>           // size_t, ptrdiff_t
#include <new>               // placement new
#include <utility>           // std::move, std::forward, std::swap
#include <iterator>          // std::reverse_iterator
#include <initializer_list>  // std::initializer_list
#include <type_traits>       // std::is_trivially_destructible_v 등
#include <memory>            // std::uninitialized_move, uninitialized_copy 등
#include <algorithm>         // std::move, std::move_backward, std::equal 등
#include <cassert>           // assert


#ifndef GTL_FIXED_MAX_OBJECT_BYTES
#define GTL_FIXED_MAX_OBJECT_BYTES (64u * 1024u)  // 64 KB
#endif

namespace mark
{
	/**
	* @brief 고정 크기 벡터 컨테이너. 런타임에 크기가 변경되지 않는 배열과 유사하지만, STL 스타일의 인터페이스를 제공합니다.
	* @tparam T 요소 타입
	* @tparam N 최대 요소 수 (용량)
	* @tparam Alignment 요소의 정렬 요구사항 (기본값은 alignof(T))
	*/
	template <typename T, size_t N, size_t Alignment = alignof(T)>
	class fixed_vector
	{
	public:
		static_assert(N > 0, "fixed_vector is a fixed-size container, N must be greater than 0.");
		static_assert(Alignment >= alignof(T), "fixed_vector: Alignment must be at least alignof(T).");
		static_assert((Alignment& (Alignment - 1)) == 0,
			"fixed_vector: Alignment must be a power of 2.");

		static_assert(sizeof(T)* N <= GTL_FIXED_MAX_OBJECT_BYTES,
			"fixed_vector: The size of the object exceeds GTL_FIXED_MAX_OBJECT_BYTES.");

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
		alignas(Alignment) unsigned char _buffer[sizeof(T) * N];

		pointer _end;

		inline pointer buffer_ptr() noexcept { return reinterpret_cast<pointer>(_buffer); }
		inline const_pointer buffer_ptr() const noexcept { return reinterpret_cast<const_pointer>(_buffer); }

		inline static void destroy_range(pointer first, pointer last) noexcept
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				for (; first != last; ++first) 
					first->~T();
			}
		}

	public:
		fixed_vector() noexcept
			: _end(reinterpret_cast<pointer>(_buffer))
		{
		}

		explicit fixed_vector(size_type n)
			: _end(reinterpret_cast<pointer>(_buffer))
		{
			resize(n);
		}

		fixed_vector(size_type n, const T& value)
			: _end(reinterpret_cast<pointer>(_buffer))
		{
			assign(n, value);
		}

		template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
		fixed_vector(InputIt first, InputIt last)
			: _end(reinterpret_cast<pointer>(_buffer))
		{
			assign(first, last);
		}

		fixed_vector(std::initializer_list<T> il)
			: _end(reinterpret_cast<pointer>(_buffer))
		{
			assign(il.begin(), il.end());
		}

		fixed_vector(const fixed_vector& other)
			: _end(reinterpret_cast<pointer>(_buffer))
		{
			assign(other.begin(), other.end());
		}

		fixed_vector(fixed_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
			: _end(reinterpret_cast<pointer>(_buffer))
		{
			_end = std::uninitialized_move(other.begin(), other.end(), buffer_ptr());
			other.clear();
		}

		~fixed_vector()
		{
			destroy_range(buffer_ptr(), _end);
		}

		inline fixed_vector& operator=(const fixed_vector& other)
		{
			if (this != &other) {
				assign(other.begin(), other.end());
			}
			return *this;
		}

		inline fixed_vector& operator=(fixed_vector&& other) 
			noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>)
		{
			if (this == &other) return *this;
			clear();
			_end = std::uninitialized_move(other.begin(), other.end(), buffer_ptr());
			other.clear();
			return *this;
		}

		inline fixed_vector& operator=(std::initializer_list<T> il)
		{
			assign(il.begin(), il.end());
			return *this;
		}

		void assign(size_type n, const T& value)
		{
			assert(n <= N && "fixed_vector::assign: n이 용량 N을 초과");
			clear();
			std::uninitialized_fill_n(buffer_ptr(), n, value);
			_end = buffer_ptr() + n;
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
				for (; first != last; ++first)
				{
					::new (static_cast<void*>(_end)) T(*first);
					++_end;
				}
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

		iterator        end()          noexcept { return _end; }
		const_iterator  end()    const noexcept { return _end; }
		const_iterator  cend()   const noexcept { return _end; }

		reverse_iterator        rbegin()        noexcept { return reverse_iterator(_end); }
		const_reverse_iterator  rbegin()  const noexcept { return const_reverse_iterator(_end); }
		const_reverse_iterator  crbegin() const noexcept { return const_reverse_iterator(_end); }

		reverse_iterator        rend()          noexcept { return reverse_iterator(buffer_ptr()); }
		const_reverse_iterator  rend()    const noexcept { return const_reverse_iterator(buffer_ptr()); }
		const_reverse_iterator  crend()   const noexcept { return const_reverse_iterator(buffer_ptr()); }

		inline size_type size() const noexcept
		{
			return static_cast<size_type>(_end - buffer_ptr());
		}

		static constexpr size_type capacity() noexcept { return N; }
		static constexpr size_type max_size() noexcept { return N; }

		bool empty() const noexcept { return _end == buffer_ptr(); }
		bool full()  const noexcept { return size() == N; }  // 고정 컨테이너에 유용

		inline reference       operator[](size_type i)       noexcept { assert(i < size()); return buffer_ptr()[i]; }
		inline const_reference operator[](size_type i) const noexcept { assert(i < size()); return buffer_ptr()[i]; }

		inline reference at(size_type i)
		{
			assert(i < size() && "fixed_vector::at: index out of range");
			return buffer_ptr()[i];
		}

		inline const_reference at(size_type i) const
		{
			assert(i < size() && "fixed_vector::at: index out of range");
			return buffer_ptr()[i];
		}

		reference       front()       noexcept { assert(!empty()); return buffer_ptr()[0]; }
		const_reference front() const noexcept { assert(!empty()); return buffer_ptr()[0]; }
		reference       back()        noexcept { assert(!empty()); return *(_end - 1); }
		const_reference back()  const noexcept { assert(!empty()); return *(_end - 1); }

		pointer         data()        noexcept { return buffer_ptr(); }
		const_pointer   data()  const noexcept { return buffer_ptr(); }

		void clear() noexcept
		{
			destroy_range(buffer_ptr(), _end);
			_end = buffer_ptr();
		}

		void push_back(const T& value)
		{
			assert(size() < N && "fixed_vector::push_back: 용량 초과");
			::new (static_cast<void*>(_end)) T(value);
			++_end;
		}

		void push_back(T&& value)
		{
			assert(size() < N && "fixed_vector::push_back: 용량 초과");
			::new (static_cast<void*>(_end)) T(std::move(value));
			++_end;
		}

		// 임시 객체 생성을 피하는 in-place 구성 (게임 코드에서 핵심)
		template <typename... Args>
		reference emplace_back(Args&&... args)
		{
			assert(size() < N && "fixed_vector::emplace_back: 용량 초과");
			pointer p = _end;
			::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			++_end;
			return *p;
		}

		void pop_back() noexcept
		{
			assert(!empty());
			--_end;
			if constexpr (!std::is_trivially_destructible_v<T>) {
				_end->~T();
			}
		}

		iterator insert(const_iterator pos, const T& value) { return emplace(pos, value); }
		iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }

		template <typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			assert(pos >= buffer_ptr() && pos <= _end);
			assert(size() < N && "fixed_vector::emplace: 용량 초과");

			pointer p = const_cast<pointer>(pos);

			if (p == _end)
			{
				::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			}
			else
			{
				::new (static_cast<void*>(_end)) T(std::move(*(_end - 1)));
				std::move_backward(p, _end - 1, _end);
				p->~T();
				::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			}
			++_end;
			return p;
		}

		iterator erase(const_iterator pos)
		{
			assert(pos >= buffer_ptr() && pos < _end);
			pointer p = const_cast<pointer>(pos);
			std::move(p + 1, _end, p);
			--_end;
			if constexpr (!std::is_trivially_destructible_v<T>) {
				_end->~T();
			}
			return p;
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first >= buffer_ptr() && last <= _end && first <= last);
			pointer pf = const_cast<pointer>(first);
			pointer pl = const_cast<pointer>(last);
			if (pf == pl) return pf;

			pointer new_end = std::move(pl, _end, pf);
			destroy_range(new_end, _end);
			_end = new_end;
			return pf;
		}

		iterator erase_unsorted(const_iterator pos)
		{
			assert(pos >= buffer_ptr() && pos < _end);
			pointer p = const_cast<pointer>(pos);
			if (p != _end - 1) {
				*p = std::move(*(_end - 1));
			}
			--_end;
			if constexpr (!std::is_trivially_destructible_v<T>) {
				_end->~T();
			}
			return p;
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
					other._end,
					_end
				);

				destroy_range(other.buffer_ptr() + common, other._end);
			}
			else if (sb < sa)
			{
				std::uninitialized_move(
					buffer_ptr() + common,
					_end,
					other._end
				);
				destroy_range(buffer_ptr() + common, _end);
			}

			_end = buffer_ptr() + sb;
			other._end = other.buffer_ptr() + sa;
		}
	};

	// 비교 연산자: N, Alignment가 달라도 원소 타입만 같으면 비교 가능
	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator==(const fixed_vector<T, N1, A1>& a,
		const fixed_vector<T, N2, A2>& b)
	{
		return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator!=(const fixed_vector<T, N1, A1>& a,
		const fixed_vector<T, N2, A2>& b) {
		return !(a == b);
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator<(const fixed_vector<T, N1, A1>& a,
		const fixed_vector<T, N2, A2>& b)
	{
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator>(const fixed_vector<T, N1, A1>& a,
		const fixed_vector<T, N2, A2>& b) {
		return b < a;
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator<=(const fixed_vector<T, N1, A1>& a,
		const fixed_vector<T, N2, A2>& b) {
		return !(b < a);
	}

	template <typename T, size_t N1, size_t N2, size_t A1, size_t A2>
	bool operator>=(const fixed_vector<T, N1, A1>& a,
		const fixed_vector<T, N2, A2>& b) {
		return !(a < b);
	}

	template <typename T, size_t N, size_t A>
	void swap(fixed_vector<T, N, A>& a, fixed_vector<T, N, A>& b)
		noexcept(noexcept(a.swap(b)))
	{
		a.swap(b);
	}

} // namespace mark
