#ifndef __DEQUE_H__
#define __DEQUE_H___

#include "TList.h"


namespace mark
{
	/**
	* TDeque
	* @brief 템플릿 덱 클래스. 내부적으로 양방향 연결 리스트(list)를 사용(포함)하여 구현됩니다.
	*/
	template<typename _T, typename _Alloc>
	struct TDeque
	{
		typedef const TList::__List_Iterator<_T> const_iterator;
		typedef TList::__List_Iterator<_T> iterator;

		TDeque() = default;

		TDeque(const TDeque<_T>& other)
		{
			_list_node = other._list_node;
		}

		TDeque(const TDeque<_T>&& other)
		{
			_list_node = std::move(other);
		}

		TDeque(std::initializer_list<_T> init_list)
		{
			for (const auto& Value : init_list)
			{
				push_back(Value);
			}
		}

		virtual ~TDeque()
		{
		}

		inline TDeque& operator=(std::initializer_list<_T> init_list)
		{
			clear();
			for (const auto& Value : init_list)
				push_back(Value);

			return *this;
		}

		inline TDeque& operator=(const TDeque<_T>& rhs)
		{
			_list_node = rhs._list_node;
			return *this;
		}

		inline TDeque& operator=(TDeque<_T>&& rhs)
		{
			_list_node = std::move(rhs);
			return *this;
		}

		inline void push_front(const _T& val)
		{
			_list_node.push_front(val);
		}

		inline void push_front(_T&& val)
		{
			_list_node.push_front(val);
		}

		inline void push_back(const _T& val)
		{
			_list_node.push_back(val);
		}

		inline void push_back(_T&& val)
		{
			_list_node.push_back(val);
		}

		inline void pop_front()
		{
			_list_node.pop_front();
		}

		inline void pop_back()
		{
			_list_node.pop_back();
		}

		inline _T& front()
		{
			return _list_node.front();
		}

		inline const _T& front() const
		{
			return _list_node.front();
		}

		inline _T& back()
		{
			return _list_node.back();
		}

		inline const _T& back() const
		{
			return _list_node.back();
		}

		inline void clear()
		{
			_list_node.clear();
		}

		inline bool empty() const noexcept
		{
			return _list_node.empty();
		}

		inline size_t size() const noexcept
		{
			return _list_node.size();
		}

		inline size_t count() const noexcept
		{
			return _list_node.count();
		}

		// Ranged-for
		inline const_iterator begin() const
		{
			return _list_node.begin();
		}

		inline iterator begin()
		{
			return _list_node.begin();
		}

		inline const_iterator end() const
		{
			return _list_node.end();
		}

		inline iterator end()
		{
			return _list_node.end();
		}

	private:
		TList<_T, _Alloc> _list_node;

	}; // deque
}

#endif __DEQUE_H__
