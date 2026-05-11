#include "pch.h"
#include "D3D11BufferPool.h"


namespace mark
{
	inline static POOL_BUFFER_USAGE MapBufferUsage(BUFFER_USAGE usage)
	{
		switch (usage)
		{
		case BUFFER_USAGE::DYNAMIC:
			return POOL_BUFFER_USAGE::DYNAMIC;
		case BUFFER_USAGE::DEFAULT:
			return POOL_BUFFER_USAGE::DEFAULT;
		}

		return POOL_BUFFER_USAGE::EMAX;
	}

	inline static POOL_BUFFER_USAGE MapBufferD3D11Usage(D3D11_USAGE usage)
	{
		switch (usage)
		{
		case D3D11_USAGE_DYNAMIC:
			return POOL_BUFFER_USAGE::DYNAMIC;
		case D3D11_USAGE_DEFAULT:
			return POOL_BUFFER_USAGE::DEFAULT;
		}

		return POOL_BUFFER_USAGE::EMAX;
	}

	inline static POOL_BUFFER_SIZE MapBufferSize(size_t size)
	{
		for (int i = 0; i < (int)POOL_BUFFER_SIZE::EMAX; ++i)
		{
			if (size <= POOL_BUFFER_SIZES[i])
				return (POOL_BUFFER_SIZE)i;
		}
		return POOL_BUFFER_SIZE::EMAX;
	}

	D3D11BufferPool::D3D11BufferPool(ID3D11Device* pDevice)
		: m_pD3D11Device(pDevice)
	{
	}

	D3D11BufferPool::~D3D11BufferPool() noexcept
	{
		Shutdown();
	}

	ID3D11Buffer* D3D11BufferPool::Acquire(
		BUFFER_TYPE BufferType,
		BUFFER_USAGE BufferUsage,
		size_t BufferSize
	)
	{
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
			return nullptr;

		}

		POOL_BUFFER_USAGE PoolUsage = MapBufferUsage(BufferUsage);
		POOL_BUFFER_SIZE  PoolSize = MapBufferSize(BufferSize);

		if (PoolSize == POOL_BUFFER_SIZE::EMAX)
			return nullptr;

		int UsageIndex = (int)PoolUsage;
		int SizeIndex = (int)PoolSize;

		BufferPageGroup* pGroup = nullptr;

		switch (PoolType)
		{
		case POOL_BUFFER_TYPE::VERTEX_BUFFER:
			pGroup = &m_VBPool[UsageIndex][SizeIndex];
			break;

		case POOL_BUFFER_TYPE::INDEX_BUFFER:
			pGroup = &m_IBPool[UsageIndex][SizeIndex]; break;
			break;

		case POOL_BUFFER_TYPE::CONSTANT_BUFFER:
			pGroup = &m_CBPool[UsageIndex][SizeIndex];
			break;

		default:
			return nullptr;
		}

		bool IsCreateBufferPage = false;
	GET_BUFFER:
		{
			AUTO_SPIN_LOCK guard(&pGroup->SpinLock);
			if (!pGroup->FreeBuffers.empty())
			{
				ID3D11Buffer* pBuffer = pGroup->FreeBuffers.front();
				pGroup->FreeBuffers.pop_front();
				return pBuffer;
			}
		}

		if (!IsCreateBufferPage && CreateBufferPage(PoolType, PoolUsage, PoolSize))
		{
			IsCreateBufferPage = true;
			goto GET_BUFFER;
		}

