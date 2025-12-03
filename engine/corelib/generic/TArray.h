#ifndef __TARRAY_H__
#define __TARRAY_H__

#include <initializer_list>
#include "TAllocator.h"
#include "TCommon.h"


namespace mark
{
	/**
	* TArray
	* @brief 템플릿 동적 배열 클래스.
	* 동적 배열은 크기가 변경될 수 있는 배열로, 필요에 따라 메모리를 할당 및 해제할 수 있습니다.
	* 이 구현은 메모리 할당 유형을 지정할 수 있는 템플릿 클래스로, 다양한 데이터 타입에 대해 사용할 수 있습니다.
	* @tparam _T 배열에 저장할 데이터 타입
	* @tparam _AllocType 메모리 할당 유형 (기본값: ALLOC_TYPE_SYSCALL)
	*/
	template<typename _T, ALLOC_TYPE _AllocType = ALLOC_TYPE::SYSCALL>
	struct TArray
	{
		constexpr static size_t DEFAULT_HEAPARRAY_CAPACITY = 32;

		template<typename _T>
		struct __Array_Iterator
		{
			_T* val;

			__Array_Iterator() : val(nullptr) {}
			__Array_Iterator(_T* _pVal) : val(_pVal) {}
			__Array_Iterator(const __Array_Iterator& other) : val(other.val) {}

			__Array_Iterator(__Array_Iterator&& other)
			{
				val = other.val;
				other.val = nullptr;
			}

			inline __Array_Iterator& operator=(const __Array_Iterator& other)
			{
				val = other.val;
				return *this;
			}

			inline __Array_Iterator& operator=(__Array_Iterator&& other)
			{
				val = other.val;
				Ohter.val = nullptr;
				return *this;
			}

			inline _T& operator*() { return *val; }
			inline const _T& operator*() const { return *val; }

			inline _T& operator->() { return *val; }
			inline const _T& operator->() const { return *val; }

			inline const bool operator==(const __Array_Iterator<_T>& rhs) const { return val == rhs.val; }
			inline const bool operator!=(const __Array_Iterator<_T>& rhs) const { return val != rhs.val; }

			inline const bool operator<(const __Array_Iterator<_T>& rhs) const { return val < rhs.val; }
			inline const bool operator>(const __Array_Iterator<_T>& rhs) const { return val > rhs.val; }

			inline const bool operator>=(const __Array_Iterator<_T>& rhs) const { return !(val < rhs.val); }
			inline const bool operator<=(const __Array_Iterator<_T>& rhs) const { return !(val > rhs.val); }

			inline __Array_Iterator& operator++() { val++; return *this; }
			inline __Array_Iterator& operator--() { val--; return *this; }
			inline __Array_Iterator& operator++(int) { val++; return *this; }
			inline __Array_Iterator& operator--(int) { val--; return *this; }
		};

		constexpr static size_t DEFAULT_ARRAY_CAPACITY = 32;

		using value_type = _T;
		typedef const __Array_Iterator<_T> const_iterator;
		typedef __Array_Iterator<_T> iterator;

		TArray() : _data(nullptr), _count(0), _capacity(0) {}
		TArray(const TArray<_T>& other)
		{
			destroy();

			_capacity = other._capacity;
			_count = other._count;

			if (_capacity)
			{
				_data = static_cast<_T*>(GENERIC_ALLOC(sizeof(_T) * _capacity, _AllocType));

				if (_count)
					memcpy(_data, other._data, sizeof(_T) * _count);
			}
		}

		TArray(TArray<_T>&& other)
		{
			destroy();

			_capacity = other._capacity;
			_count = other._count;
			_data = other._data;
			other._capacity = 0;
			other._count = 0;
			other._data = nullptr;
		}

		TArray(size_t capacity)
			: _capacity(!capacity ? DEFAULT_HEAPARRAY_CAPACITY : capacity)
		{
			_data = static_cast<_T*>(GENERIC_ALLOC(sizeof(_T) * _capacity, _AllocType));
		}

		TArray(std::initializer_list<value_type> init_list)
		{
			reserve(init_list.size() << 1);
			for (const auto& Value : init_list)
			{
				push_back(Value);
			}
		}

