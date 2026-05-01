#include "pch.h"
#include "D3D11BufferPool.h"
#include "D3D11Buffer.h"


namespace mark
{
	enum class POOL_BUFFER_TYPE {
		VERTEX_BUFFER, INDEX_BUFFER, CONSTANT_BUFFER, EMAX
	};

	enum class POOL_BUFFER_USAGE {
		DEFAULT, DYNAMIC, EMAX
	};

	enum class POOL_BUFFER_SIZE {
		SIZE_256, SIZE_512, SIZE_1024, SIZE_2048,
		SIZE_4096, SIZE_8192, SIZE_16384, SIZE_32768,
		SIZE_65536, SIZE_131072, SIZE_262144, SIZE_524288,
		SIZE_1048576, SIZE_2097152, EMAX
	};

	static constexpr size_t POOL_BUFFER_SIZES[] = {
		256, 512, 1024, 2048,
		4096, 8192, 16384, 32768,
		65536, 131072, 262144, 524288,
		1048576, 2097152
	};

	constexpr static size_t BUFFER_COUNT_PER_PAGE[] = {
		128, 128, 128, 128,
		64, 64, 64, 64,
		32, 32, 32, 32,
		16, 16
	};

	static POOL_BUFFER_USAGE MapBufferUsage(BUFFER_USAGE usage)
	{
		switch (usage)
		{
		case BUFFER_USAGE::DYNAMIC:
			return POOL_BUFFER_USAGE::DYNAMIC;
		default:
			return POOL_BUFFER_USAGE::DEFAULT;
		}
	}

	static POOL_BUFFER_SIZE MapBufferSize(size_t size)
	{
		for (int i = 0; i < (int)POOL_BUFFER_SIZE::EMAX; ++i)
		{
			if (size <= POOL_BUFFER_SIZES[i])
				return (POOL_BUFFER_SIZE)i;
		}
		return POOL_BUFFER_SIZE::EMAX;
	}

	D3D11BufferPool::~D3D11BufferPool() noexcept
	{
		Shutdown();
	}

	bool D3D11BufferPool::GetBuffer(
		BUFFER_TYPE BufferType,
		BUFFER_USAGE BufferUsage,
		size_t BufferSize,
		D3D11Buffer** ppOut
	)
	{
		if (!ppOut)
			return false;

		POOL_BUFFER_TYPE PoolType;
		switch (BufferType)
		{
		case BUFFER_TYPE::VERTEX_BUFFER:
			PoolType = POOL_BUFFER_TYPE::VERTEX_BUFFER;
			break;

		case BUFFER_TYPE::INDEX_BUFFER:
			PoolType = POOL_BUFFER_TYPE::INDEX_BUFFER;
			break;

		case BUFFER_TYPE::CONSTANT_BUFFER:
			PoolType = POOL_BUFFER_TYPE::CONSTANT_BUFFER;
			break;

		default:
			return false;

		}

		POOL_BUFFER_USAGE PoolUsage = MapBufferUsage(BufferUsage);
		POOL_BUFFER_SIZE  PoolSize = MapBufferSize(BufferSize);

		if (PoolSize == POOL_BUFFER_SIZE::EMAX)
			return false;

		int usageIdx = (int)PoolUsage;
		int sizeIdx = (int)PoolSize;

		BufferPageGroup* pGroup = nullptr;
		switch (PoolType)
		{
		case POOL_BUFFER_TYPE::VERTEX_BUFFER:
			pGroup = &m_VBPool[usageIdx][sizeIdx];
			break;

		case POOL_BUFFER_TYPE::INDEX_BUFFER:
			pGroup = &m_IBPool[usageIdx][sizeIdx]; break;
			break;

		case POOL_BUFFER_TYPE::CONSTANT_BUFFER:
			pGroup = &m_CBPool[usageIdx][sizeIdx];
			break;

		default:
			return false;
		}

		{
			AUTO_SPIN_LOCK guard(&pGroup->SpinLock);
			if (!pGroup->FreeBuffers.empty())
			{
				BufferIndex idx = pGroup->FreeBuffers.front();
				pGroup->FreeBuffers.pop_front();

				D3D11Buffer* pBuf = &pGroup->BufferPages[idx.PageIndex].pBufferSlots[idx.SlotIndex];
				pBuf->INL_SetPoolIndex(idx.PageIndex, idx.SlotIndex);

				*ppOut = pBuf;

				return true;
			}
		}

		if (!CreateBufferPage(PoolType, PoolUsage, PoolSize))
			return false;

		{
			AUTO_SPIN_LOCK guard(&pGroup->SpinLock);
			if (pGroup->FreeBuffers.empty())
				return false;

			BufferIndex idx = pGroup->FreeBuffers.front();
			pGroup->FreeBuffers.pop_front();

			D3D11Buffer* pBuf = &pGroup->BufferPages[idx.PageIndex].pBufferSlots[idx.SlotIndex];
			pBuf->INL_SetPoolIndex(idx.PageIndex, idx.SlotIndex);
			*ppOut = pBuf;
			return true;
		}
	}

