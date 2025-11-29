#ifndef __LIST_H__
#define __LIST_H__

#include <initializer_list>
#include "TAllocator.h"
#include "TCommon.h"


namespace mark
{
	/**
	* TList
	* @brief 템플릿 양방향 연결 리스트 클래스.
	* 양방향 연결 리스트는 각 노드가 이전 및 다음 노드에 대한 포인터를 포함하는 데이터 구조입니다. 
	* 이 구현은 메모리 할당 유형을 지정할 수 있는 템플릿 클래스로, 다양한 데이터 타입에 대해 사용할 수 있습니다.
	* @tparam _T 리스트에 저장할 데이터 타입
	* @tparam _AllocType 메모리 할당 유형 (기본값: ALLOC_TYPE_SYSCALL)
	*/
	template<typename _T, ALLOC_TYPE _AllocType = ALLOC_TYPE::SYSCALL>
	struct TList
	{
		template<typename _T>
		struct __List_Node
		{
			_T val;
			__List_Node* prev;
			__List_Node* next;
		};

		template<typename _T>
		struct __List_Iterator
		{
			__List_Node<_T>* node;

			__List_Iterator() : node(nullptr) {}
			__List_Iterator(__List_Node<_T>* node) : node(node) {}
			__List_Iterator(const __List_Iterator& other) noexcept : node(other.node) {}
			__List_Iterator(__List_Iterator&& other) noexcept : node(other.node)
			{
				other.node = nullptr;
			}

			inline __List_Iterator& operator=(const __List_Iterator& other)
			{
				node = other.node;
				return *this;
			}

			inline __List_Iterator& operator=(__List_Iterator&& other)
			{
				node = other.node;
				Ohter.node = nullptr;
				return *this;
			}

			inline _T& operator*() { return node->val; }
			inline const _T& operator*() const { return node->val; }

			inline _T& operator->() { return node->val; }
			inline const _T& operator->() const { return node->val; }

			inline const bool operator==(const __List_Iterator<_T>& rhs) const { return node == rhs.node; }
			inline const bool operator!=(const __List_Iterator<_T>& rhs) const { return node != rhs.node; }

			inline const bool operator<(const __List_Iterator<_T>& rhs) const { return node->val < rhs.node->val; }
			inline const bool operator>(const __List_Iterator<_T>& rhs) const { return node->val > rhs.node->val; }

			inline const bool operator>=(const __List_Iterator<_T>& rhs) const { return !(node->val < rhs.node->val); }
			inline const bool operator<=(const __List_Iterator<_T>& rhs) const { return !(node->val > rhs.node->val); }

			inline __List_Iterator& operator++() { node = node->next; return *this; }
			inline __List_Iterator& operator--() { node = node->prev; return *this; }
			inline __List_Iterator& operator++(int) { node = node->next; return *this; }
			inline __List_Iterator& operator--(int) { node = node->prev; return *this; }

		};

		using value_type = _T;
		typedef const __List_Iterator<value_type> const_iterator;
		typedef __List_Iterator<value_type> iterator;

		TList() = default;
		TList(const TList<_T>& other)
		{
			copy_other(other);
		}

		TList(TList<_T>&& other)
		{
			_count = other._count;
			_head = other._head;
			_tail = other._tail;

			other._count = 0;
			other._head = nullptr;
			other._tail = nullptr;
		}

		TList(std::initializer_list<value_type> init_list)
		{
			for (const auto& val : init_list)
				push_back(val);
		}

		TList<value_type>& operator=(std::initializer_list<value_type> init_list)
		{
			clear();
			for (const auto& value : init_list)
				push_back(value);

			return *this;
		}

		virtual ~TList()
		{
			clear();
		}

		inline TList& operator=(const TList<value_type>& rhs)
		{
			copy_other(rhs);
			return *this;
		}

		inline TList& operator=(TList<value_type>&& rhs)
		{
			_count = rhs._count;
			_head = rhs._head;
			_tail = rhs._tail;

			rhs._count = 0;
			rhs._head = nullptr;
			rhs._tail = nullptr;

			return *this;
		}

		/**
		* @brief 리스트의 앞에 값을 추가합니다.
		* @param Value 추가할 값
		* @return 없음
		*/
		void push_front(const value_type& value)
		{
			__List_Node<value_type>* new_node = GENERIC_ALLOC(sizeof(__List_Node<value_type>), _AllocType);

			if (!new_node)
			{
				__ASSERT(0, "Memory allocation failed in TList::push_front");
				return;
			}

			new_node->val = value;
			new_node->prev = nullptr;
			new_node->next = nullptr;

			if (_head)
			{
				new_node->next = _head;
				_head->prev = new_node;
				_head = new_node;
			}
			else
			{
				_head = new_node;
				_tail = new_node;
			}

			++_count;
		}

