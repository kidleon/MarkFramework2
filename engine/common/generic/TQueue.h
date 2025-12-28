#ifndef __TQUEUE_H__
#define __TQUEUE_H__

#include "TList.h"


namespace mark
{
	/**
	* TQueue
	* @brief 템플릿 큐 클래스. 내부적으로 양방향 연결 리스트(list)를 사용(포함)하여 구현됩니다.
	*/
	template<typename _T, typename _Alloc>
	struct TQueue
	{
		typedef const TList::__List_Iterator<_T> const_iterator;
		typedef TList::__List_Iterator<_T> iterator;

		TQueue() = default;

		TQueue(const TQueue<_T>& other)
		{
			_list_node = other._list_node;
		}

		TQueue(const TQueue<_T>&& other)
		{
			_list_node = std::move(other);
		}

		TQueue(std::initializer_list<_T> init_list)
		{
			for (const auto& Value : init_list)
			{
				push_back(Value);
			}
		}

		virtual ~TQueue()
		{
		}

		inline TQueue& operator=(std::initializer_list<_T> init_list)
		{
			clear();
			for (const auto& Value : init_list)
				push_back(Value);

			return *this;
		}

		inline TQueue& operator=(const TQueue<_T>& rhs)
		{
			_list_node = rhs._list_node;
			return *this;
		}

		inline TQueue& operator=(TQueue<_T>&& rhs)
		{
			_list_node = std::move(rhs);
			return *this;
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

		inline _T& front()
		{
			return _list_node.front();
		}

		inline const _T& front() const
		{
			return _list_node.front();
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

	}; // queue
}

#endif // __TQUEUE_H__