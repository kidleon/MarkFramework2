#include "pch.h"
#include "SurfaceMaterialPool.h"
#include "SurfaceMaterial.h"


namespace mark
{
	SurfaceMaterialPool::~SurfaceMaterialPool() noexcept
	{
		Shutdown();
	}

	bool SurfaceMaterialPool::Initialize()
	{
		m_hMemoryBlockPool = create_memory_block_pool(sizeof(SurfaceMaterial), 256);
		if (!m_hMemoryBlockPool)
		{
			SYS_LOG_ERR("Failed to create memory block pool for SurfaceMaterial.");
			return false;
		}
		return true;
	}

	void SurfaceMaterialPool::Shutdown()
	{
		if (m_hMemoryBlockPool)
		{
			const int32_t outstanding = m_OutstandingCount.load(std::memory_order_acquire);
			if (outstanding != 0)
			{
				SYS_LOG_ERR_F("SurfaceMaterialPool shutdown with {} outstanding material(s) — caller leaked references.", outstanding);
			}

			destroy_memory_block_pool(m_hMemoryBlockPool);
			m_hMemoryBlockPool = nullptr;
		}
	}

	SurfaceMaterial* SurfaceMaterialPool::Acquire()
	{
		memory_block_id blockId = acquire_memory_block(m_hMemoryBlockPool);
		if (blockId.generation == 0)
		{
			return nullptr; // 풀에서 사용할 수 있는 블록이 없음
		}

		void* pBlockMemory = get_memory_block_memory(m_hMemoryBlockPool, blockId);

		m_OutstandingCount.fetch_add(1, std::memory_order_relaxed);
		return new (pBlockMemory) SurfaceMaterial(this, blockId);
	}

	void SurfaceMaterialPool::Release(SurfaceMaterial* pMaterial)
	{
		if (pMaterial)
		{
			memory_block_id blockId = pMaterial->INL_GetBlockId();
			pMaterial->~SurfaceMaterial(); // 명시적으로 소멸자 호출
			release_memory_block(m_hMemoryBlockPool, blockId);
			m_OutstandingCount.fetch_sub(1, std::memory_order_release);
		}
	}
}
