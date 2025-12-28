#ifndef __THASH_MAP_H__
#define __THASH_MAP_H__

#include "TCommon.h"
#include "THash.h"


namespace mark
{
	/**
	* THashMap
	* @brief 템플릿 해시 맵 클래스.
	* 해시 맵은 키-값 쌍을 저장하는 데이터 구조로, 키를 사용하여 값을 빠르게 검색할 수 있습니다.
	* 이 구현은 메모리 할당 유형을 지정할 수 있는 템플릿 클래스로, 다양한 데이터 타입에 대해 사용할 수 있습니다.
	* @tparam _Tk 해시 맵의 키 타입
	* @tparam _Tval 해시 맵의 값 타입
	* @tparam HashType 해시 함수 유형 (기본값: HASH_TYPE::FNV)
	* @tparam NUM_BUCKET 버킷 수 (기본값: 32)
	* @tparam _AllocType 메모리 할당 유형 (기본값: ALLOC_TYPE::SYSCALL)
	*/
	template<typename _Tk, typename _Tval, typename _Alloc, size_t NUM_BUCKET = 32, HASH_TYPE HashType = HASH_TYPE::FNV>
	struct THashMap
	{
		template<typename _Tk, typename _Tval>
		struct __HashMap_Node
		{
			TPair<_Tk, _Tval> pair;

			HASH hash;

			__HashMap_Node* prev;
			__HashMap_Node* next;
			__HashMap_Node* chain_prev;
			__HashMap_Node* chain_next;
		};

		template<typename _Tk, typename _Tval>
		struct _HashMap_Iterator
		{
			_HashMap_Iterator() : node(nullptr) {}
			_HashMap_Iterator(__HashMap_Node<_Tk, _Tval>* node) : node(node) {}
			_HashMap_Iterator(const _HashMap_Iterator& other) : node(other.node) {}
			_HashMap_Iterator(_HashMap_Iterator&& other) { node = other.node; other.node = nullptr; }

			inline _HashMap_Iterator& operator=(const _HashMap_Iterator& other) { node = other.node; return *this; }
			inline _HashMap_Iterator& operator=(_HashMap_Iterator&& other) { node = other.node; other.node = nullptr;  return *this; }

			inline __HashMap_Node<_Tk, _Tval>& operator*() { return *node; }
			inline const __HashMap_Node<_Tk, _Tval>& operator*() const { return *node; }

			inline __HashMap_Node<_Tk, _Tval>* operator->() { return node; }
			inline const __HashMap_Node<_Tk, _Tval>* operator->() const { return node; }

			inline const BOOL operator ==(const _HashMap_Iterator& rhs) const { return node == rhs.node; }
			inline const BOOL operator !=(const _HashMap_Iterator& rhs) const { return node != rhs.node; }

			inline _HashMap_Iterator& operator++() { node = node->chain_next; return *this; }
			inline _HashMap_Iterator& operator++(int) { node = node->chain_next; return *this; }
			inline _HashMap_Iterator& operator--() { node = node->chain_prev; return *this; }
			inline _HashMap_Iterator& operator--(int) { node = node->chain_prev; return *this; }

			__HashMap_Node<_Tk, _Tval>* node;
		};

		typedef const _HashMap_Iterator<_Tk, _Tval> const_iterator;
		typedef _HashMap_Iterator<_Tk, _Tval> iterator;

		THashMap() = default;

		THashMap(const THashMap& other)
		{
			copy_other(other);
		}

		THashMap(THashMap&& other)
		{
			move_other(std::move(other));
		}

		virtual ~THashMap()
		{
			destroy();
		}

		inline THashMap& operator=(const THashMap& other) noexcept
		{
			destroy();
			copy_other(other);
			return *this;
		}

		inline THashMap& operator=(THashMap&& other) noexcept
		{
			clear();
			move_other(std::move(other));
			return *this;
		}

		inline _Tval& operator[](const _Tk& key)
		{
			const_iterator it = find(key);
			if (it == end())
			{
				DebugBreak();
				throw std::exception("NOT FOUND ITEM");
			}
			return it.node->pair.value;
		}

		inline bool empty() const noexcept { return _Count ? true : false; }
		inline void clear() noexcept { destroy(); }

		inline size_t size() const noexcept { return _Count; }
		inline size_t count() const noexcept { return _Count; }

		inline void insert(const _Tk& key, const _Tval& value)
		{
			if (contains(key)) // If the key already exists, we do not insert it again.
				return;

			__HashMap_Node<_Tk, _Tval>* pHashNode = static_cast<__HashMap_Node<_Tk, _Tval>*>(_Alloc::alloc(sizeof(__HashMap_Node<_Tk, _Tval>)));

			HASH hash = 0;
			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tk>{}(key);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tk>{}(key);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			pHashNode->hash = hash;
			pHashNode->pair.key = key;
			pHashNode->pair.value = value;
			pHashNode->prev = nullptr;
			pHashNode->next = _Buckets[bucket_index];
			if (pHashNode->next)
				pHashNode->next->prev = pHashNode;

			_Buckets[bucket_index] = pHashNode;

			pHashNode->chain_prev = nullptr;
			pHashNode->chain_next = _Chain_Blocks;
			if (pHashNode->chain_next)
			{
				pHashNode->chain_next->chain_prev = pHashNode;
			}

			_Chain_Blocks = pHashNode;

			_Count++;
		}