		/**
		* @brief 리스트의 앞에 이동된 값을 추가합니다.
		* @param Value 이동할 값 (rvalue 참조)
		* @return 없음
		*/
		void push_front(value_type&& value)
		{
			__List_Node<value_type>* new_node = GENERIC_ALLOC(sizeof(__List_Node<value_type>), _AllocType);

			if (!new_node)
			{
				__ASSERT(0, "Memory allocation failed in TList::push_front");
				return;
			}

			new_node->val = std::move(value);
			new_node->prev = nullptr;
			new_node->next = nullptr;

			if (_head)
			{
				new_node->next = _head;
				_head->prev = new_node;
				_head = new_node;
			}
			else
			{
				_head = new_node;
				_tail = new_node;
			}

			++_count;
		}

		/**
		* @brief 리스트의 끝에 값을 추가합니다.
		* @param Value 추가할 값 (const 참조)
		* @return 없음
		*/
		void push_back(const value_type& value)
		{
			__List_Node<value_type>* new_node = GENERIC_ALLOC(sizeof(__List_Node<value_type>), _AllocType);

			if (!new_node)
			{
				__ASSERT(0, "Memory allocation failed in TList::push_front");
				return;
			}

			new_node->val = value;
			new_node->prev = nullptr;
			new_node->next = nullptr;

			if (_tail)
			{
				_tail->next = new_node;
				new_node->prev = _tail;
				_tail = new_node;
			}
			else
			{
				_head = new_node;
				_tail = new_node;
			}

			++_count;
		}

		/**
		* @brief 리스트의 끝에 이동된 값을 추가합니다.
		* @param Value 이동할 값 (rvalue 참조)
		* @return 없음
		*/
		void push_back(value_type&& value)
		{
			__List_Node<value_type>* new_node = GENERIC_ALLOC(sizeof(__List_Node<value_type>), _AllocType);

			if (!new_node)
			{
				__ASSERT(0, "Memory allocation failed in TList::push_front");
				return;
			}

			new_node->val = std::move(value);
			new_node->prev = nullptr;
			new_node->next = nullptr;

			if (_tail)
			{
				_tail->next = new_node;
				new_node->prev = _tail;
				_tail = new_node;
			}
			else
			{
				_head = new_node;
				_tail = new_node;
			}

			++_count;
		}

		/**
		* @brief 리스트의 앞에서 값을 제거합니다.
		* @return 없음
		*/
		void pop_front()
		{
			if (!_count || !_head) return;

			__List_Node<value_type>* head = _head;
			_head = _head->next;

			if (_head)
				_head->prev = nullptr;
			--_count;

			if constexpr (std::is_destructible<value_type>::value && !std::is_pointer<value_type>::value)
			{
				destroy(&head->val);
			}

			GENERIC_FREE(head, _AllocType);
		}

		/**
		* @brief 리스트의 끝에서 값을 제거
		* @return 없음
		*/
		void pop_back()
		{
			if (!_count || !_tail) return;

			__List_Node<value_type>* tail = _tail;

			_tail = _tail->prev;
			if (_tail)
				_tail->next = nullptr;
			--_count;

			if constexpr (std::is_destructible<value_type>::value && !std::is_pointer<value_type>::value)
			{
				destroy(&tail->val);
			}

			GENERIC_FREE(tail, _AllocType);
		}

		/**
		* @brief 지정된 위치에 값을 삽입합니다.
		* @param position 삽입할 위치의 이터레이터
		* @param Value 삽입할 값 (rvalue 참조)
		* @return 없음
		*/
		void insert(iterator position, value_type&& val)
		{
			__ASSERT(_count == 0, "position == begin() But Count is not zero");
			__List_Node<value_type>* new_node = GENERIC_ALLOC(sizeof(__List_Node<value_type>), _AllocType);

			if (!new_node)
			{
				__ASSERT(0, "Memory allocation failed in TList::insert");
				return;
			}

			new_node->val = std::move(val);
			new_node->prev = nullptr;
			new_node->next = nullptr;

			if (position == begin())
			{
				if (!_head)
				{
					_head = new_node;
					_tail = new_node;
				}
				else
				{
					new_node->next = _head;
					_head->prev = new_node;
					_head = new_node;
				}
			}
			else if (position == end())
			{
				if (!_tail)
				{
					_head = new_node;
					_tail = new_node;
				}
				else
				{
					_tail->next = new_node;
					new_node->prev = _tail;
					_tail = new_node;
				}
			}
			else
			{
				__ASSERT(position.node->prev, "INVALID PREV POSITION");
				__ASSERT(position.node->next, "INVALID NEXT POSITION");

				position.node->prev->next = new_node;
				new_node->prev = position.node->prev;
				new_node->next = position.node;
				position.node->prev = new_node;
			}

			++_count;
		}