	void D3D11BufferPool::ReleaseBuffer(D3D11Buffer* pBuffer)
	{
		if (!pBuffer)
			return;

		POOL_BUFFER_TYPE poolType;
		switch (pBuffer->INL_GetBufferType())
		{
		case BUFFER_TYPE::VERTEX_BUFFER:   poolType = POOL_BUFFER_TYPE::VERTEX_BUFFER;   break;
		case BUFFER_TYPE::INDEX_BUFFER:    poolType = POOL_BUFFER_TYPE::INDEX_BUFFER;    break;
		case BUFFER_TYPE::CONSTANT_BUFFER: poolType = POOL_BUFFER_TYPE::CONSTANT_BUFFER; break;
		default: return;
		}

		POOL_BUFFER_USAGE poolUsage = MapBufferUsage(pBuffer->INL_GetBufferUsage());
		POOL_BUFFER_SIZE  poolSize = MapBufferSize(pBuffer->INL_GetBufferSize());
		if (poolSize == POOL_BUFFER_SIZE::EMAX)
			return;

		int usageIdx = (int)poolUsage;
		int sizeIdx = (int)poolSize;

		D3D11BufferPageGroup* pGroup = nullptr;
		switch (poolType)
		{
		case POOL_BUFFER_TYPE::VERTEX_BUFFER:   pGroup = &m_VBPool[usageIdx][sizeIdx]; break;
		case POOL_BUFFER_TYPE::INDEX_BUFFER:    pGroup = &m_IBPool[usageIdx][sizeIdx]; break;
		case POOL_BUFFER_TYPE::CONSTANT_BUFFER: pGroup = &m_CBPool[usageIdx][sizeIdx]; break;
		default: return;
		}

		BufferIndex idx;
		idx.PageIndex = pBuffer->INL_GetPageIndex();
		idx.SlotIndex = pBuffer->INL_GetSlotIndex();

		AUTO_SPIN_LOCK guard(&pGroup->SpinLock);
		pGroup->FreeBuffers.push_back(idx);
	}

