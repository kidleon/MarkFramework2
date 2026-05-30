#pragma once
#include "memory_block_pool.h"


namespace mark
{
	template<typename T>
	class TMemoryBlockPool
	{
	public:
		bool Initialize()
		{
			m_hMemoryBlockPool = create_memory_block_pool(sizeof(T), 256);
			if (!m_hMemoryBlockPool)
				return false;

			return m_hMemoryBlockPool != nullptr;
		}

		void Shutdown()
		{
			if (m_hMemoryBlockPool)
			{
				destroy_memory_block_pool(m_hMemoryBlockPool);
				m_hMemoryBlockPool = nullptr;
				m_OutstandingCount = 0;
			}
		}

		T* Acquire()
		{
			if (!m_hMemoryBlockPool)
				return nullptr;

			const memory_block_id blockId = acquire_memory_block(m_hMemoryBlockPool);
			if (blockId.page_index == 0xFFFF)
				return nullptr;

			++m_OutstandingCount;
			T* pObj = reinterpret_cast<T*>(get_memory_block_memory(m_hMemoryBlockPool, blockId));
			new (pObj) T(this);

			pObj->SetMemoryBlockId(blockId);

			return pObj;
		}

		template<typename... Args>
		T* Acquire(Args&&... args)
		{
			if (!m_hMemoryBlockPool)
				return nullptr;

			const memory_block_id blockId = acquire_memory_block(m_hMemoryBlockPool);
			if (blockId.page_index == 0xFFFF)
				return nullptr;

			++m_OutstandingCount;

			T* pObj = reinterpret_cast<T*>(get_memory_block_memory(m_hMemoryBlockPool, blockId));
			new (pObj) T(std::forward<Args>(args)...);

			pObj->SetMemoryBlockId(blockId);

			return pObj;
		}


		void Release(T* pObject)
		{
			if (!m_hMemoryBlockPool || !pObject)
				return;

			const memory_block_id blockId = pObject->GetMemoryBlockId();
			pObject->~T();

			release_memory_block(m_hMemoryBlockPool, blockId);

			--m_OutstandingCount;
		}

	private:
		HANDLE m_hMemoryBlockPool = nullptr;
		std::atomic<int32_t> m_OutstandingCount{ 0 };
	};
}