		inline void insert(const _Tk& key, _Tval&& value)
		{
			if (contains(key)) // If the key already exists, we do not insert it again.
				return;

			__HashMap_Node<_Tk, _Tval>* pHashNode = static_cast<__HashMap_Node<_Tk, _Tval>*>(_Alloc::alloc(sizeof(__HashMap_Node<_Tk, _Tval>)));

			HASH hash = 0;
			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tk>{}(key);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tk>{}(key);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			pHashNode->hash = hash;
			pHashNode->pair.key = key;
			pHashNode->pair.value = std::move(value);
			pHashNode->prev = nullptr;
			pHashNode->next = _Buckets[bucket_index];
			if (pHashNode->next)
				pHashNode->next->prev = pHashNode;

			_Buckets[bucket_index] = pHashNode;

			pHashNode->chain_prev = nullptr;
			pHashNode->chain_next = _Chain_Blocks;
			if (pHashNode->chain_next)
			{
				pHashNode->chain_next->chain_prev = pHashNode;
			}

			_Chain_Blocks = pHashNode;

			_Count++;
		}

		inline void erase(iterator iter)
		{
			erase(iter.node->pair.key);
		}

		inline void erase(const _Tk& key)
		{
			HASH hash = 0;
			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tk>{}(key);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tk>{}(key);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			__HashMap_Node<_Tk, _Tval>* node = _Buckets[bucket_index];

			while (node)
			{
				if (node->hash != hash)
				{
					node = node->next;
					continue;
				}

				__HashMap_Node<_Tk, _Tval>* pNext = node->next;
				if (!node->pPrev)
				{
					_Buckets[bucket_index] = node->next;

					if (node->next)
						node->next->prev = nullptr;
				}
				else
				{
					node->prev->next = node->next;
					if (node->next)
						node->next->prev = node->prev;
				}

				if (!node->chain_prev)
				{
					if (node->chain_next)
						node->chain_next->chain_prev = nullptr;
					_Chain_Blocks = node->chain_next;
				}
				else
				{
					node->chain_prev->chain_next = node->chain_next;
					if (node->chain_next)
						node->chain_next->chain_prev = node->chain_prev;
				}

				if constexpr (!std::is_trivial_v<_Tval>)
					node->pair.value.~_Tval();

				_Alloc::free((void*)node);

				_Count--;

				node = pNext;
			}
		}

		inline bool contains(const _Tk& key)
		{
			HASH hash = 0;
			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tk>{}(key);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tk>{}(key);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			__HashMap_Node<_Tk, _Tval>* node = _Buckets[bucket_index];
			while (node)
			{
				if (node->hash == hash)
					return true;
				node = node->next;
			}

			return false;
		}

		inline const_iterator find(const _Tk& Key)
		{
			HASH hash = 0;
			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tk>{}(Key);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tk>{}(Key);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			__HashMap_Node<_Tk, _Tval>* node = _Buckets[bucket_index];
			while (node)
			{
				if (node->hash == hash)
					return const_iterator(node);
				node = node->next;
			}

			return end();
		}

		inline bool try_find(const _Tk& key, _Tval& result)
		{
			HASH hash = 0;
			if constexpr (HashType == HASH_TYPE::FNV)
			{
				hash = THash_FNV<_Tk>{}(key);
			}
			else if constexpr (HashType == HASH_TYPE::SDBM)
			{
				hash = THash_SDBM<_Tk>{}(key);
			}
			else
			{
				static_assert(false, "Unsupported Hash Type");
			}

			uint32_t bucket_index = hash % NUM_BUCKET;

			__HashMap_Node<_Tk, _Tval>* node = _Buckets[bucket_index];
			while (node)
			{
				if (node->hash == hash)
				{
					result = node->pair.value;
					return true;
				}
				node = node->next;
			}

			return false;
		}

		inline iterator begin() { return iterator(_Chain_Blocks); }
		inline iterator end() { return iterator(nullptr); }
		inline const_iterator cbegin() { return iterator(_Chain_Blocks); }
		inline const_iterator cend() { return iterator(nullptr); }

	private:
		void copy_other(const THashMap& other)
		{
			for (int i = 0; i < NUM_BUCKET; ++i)
			{
				if (!other._Buckets[i])
					continue;

				__HashMap_Node<_Tk, _Tval>* potherNode = other._Buckets[i];
				while (potherNode)
				{
					insert(potherNode->pair.key, pother->pair.value);
					potherNode = potherNode->next;
				}
			}

			_Count = other._Count;
		}

		void move_other(THashMap&& other) noexcept
		{
			_Count = other._Count;
			for (int i = 0; i < NUM_BUCKET; ++i)
			{
				_Buckets[i] = other._Buckets[i];
			}

			_Chain_Blocks = other._Chain_Blocks;

			other._Count = 0;
			for (int i = 0; i < NUM_BUCKET; ++i)
			{
				other._Buckets[i] = nullptr;
			}
			other._Chain_Blocks = nullptr;
		}

		void destroy() noexcept
		{
			for (int b = 0; b < NUM_BUCKET; b++)
			{
				__HashMap_Node<_Tk, _Tval>* node = _Buckets[b];
				while (node)
				{
					if constexpr (std::is_destructible<_Tval>::value && !std::is_pointer<_Tval>::value)
					{
						node->pair.value.~_Tval();
					}

					__HashMap_Node<_Tk, _Tval>* pNextNode = node->next;

					_Alloc::free((void*)node);

					node = pNextNode;
				}

				_Buckets[b] = nullptr;
			}

			_Chain_Blocks = nullptr;
			_Count = 0;
		}

	private:
		__HashMap_Node<_Tk, _Tval>* _Buckets[NUM_BUCKET] = { nullptr };
		__HashMap_Node<_Tk, _Tval>* _Chain_Blocks = nullptr;
		size_t _Count = 0;
		
	};
}


#endif // __THASH_MAP_H__