		bool D3D11BufferPool::CreateBufferPage(
			POOL_BUFFER_TYPE BufferType,
			POOL_BUFFER_USAGE BufferUsage,
			POOL_BUFFER_SIZE BufferSize
		)
		{
			if (!m_pD3D11Device)
				return false;

			int usageIdx = (int)BufferUsage;
			int sizeIdx = (int)BufferSize;

			D3D11BufferPageGroup* pGroup = nullptr;
			switch (BufferType)
			{
			case POOL_BUFFER_TYPE::VERTEX_BUFFER:   pGroup = &m_VBPool[usageIdx][sizeIdx]; break;
			case POOL_BUFFER_TYPE::INDEX_BUFFER:    pGroup = &m_IBPool[usageIdx][sizeIdx]; break;
			case POOL_BUFFER_TYPE::CONSTANT_BUFFER: pGroup = &m_CBPool[usageIdx][sizeIdx]; break;
			default: return false;
			}

			uint32_t numBuffers = (uint32_t)BUFFER_COUNT_PER_PAGE[sizeIdx];
			size_t   bufBytes = POOL_BUFFER_SIZES[sizeIdx];

			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = (UINT)bufBytes;
			desc.StructureByteStride = 0;
			desc.MiscFlags = 0;

			switch (BufferType)
			{
			case POOL_BUFFER_TYPE::VERTEX_BUFFER:   desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   break;
			case POOL_BUFFER_TYPE::INDEX_BUFFER:    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;    break;
			case POOL_BUFFER_TYPE::CONSTANT_BUFFER: desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; break;
			default: return false;
			}

			switch (BufferUsage)
			{
			case POOL_BUFFER_USAGE::DYNAMIC:
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;
			default:
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.CPUAccessFlags = 0;
				break;
			}

			BUFFER_TYPE  extType;
			BUFFER_USAGE extUsage;
			switch (BufferType)
			{
			case POOL_BUFFER_TYPE::VERTEX_BUFFER:   extType = BUFFER_TYPE::VERTEX_BUFFER;   break;
			case POOL_BUFFER_TYPE::INDEX_BUFFER:    extType = BUFFER_TYPE::INDEX_BUFFER;    break;
			case POOL_BUFFER_TYPE::CONSTANT_BUFFER: extType = BUFFER_TYPE::CONSTANT_BUFFER; break;
			default: return false;
			}
			extUsage = (BufferUsage == POOL_BUFFER_USAGE::DYNAMIC) ? BUFFER_USAGE::DYNAMIC : BUFFER_USAGE::DEFAULT;

			D3D11Buffer* pSlots = (D3D11Buffer*)CORE_SYS_ALLOC(sizeof(D3D11Buffer) * numBuffers);
			if (!pSlots)
				return false;

			uint32_t created = 0;
			for (uint32_t i = 0; i < numBuffers; ++i)
			{
				ID3D11Buffer* pD3DBuf = nullptr;
				if (FAILED(m_pD3D11Device->CreateBuffer(&desc, nullptr, &pD3DBuf)))
				{
					for (uint32_t j = 0; j < created; ++j)
						pSlots[j].~D3D11Buffer();
					sys_free(pSlots);
					return false;
				}
				new (&pSlots[i]) D3D11Buffer(pD3DBuf, bufBytes, extType, extUsage, TRUE);
				++created;
			}

			AUTO_SPIN_LOCK guard(&pGroup->SpinLock);

			uint32_t pageIndex = (uint32_t)pGroup->BufferPages.size();

			D3D11BufferPage page;
			page.pBufferSlots = pSlots;
			page.NumBuffers = numBuffers;
			page.PageIndex = pageIndex;
			pGroup->BufferPages.push_back(page);

			for (uint32_t i = 0; i < numBuffers; ++i)
			{
				BufferIndex idx;
				idx.PageIndex = (uint16_t)pageIndex;
				idx.SlotIndex = (uint16_t)i;
				pGroup->FreeBuffers.push_back(idx);
			}

			return true;
		}

		void D3D11BufferPool::Shutdown()
		{
			for (int usage = 0; usage < (int)POOL_BUFFER_USAGE::EMAX; ++usage)
			{
				for (int size = 0; size < (int)POOL_BUFFER_SIZE::EMAX; ++size)
				{
					for (auto& page : m_VBPool[usage][size].BufferPages)
					{
						CORE_DELETE_ARRAY(D3D11Buffer, page.pBufferSlots, page.NumBuffers);
					}
					m_VBPool[usage][size].BufferPages.clear();
					m_VBPool[usage][size].FreeBuffers.clear();

					for (auto& page : m_IBPool[usage][size].BufferPages)
					{
						CORE_DELETE_ARRAY(D3D11Buffer, page.pBufferSlots, page.NumBuffers);
					}
					m_IBPool[usage][size].BufferPages.clear();
					m_IBPool[usage][size].FreeBuffers.clear();

					for (auto& page : m_CBPool[usage][size].BufferPages)
					{
						CORE_DELETE_ARRAY(D3D11Buffer, page.pBufferSlots, page.NumBuffers);
					}
					m_CBPool[usage][size].BufferPages.clear();
					m_CBPool[usage][size].FreeBuffers.clear();
				}
			}
		}
	}
}
