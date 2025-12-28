#ifndef __THASH_SET_H__
#define __THASH_SET_H__

#include "TAllocator.h"
#include "TCommon.h"
#include "THash.h"


namespace mark
{
	/**
	* THashSet
	* @brief 템플릿 해시 셋 클래스.
	* 해시 셋은 고유한 값을 저장하는 데이터 구조로, 값의 존재 여부를 빠르게 확인할 수 있습니다.
	* 이 구현은 메모리 할당 유형을 지정할 수 있는 템플릿 클래스로, 다양한 데이터 타입에 대해 사용할 수 있습니다.
	* @tparam _T 해시 셋의 값 타입
	* @tparam HashType 해시 함수 유형 (기본값: HASH_TYPE::FNV)
	* @tparam NUM_BUCKET 버킷 수 (기본값: 32)
	* @tparam _AllocType 메모리 할당 유형 (기본값: ALLOC_TYPE::SYSCALL)
	*/
	template<typename _Tval, typename _Alloc, size_t NUM_BUCKET = 32, HASH_TYPE HashType = HASH_TYPE::FNV>
	struct THashSet
	{
		template<typename _Tval>
		struct __HashSet_Node
		{
			_Tval val;
			HASH hash;

			__HashSet_Node* prev;
			__HashSet_Node* next;
			__HashSet_Node* chain_prev;
			__HashSet_Node* chain_next;
		};

		template<typename _Tval>
		struct _HashSet_Iterator
		{
			_HashSet_Iterator() : node(nullptr) {}
			_HashSet_Iterator(__HashSet_Node<_Tval>* _pNode) : node(_pNode) {}
			_HashSet_Iterator(const _HashSet_Iterator& other) : node(other.node) {}
			_HashSet_Iterator(_HashSet_Iterator&& other) { node = other.node; other.node = nullptr; }

			inline _HashSet_Iterator& operator=(const _HashSet_Iterator& other) { node = other.node; return *this; }
			inline _HashSet_Iterator& operator=(_HashSet_Iterator&& other) { node = other.node; other.node = nullptr;  return *this; }

			inline __HashSet_Node<_Tval>& operator*() { return *node; }
			inline const __HashSet_Node<_Tval>& operator*() const { return *node; }

			inline __HashSet_Node<_Tval>* operator->() { return node; }
			inline const __HashSet_Node<_Tval>* operator->() const { return node; }

			inline const BOOL operator ==(const _HashSet_Iterator& rhs) const { return node == rhs.node; }
			inline const BOOL operator !=(const _HashSet_Iterator& rhs) const { return node != rhs.node; }

			inline _HashSet_Iterator& operator++() { node = node->chain_next; return *this; }
			inline _HashSet_Iterator& operator++(int) { node = node->chain_next; return *this; }
			inline _HashSet_Iterator& operator--() { node = node->chain_prev; return *this; }
			inline _HashSet_Iterator& operator--(int) { node = node->chain_prev; return *this; }

			__HashSet_Node<_Tval>* node;
		};

		typedef const _HashSet_Iterator<_Tval> const_iterator;
		typedef _HashSet_Iterator<_Tval> iterator;

		THashSet() = default;

		THashSet(const THashSet& other)
		{
			copy_other(other);
		}

		THashSet(THashSet&& other)
		{
			move_other(std::move(other));
		}

		virtual ~THashSet()
		{
			destroy();
		}

		inline THashSet& operator=(const THashSet& other) noexcept
		{
			destroy();
			copy_other(other);
			return *this;
		}

		inline THashSet& operator=(THashSet&& other) noexcept
		{
			clear();
			move_other(std::move(other));
			return *this;
		}

		inline bool empty() const noexcept { return _Count ? true : false; }
		inline void clear() noexcept { destroy(); }

		inline size_t size() const noexcept { return _Count; }
		inline size_t count() const noexcept { return _Count; }

		inline void insert(const _Tval& val)
		{
			if (contains(val)) return;

			__HashSet_Node<_Tval>* pHashNode = static_cast<__HashSet_Node<_Tval>*>(_Alloc::alloc(sizeof(__HashSet_Node<_Tval>)));

			HASH hash;

			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tval>{}(val);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tval>{}(val);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			pHashNode->hash = hash;
			pHashNode->val = val;
			pHashNode->prev = nullptr;
			pHashNode->next = _pBuckets[bucket_index];
			if (pHashNode->next)
				pHashNode->next->prev = pHashNode;

			_pBuckets[bucket_index] = pHashNode;

			pHashNode->chain_prev = nullptr;
			pHashNode->chain_next = _pChainBlocks;
			if (pHashNode->chain_next)
			{
				pHashNode->chain_text->chain_prev = pHashNode;
			}

			_pChainBlocks = pHashNode;

			_Count++;
		}