		virtual ~TArray()
		{
			destroy();
		}

		TArray<value_type>& operator=(std::initializer_list<value_type> init_list)
		{
			destroy();

			reserve(init_list.size());
			for (const auto& Value : init_list)
			{
				push_back(Value);
			}
			return *this;
		}

		inline TArray& operator=(const TArray& rhs)
		{
			destroy();

			_capacity = rhs._capacity;
			_count = rhs._count;

			if (_capacity)
			{
				_data = static_cast<_T*>(GENERIC_ALLOC(sizeof(_T) * rhs._capacity, _AllocType));

				if (_count)
					memcpy(_data, rhs._data, sizeof(_T) * rhs._count);
			}

			return *this;
		}

		inline TArray& operator=(TArray&& rhs)
		{
			destroy();
			_capacity = rhs._capacity;
			_count = rhs._count;
			_data = rhs._data;

			rhs._data = nullptr;
			rhs._capacity = 0;
			rhs._count = 0;

			return *this;
		}

		inline _T& operator[](size_t index)
		{
			__ASSERT(index < _count, "INVALID ARRAY INDEX");
			return _data[index];
		}

		inline _T& at(size_t index)
		{
			__ASSERT(index < _count, "INVALID ARRAY INDEX");
			return _data[index];
		}

		inline _T* data()
		{
			return _data;
		}

		inline void clear()
		{
			if (_data)
			{
				if constexpr (std::is_destructible<_T>::value && !std::is_pointer<_T>::value)
				{
					for (size_t i = 0; i < _count; ++i)
					{
						_data[i].~_T();
					}
				}
			}

			_count = 0;
		}

		// resize는 지원하지 않는다.. 
		inline void reserve(size_t reserve)
		{
			if (!reserve)
				reserve = DEFAULT_HEAPARRAY_CAPACITY;

			if (_capacity && _capacity >= reserve)
				return;

			_T* new_data = static_cast<_T*>(GENERIC_REALLOC(_data, sizeof(_T) * _capacity, sizeof(_T) * reserve, _AllocType));
			if (!new_data)
			{
				__ASSERT(0, "Memory allocation failed in TArray::reserve");
				return;
			}

			_data = new_data;
			_capacity = reserve;
		}

		inline void push_back(const _T& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);
			_data[_count++] = val;
		}