		return nullptr;
	}

	bool D3D11BufferPool::Release(ID3D11Buffer* pBuffer)
	{
		if (!pBuffer) [[unlikely]]
			return false;

		D3D11_BUFFER_DESC Desc = {};
		pBuffer->GetDesc(&Desc);

		POOL_BUFFER_TYPE PoolType;
		switch (Desc.BindFlags)
		{
		case D3D11_BIND_VERTEX_BUFFER:
			PoolType = POOL_BUFFER_TYPE::VERTEX_BUFFER;
			break;

		case D3D11_BIND_INDEX_BUFFER:
			PoolType = POOL_BUFFER_TYPE::INDEX_BUFFER;
			break;

		case D3D11_BIND_CONSTANT_BUFFER:
			PoolType = POOL_BUFFER_TYPE::CONSTANT_BUFFER;
			break;

		default:
			LOG_ERR("D3D11HardwareBufferPool::Release - Invalid buffer bind flags");
			return false;
		}

		POOL_BUFFER_USAGE PoolUsage = MapBufferD3D11Usage(Desc.Usage);
		POOL_BUFFER_SIZE  PoolSize = MapBufferSize(Desc.ByteWidth);

		if (PoolSize == POOL_BUFFER_SIZE::EMAX)
			return false;

		int UsageIndex = (int)PoolUsage;
		int SizeIndex = (int)PoolSize;

		BufferPageGroup* pGroup = nullptr;
		switch (PoolType)
		{
		case POOL_BUFFER_TYPE::VERTEX_BUFFER:
			pGroup = &m_VBPool[UsageIndex][SizeIndex];
			break;

		case POOL_BUFFER_TYPE::INDEX_BUFFER:
			pGroup = &m_IBPool[UsageIndex][SizeIndex];
			break;

		case POOL_BUFFER_TYPE::CONSTANT_BUFFER:
			pGroup = &m_CBPool[UsageIndex][SizeIndex];
			break;

		default:
			return false;
		}

		AUTO_SPIN_LOCK guard(&pGroup->SpinLock);
		pGroup->FreeBuffers.push_front(pBuffer);

		return true;
	}

	bool D3D11BufferPool::CreateBufferPage(
		POOL_BUFFER_TYPE BufferType,
		POOL_BUFFER_USAGE BufferUsage,
		POOL_BUFFER_SIZE BufferSize
	)
	{
		if (!m_pD3D11Device)
			return false;

		int UsageIndex = (int)BufferUsage;
		int SizeIndex = (int)BufferSize;

		BufferPageGroup* pGroup = nullptr;
		switch (BufferType)
		{
		case POOL_BUFFER_TYPE::VERTEX_BUFFER:
			pGroup = &m_VBPool[UsageIndex][SizeIndex];
			break;

		case POOL_BUFFER_TYPE::INDEX_BUFFER:
			pGroup = &m_IBPool[UsageIndex][SizeIndex];
			break;

		case POOL_BUFFER_TYPE::CONSTANT_BUFFER:
			pGroup = &m_CBPool[UsageIndex][SizeIndex];
			break;

		default:
			return false;
		}

		uint32_t NumBuffers = (uint32_t)BUFFER_COUNT_PER_PAGE[SizeIndex];
		size_t BufferBytes = POOL_BUFFER_SIZES[SizeIndex];

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)BufferBytes;
		desc.StructureByteStride = 0;
		desc.MiscFlags = 0;

		switch (BufferType)
		{
		case POOL_BUFFER_TYPE::VERTEX_BUFFER:
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			break;

		case POOL_BUFFER_TYPE::INDEX_BUFFER:
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;

		case POOL_BUFFER_TYPE::CONSTANT_BUFFER:
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			break;

		default:
			return false;
		}

		switch (BufferUsage)
		{
			case POOL_BUFFER_USAGE::DYNAMIC:
				desc.Usage = D3D11_USAGE_DYNAMIC;
				break;

			case POOL_BUFFER_USAGE::DEFAULT:
				desc.Usage = D3D11_USAGE_DEFAULT;
				break;

			default:
				return false;
		}

		uint32_t NumCreated = 0;

		sys_vector<ID3D11Buffer*> NewBuffers;
		NewBuffers.reserve(NumBuffers);

		for (uint32_t i = 0; i < NumBuffers; ++i)
		{
			ID3D11Buffer* pD3DBuf = nullptr;
			if (FAILED(m_pD3D11Device->CreateBuffer(&desc, nullptr, &pD3DBuf)))
			{
				for (uint32_t j = 0; j < NumCreated; ++j)
					NewBuffers[j]->Release();
				return false;
			}

			NewBuffers.push_back(pD3DBuf);
			NumCreated++;
		}

		{
			AUTO_SPIN_LOCK AutoLock(&pGroup->SpinLock);
			pGroup->OriginalBuffers.insert(pGroup->OriginalBuffers.end(), NewBuffers.begin(), NewBuffers.end());
			pGroup->FreeBuffers.insert(pGroup->FreeBuffers.end(), NewBuffers.begin(), NewBuffers.end());
		}

		return true;
	}

	void D3D11BufferPool::Shutdown()
	{
		for (int usage = 0; usage < (int)POOL_BUFFER_USAGE::EMAX; ++usage)
		{
			for (int size = 0; size < (int)POOL_BUFFER_SIZE::EMAX; ++size)
			{
				for (auto pBuffer : m_VBPool[usage][size].OriginalBuffers)
				{
					pBuffer->Release();
				}
				m_VBPool[usage][size].OriginalBuffers.clear();
				m_VBPool[usage][size].FreeBuffers.clear();

				for (auto pBuffer : m_IBPool[usage][size].OriginalBuffers)
				{
					pBuffer->Release();
				}
				m_IBPool[usage][size].OriginalBuffers.clear();
				m_IBPool[usage][size].FreeBuffers.clear();

				for (auto pBuffer : m_CBPool[usage][size].OriginalBuffers)
				{
					pBuffer->Release();
				}
				m_CBPool[usage][size].OriginalBuffers.clear();
				m_CBPool[usage][size].FreeBuffers.clear();
			}
		}
	}
}