		inline void insert(_Tval&& val)
		{
			if (contains(val)) return;

			__HashSet_Node<_Tval>* pHashNode = static_cast<__HashSet_Node<_Tval>*>(_Alloc::alloc(sizeof(__HashSet_Node<_Tval>)));

			HASH hash;

			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tval>{}(val);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tval>{}(val);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			pHashNode->hash = hash;
			pHashNode->val = std::move(val);
			pHashNode->prev = nullptr;
			pHashNode->next = _pBuckets[bucket_index];
			if (pHashNode->next)
				pHashNode->next->prev = pHashNode;

			_pBuckets[bucket_index] = pHashNode;

			pHashNode->chain_prev = nullptr;
			pHashNode->chain_next = _pChainBlocks;
			if (pHashNode->chain_next)
			{
				pHashNode->chain_next->chain_prev = pHashNode;
			}

			_pChainBlocks = pHashNode;

			_Count++;
		}

		inline void erase(iterator iter)
		{
			erase(iter.node->val);
		}

		inline void erase(const _Tval& Val)
		{
			HASH Hash;

			if constexpr (HashType == HASH_TYPE::FNV)
			{
				Hash = THash_FNV<_Tval>{}(Val);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				Hash = THash_SDBM<_Tval>{}(Val);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = Hash % NUM_BUCKET;

			__HashSet_Node<_Tval>* pNode = _pBuckets[bucket_index];

			while (pNode)
			{
				if (pNode->hash != Hash)
				{
					pNode = pNode->next;
					continue;
				}

				__HashSet_Node<_Tval>* pNext = pNode->next;
				if (!pNode->prev)
				{
					_pBuckets[bucket_index] = pNode->next;

					if (pNode->next)
						pNode->next->prev = nullptr;
				}
				else
				{
					pNode->prev->next = pNode->next;
					if (pNode->next)
						pNode->next->prev = pNode->prev;
				}

				if (!pNode->chain_prev)
				{
					if (pNode->chain_next)
						pNode->chain_next->chain_prev = nullptr;
					_pChainBlocks = pNode->chain_next;
				}
				else
				{
					pNode->chain_prev->chain_next = pNode->chain_next;
					if (pNode->chain_next)
						pNode->chain_next->chain_prev = pNode->chain_prev;
				}

				if constexpr (!std::is_trivial_v<_Tval>)
					pNode->value.~_Tval();

				_Alloc::free((void*)pNode);

				_Count--;

				pNode = pNext;
			}
		}

		inline bool contains(const _Tval& Val)
		{
			HASH hash;

			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tval>{}(Val);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tval>{}(Val);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			__HashSet_Node<_Tval>* pNode = _pBuckets[bucket_index];
			while (pNode)
			{
				if (pNode->hash == hash)
					return true;
				pNode = pNode->next;
			}

			return false;
		}

		inline const_iterator find(const _Tval& val)
		{
			HASH hash;

			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tval>{}(val);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tval>{}(val);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			__HashSet_Node<_Tval>* pNode = _pBuckets[bucket_index];
			while (pNode)
			{
				if (pNode->hash == hash)
					return const_iterator(pNode);
				pNode = pNode->next;
			}

			return end();
		}

		inline iterator begin() { return iterator(_pChainBlocks); }
		inline iterator end() { return iterator(nullptr); }
		inline const_iterator cbegin() { return iterator(_pChainBlocks); }
		inline const_iterator cend() { return iterator(nullptr); }

	private:
		void copy_other(const THashSet& other)
		{
			for (int i = 0; i < NUM_BUCKET; ++i)
			{
				if (!other._pBuckets[i])
					continue;

				__HashSet_Node<_Tval>* pOtherNode = other._pBuckets[i];
				while (pOtherNode)
				{
					insert(pOtherNode->value);
					pOtherNode = pOtherNode->next;
				}
			}

			_Count = other._Count;
		}

		void move_other(THashSet&& other) noexcept
		{
			_Count = other._Count;
			for (int i = 0; i < NUM_BUCKET; ++i)
			{
				_pBuckets[i] = other._pBuckets[i];
			}

			_pChainBlocks = other._pChainBlocks;

			other._Count = 0;
			for (int i = 0; i < NUM_BUCKET; ++i)
			{
				other._pBuckets[i] = nullptr;
			}
			other._pChainBlocks = nullptr;
		}

		void destroy() noexcept
		{
			for (int b = 0; b < NUM_BUCKET; b++)
			{
				__HashSet_Node<_Tval>* pNode = _pBuckets[b];
				while (pNode)
				{
					if constexpr (std::is_destructible<_Tval>::value && !std::is_pointer<_Tval>::value)
					{
						pNode->val.~_Tval();
					}

					__HashSet_Node<_Tval>* pNextNode = pNode->next;

					_Alloc::free((void*)pNode);

					pNode = pNextNode;
				}
				_pBuckets[b] = nullptr;
			}

			_pChainBlocks = nullptr;

			_Count = 0;
		}

	private:
		__HashSet_Node<_Tval>* _pBuckets[NUM_BUCKET] = { nullptr };
		__HashSet_Node<_Tval>* _pChainBlocks = nullptr;
		size_t _Count = 0;

	};

}

#endif // __THASH_SET_H__