		inline void push_back(_T&& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);
			_data[_count++] = std::move(val);
		}

		inline void push_range(const _T* array, size_t count)
		{
			if (_capacity < count)
				reserve(count << 1);

			clear();

			memcpy(_data, array, sizeof(_T) * count);
			_count = count;
		}

		inline void push_front(const _T& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);

			memmove(_data + 1, _data, sizeof(_T) * _count);
			_data[0] = val;
			_count++;
		}

		inline void push_front(_T&& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);

			memmove(_data + 1, _data, sizeof(_T) * _count);
			_data[0] = std::move(val);
			_count++;
		}

		inline void pop_back()
		{
			if (!_count) return;

			if constexpr (std::is_destructible<_T>::value && !std::is_pointer<_T>::value)
			{
				_data[_count - 1].~_T();
			}

			_count--;
		}

		inline void pop_front()
		{
			if (!_count) return;

			if constexpr (std::is_destructible<_T>::value && !std::is_pointer<_T>::value)
			{
				_data[0].~_T();
			}

			_count--;

			if (_count)
				memmove(_data, _data + 1, sizeof(_T) * _count);
		}

		inline void insert_back(size_t pos, const _T& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);

			if (pos >= _count)
				pos = _count - 1;

			size_t diff = _count - pos - 1;

			if (diff)
			{
				memmove(_data + pos + 2, _data + pos + 1, sizeof(_T) * diff);
			}

			_data[pos + 1] = val;
			_count++;
		}

		inline void insert_back(size_t pos, _T&& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);

			if (pos >= _count)
				pos = _count - 1;

			size_t diff = _count - pos - 1;

			if (diff)
			{
				memmove(_data + pos + 2, _data + pos + 1, sizeof(_T) * diff);
			}

			_data[pos + 1] = std::move(val);
			_count++;
		}

		inline void insert_front(size_t pos, const _T& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);

			if (pos >= _count)
				pos = _count - 1;

			size_t diff = _count - pos;

			if (diff)
			{
				memmove(_data + pos + 1, _data + pos, sizeof(_T) * diff);
			}

			_data[pos] = val;
			_count++;
		}

		inline void insert_front(size_t pos, _T&& val)
		{
			if (_count >= _capacity)
				reserve(_capacity << 1);

			if (pos >= _count)
				pos = _count - 1;

			size_t diff = _count - pos;

			if (diff)
			{
				memmove(_data + pos + 1, _data + pos, sizeof(_T) * diff);
			}

			_data[pos] = std::move(val);
			_count++;
		}

		inline void erase(size_t pos)
		{
			if (!_count || _count <= pos) return;

			size_t diff = _count - pos - 1;

			if constexpr (std::is_destructible<_T>::value && !std::is_pointer<_T>::value)
			{
				_data[pos].~_T();
			}

			if (diff)
			{
				memmove(_data + pos, _data + pos + 1, sizeof(_T) * diff);
			}

			_count--;
		}

		inline void erase(iterator it)
		{
			size_t index = ((uintptr_t)_data - (uintptr_t)it->val) / sizeof(_T);
			erase(index);
		}

		template<typename Pred>
		inline void sort(Pred pred)
		{
			mark::sort(_data, _count, pred);
		}

		inline void sort()
		{
			sort([](const _T& a, const _T& b)
				{
					return a < b;
				});
		}

		inline int find_index(const _T& val)
		{
			for (size_t i = 0; i < _count; ++i)
			{
				if (_data[i] == val)
					return (int)i;
			}

			return -1;
		}

		template<class Pred, typename ...Args>
		inline int find_index(Pred pred, Args... args)
		{
			for (size_t i = 0; i < _count; ++i)
			{
				if (pred(_data[i], std::forward<Args>(args)...))
					return (int)i;
			}
			return -1;
		}

		template<class Pred, typename ...Args>
		inline const_iterator find(Pred pred, Args... args)
		{
			for (size_t i = 0; i < _count; ++i)
			{
				if (pred(_data[i], std::forward<Args>(args)...))
					return const_iterator(&_data[i]);
			}

			return end();
		}

		// 반드시 sort가 된 상태에서 사용해야 함..
		template<class Pred, typename ...Args>
		inline int binary_find_index(Pred pred, Args... args)
		{
			return generic::binary_search<_T, Pred, Args...>(
				_data,
				_count,
				pred,
				std::forward<Args>(args)...);
		}

		// 반드시 sort가 된 상태에서 사용해야 함..
		template<class Pred, typename ...Args>
		inline const_iterator binary_find(Pred pred, Args... args)
		{
			int idx = binary_search<_T, Pred, Args...>(
				_data,
				_count,
				pred,
				std::forward<Args>(args)...
			);

			if (-1 == idx)
				return end();

			return const_iterator(&_data[idx]);
		}

		inline size_t capacity() const noexcept
		{
			return _capacity;
		}

		inline size_t size() const noexcept
		{
			return _count;
		}

		inline size_t count() const noexcept
		{
			return _count;
		}

		inline bool empty() const noexcept
		{
			return _count == 0;
		}

		// Ranged-for
		inline const_iterator begin() const
		{
			return const_iterator(_data);
		}

		inline iterator begin()
		{
			return iterator(_data);
		}

		inline const_iterator end() const
		{
			return const_iterator(_data + _count);
		}

		inline iterator end()
		{
			return iterator(_data + _count);
		}


	private:
		inline void destroy()
		{
			if (_data)
			{
				if constexpr (std::is_destructible<_T>::value && !std::is_pointer<_T>::value)
				{
					for (size_t i = 0; i < _count; ++i)
					{
						_data[i].~_T();
					}
				}

				GENERIC_FREE((void*)_data, _AllocType);
				_data = nullptr;
			}

			_capacity = 0;
			_count = 0;
		}

	private:
		_T* _data;
		size_t _count;
		size_t _capacity;
		
	};
}


#endif // __TARRAY_H__