		/**
		* @brief 리스트의 앞의 값을 반환합니다.
		* @return 앞의 값의 참조
		*/
		inline value_type& front()
		{
			if (!_count || !_head)
			{
				__ASSERT(false, "ZERO ELEMENT...");
				throw;
			}

			return _head->val;
		}

		/**
		* @brief 리스트의 끝의 값을 반환합니다.
		* @return 끝의 값의 참조
		*/
		inline value_type& back()
		{
			if (!_count || !_tail)
			{
				__ASSERT(false, "ZERO ELEMENT...");
				throw;
			}

			return _tail->val;
		}

		/**
		* @brief 리스트의 모든 요소를 제거합니다.
		* @return 없음
		*/
		void clear()
		{
			if (_count)
			{
				__List_Node<value_type>* node = _head;
				while (node)
				{
					__List_Node<value_type>* next = node->next;

					if constexpr (std::is_destructible<value_type>::value && !std::is_pointer<value_type>::value)
					{
						destroy(&node->val);
					}

					GENERIC_FREE(node, _AllocType);
					
					node = next;
				}
			}

			_count = 0;
			_head = _tail = nullptr;
		}

		/**
		* @brief 리스트가 비어있는지 확인합니다.
		* @return 비어있으면 true, 아니면 false
		*/
		inline bool empty() const noexcept
		{
			return _count == 0 ? true : false;
		}

		/**
		* @brief 리스트의 요소 개수를 반환합니다. (동일한 기능의 count() 메서드와 동일)
		* @return 요소 개수
		*/
		inline size_t size() const noexcept
		{
			return _count;
		}

		/**
		* @brief 리스트의 요소 개수를 반환합니다. (동일한 기능의 size() 메서드와 동일)
		* @return 요소 개수
		*/
		inline size_t count() const noexcept
		{
			return _count;
		}

		/**
		* @brief 리스트에서 특정 값을 찾습니다.
		* @param Value 찾을 값
		* @return 값이 있으면 해당 이터레이터, 없으면 end() 이터레이터
		*/
		iterator find(const value_type& val)
		{
			__List_Node<value_type>* node = _head;

			while (node)
			{
				if (node->val == val) 
					return iterator(node);
				node = node->next;
			}

			return iterator(nullptr);
		}

		/**
		* @brief 리스트에서 특정 조건을 만족하는 값을 찾습니다.
		* @tparam Pred 조건을 나타내는 함수 객체 타입
		* @param pred 찾을 조건을 나타내는 함수 객체
		* @return 조건을 만족하는 값이 있으면 해당 이터레이터, 없으면 end() 이터레이터
		*/
		template<class Pred>
		iterator find(Pred pred)
		{
			__List_Node<value_type>* node = _head;

			while (node)
			{
				if (pred(node->val)) 
					return iterator(node);
				node = node->next;
			}

			return iterator(nullptr);
		}

		/**
		* @brief 리스트에서 특정 조건을 만족하는 값을 찾습니다.
		* @tparam Pred 조건을 나타내는 함수 객체 타입
		* @param pred 찾을 조건을 나타내는 함수 객체
		* @param args 조건 함수에 전달할 추가 인자들
		* @return 조건을 만족하는 값이 있으면 해당 이터레이터, 없으면 end() 이터레이터
		*/
		template<class Pred, typename ...Args>
		iterator find(Pred pred, Args... args)
		{
			__List_Node<value_type>* node = _head;
			while (node)
			{
				if (pred(node->val, std::forward<Args>(args)...)) return iterator(node);
				node = node->next;
			}

			return iterator(nullptr);
		}

		/**
		* @brief 리스트에서 특정 값을 제거합니다.
		* @param Value 제거할 값
		* @return 없음
		*/
		void erase(const value_type& val)
		{
			iterator it = find(val);
			if (end() == it)
				return;

			erase(it);
		}

		/**
		* @brief 리스트에서 특정 이터레이터 위치의 값을 제거합니다.
		* @param it 제거할 위치의 이터레이터
		* @return 없음
		*/
		void erase(iterator it)
		{
			if (!it.node) return;

			if (!it.node->prev && !it.node->next)  // COUNT 1, Head == Tail == Node
			{
				__ASSERT(COUNT == 1, "INVALID ITERATOR");

				if constexpr (std::is_destructible<value_type>::value && !std::is_pointer<value_type>::value)
				{
					destroy(&it.node->val);
				}

				GENERIC_FREE(it.node, _AllocType);

				_head = nullptr;
				_tail = nullptr;
				_count = 0;

				return;
			}
			else if (!it.node->prev && it.node->next) // HEAD == NODE
			{
				__ASSERT(it.node == _head, "INVALID HEAD NODE");
				pop_front();
				return;
			}
			else if (it.node->prev && !it.node->next)
			{
				__ASSERT(it.node == _tail, "INVALID TAIL NODE");
				pop_back();
				return;
			}
			else
			{
				__List_Node<_T>* node = it.node;
				node->prev->next = node->next;
				node->next->prev = node->prev;

				if constexpr (std::is_destructible<value_type>::value && !std::is_pointer<value_type>::value)
				{
					destroy(&it.node->val);
				}

				GENERIC_FREE(it.node, _AllocType);
				--_count;
			}
		}

		// TList는 연속된 선형 메모리가 아니기 때문에 이터레이션을 이용해 병합정렬을 사용한다. 좀느림..
		template<class Pred>
		void sort(Pred pred)
		{
			if (!_head || !_tail) return;

			merge_sort(_head, pred);

			_head->prev = nullptr;

			__List_Node<value_type>* node = _head;
			while (node)
			{
				if (node->next)
					node->next->prev = node;
				else
					_tail = node;

				node = node->next;
			}
		}

		inline void sort()
		{
			sort([](const value_type& a, const value_type& b)
				{
					return a < b;
				});
		}

		// Ranged-for
		inline const_iterator begin() const
		{
			return const_iterator(_head);
		}

		inline iterator begin()
		{
			return iterator(_head);
		}

		inline const_iterator end() const
		{
			return const_iterator(nullptr);
		}

		inline iterator end()
		{
			return iterator(nullptr);
		}

	private:
		void copy_other(const TList<value_type>& Other)
		{
			clear();

			if (!Other._count)
				return;

			__List_Node<value_type>* pOtherNode = Other._head;
			__List_Node<value_type>* pMyNode = nullptr;
			while (pOtherNode)
			{
				push_back(pOtherNode->val);
				pOtherNode = pOtherNode->next;
			}
		}

		template<class Pred>
		void merge_sort(__List_Node<value_type>*& pHead, Pred pred)
		{
			if (pHead->next != nullptr)
			{
				__List_Node<value_type>* pHead1 = nullptr;
				__List_Node<value_type>* pHead2 = pHead;
				size_t len = get_node_count(pHead);
				for (size_t i = 0; i < len / 2; ++i)
				{
					pHead1 = pHead2;
					pHead2 = pHead2->next;
				}

				if (pHead1)
					pHead1->next = nullptr;
				pHead1 = pHead;
				merge_sort(pHead1, pred);
				merge_sort(pHead2, pred);
				pHead = merge(pHead1, pHead2, pred);
			}
		}

		template<class Pred>
		__List_Node<value_type>* merge(__List_Node<value_type>*& pHead1, __List_Node<value_type>*& pHead2, Pred pred)
		{
			__List_Node<value_type>* pNewHead;

			if (pHead1 == nullptr) return pHead2;
			else if (pHead2 == nullptr) return pHead1;

			//compare the value
			if (pred(pHead1->val, pHead2->val))
			{
				pNewHead = pHead1;
				pNewHead->next = merge(pHead1->next, pHead2, pred);
			}
			else
			{
				pNewHead = pHead2;
				pNewHead->next = merge(pHead1, pHead2->next, pred);
			}

			return pNewHead;
		}

		inline size_t get_node_count(__List_Node<value_type>* node)
		{
			__List_Node<value_type>* cur = node;
			size_t i = 0;
			for (; cur != nullptr; cur = cur->next)
			{
				i++;
			}
			return i;
		}

	protected:
			__List_Node<value_type>* _head = nullptr;
			__List_Node<value_type>* _tail = nullptr;
			size_t _count = 0;

	};
}


#endif // __LIST_H__
