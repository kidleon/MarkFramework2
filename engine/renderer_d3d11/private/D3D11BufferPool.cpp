#include "pch.h"
#include "D3D11BufferPool.h"
#include "D3D11Buffer.h"


constexpr size_t BUFFER_SIZES[D3D11BufferPool::BUFFER_SIZE::B_SIZE_COUNT] = {
	1024,       // B_1K
	2048,       // B_2K
	4096,       // B_4K
	8192,       // B_8K
	16384,      // B_16K
	65536,      // B_64K
	131072,     // B_128K
	262144,     // B_256K
	524288,     // B_512K
	1048576,    // B_1M
	2097152     // B_2M
};

inline int32 GetBufferIndex(size_t BufferSize)
{
	for (int32 i = 0; i < D3D11BufferPool::BUFFER_SIZE::B_SIZE_COUNT; ++i)
	{
		if (BufferSize <= BUFFER_SIZES[i])
			return i;
	}

	return -1; // 지원하지 않는 크기
}

D3D11BufferPool* D3D11BufferPool::s_pInstance = nullptr;


D3D11BufferPool::D3D11BufferPool(ID3D11Device* pD3D11Device)
	: m_pD3D11Device(pD3D11Device)
{
	if (!D3D11BufferPool::s_pInstance)
		D3D11BufferPool::s_pInstance = this;

	if (m_pD3D11Device)
		m_pD3D11Device->AddRef();

	for (size_t bu = 0; bu < BU_COUNT; ++bu)
	{
		for (int32 i = 0; i < BUFFER_SIZE::B_SIZE_COUNT; ++i)
		{
			init_linked_list(&m_VBPool[bu][i]);
			init_linked_list(&m_IBPool[bu][i]);
		}
	}
}

D3D11BufferPool::~D3D11BufferPool()
{
	for (size_t bu = 0; bu < BU_COUNT; ++bu)
	{
		for (int32 i = 0; i < BUFFER_SIZE::B_SIZE_COUNT; ++i)
		{
			while (!linked_list_empty(&m_VBPool[bu][i]))
			{
				LINK_NODE* pNode = linked_list_pop_front(&m_VBPool[bu][i]);
				D3D11Buffer* pBuffer = static_cast<D3D11Buffer*>(pNode->data);
				D3D11_DELETE(pBuffer, D3D11Buffer);
				m_AllocatedVBCount[bu][i]--;
			}

			while (!linked_list_empty(&m_IBPool[bu][i]))
			{
				LINK_NODE* pNode = linked_list_pop_front(&m_IBPool[bu][i]);
				D3D11Buffer* pBuffer = static_cast<D3D11Buffer*>(pNode->data);
				D3D11_DELETE(pBuffer, D3D11Buffer);
				m_AllocatedIBCount[bu][i]--;
			}

			if (m_AllocatedVBCount[bu][i] > 0)
			{
				SYS_LOG_W("D3D11BufferPool::~D3D11BufferPool: Warning - %zu vertex buffers of size %zu bytes were not released properly.", m_AllocatedVBCount[bu][i], BUFFER_SIZES[i]);
			}

			if (m_AllocatedIBCount[bu][i] > 0)
			{
				SYS_LOG_W("D3D11BufferPool::~D3D11BufferPool: Warning - %zu index buffers of size %zu bytes were not released properly.", m_AllocatedIBCount[bu][i], BUFFER_SIZES[i]);
			}
		}
	}

	CHECK_RELEASE(m_pD3D11Device);
}

D3D11Buffer* D3D11BufferPool::AcquireVB(size_t BufferSize, BUFFER_USAGE Usage)
{
	int32 BufferIndex = GetBufferIndex(BufferSize);
	if (BufferIndex == -1)
	{
		SYS_LOG_E("D3D11BufferPool::AcquireVB: Invalid buffer size %zu bytes requested for vertex buffer.", BufferSize);
		return nullptr;
	}

	int32 UsageIndex = (Usage == BUFFER_USAGE::DYNAMIC || Usage == BUFFER_USAGE::STAGING) ? 1 : 0;

	if (linked_list_empty(&m_VBPool[UsageIndex][BufferIndex]))
	{
		// 풀 확장
		size_t NumBuffersToCreate = 10; // 예시로 10개씩 생성
		ExpandVB(BUFFER_SIZES[BufferIndex], NumBuffersToCreate, Usage);

		if (linked_list_empty(&m_VBPool[UsageIndex][BufferIndex]))
		{
			SYS_LOG_E("D3D11BufferPool::AcquireVB: Failed to expand vertex buffer pool for size %zu bytes.", BUFFER_SIZES[BufferIndex]);
			return nullptr;
		}
	}

	LINK_NODE* pNode = linked_list_pop_front(&m_VBPool[UsageIndex][BufferIndex]);
	if (!pNode)
	{
		SYS_LOG_E("D3D11BufferPool::AcquireVB: Failed to acquire vertex buffer from pool for size %zu bytes.", BUFFER_SIZES[BufferIndex]);
		return nullptr;
	}

	return static_cast<D3D11Buffer*>(pNode->data);
}

D3D11Buffer* D3D11BufferPool::AcquireIB(size_t BufferSize, BUFFER_USAGE Usage)
{
	int32 BufferIndex = GetBufferIndex(BufferSize);
	if (BufferIndex == -1)
	{
		SYS_LOG_E("D3D11BufferPool::AcquireIB: Invalid buffer size %zu bytes requested for index buffer.", BufferSize);
		return nullptr;
	}

	int32 UsageIndex = (Usage == BUFFER_USAGE::DYNAMIC || Usage == BUFFER_USAGE::STAGING) ? 1 : 0;

	if (linked_list_empty(&m_IBPool[UsageIndex][BufferIndex]))
	{
		// 풀 확장
		size_t NumBuffersToCreate = 10; // 예시로 10개씩 생성
		ExpandIB(BUFFER_SIZES[BufferIndex], NumBuffersToCreate, Usage);
		if (linked_list_empty(&m_IBPool[UsageIndex][BufferIndex]))
		{
			SYS_LOG_E("D3D11BufferPool::AcquireIB: Failed to expand index buffer pool for size %zu bytes.", BUFFER_SIZES[BufferIndex]);
			return nullptr;
		}
	}

	LINK_NODE* pNode = linked_list_pop_front(&m_IBPool[UsageIndex][BufferIndex]);
	if (!pNode)
	{
		SYS_LOG_E("D3D11BufferPool::AcquireIB: Failed to acquire index buffer from pool for size %zu bytes.", BUFFER_SIZES[BufferIndex]);
		return nullptr;
	}

	return static_cast<D3D11Buffer*>(pNode->data);
}

void D3D11BufferPool::ReleaseVB(D3D11Buffer* pBuffer)
{
	if (!pBuffer)
		return;

	int32 BufferIndex = GetBufferIndex(pBuffer->INL_GetBufferSize());
	if (-1 == BufferIndex)
	{
		SYS_LOG_E("D3D11BufferPool::ReleaseVB: Invalid vertex buffer size for release.");
		return;
	}

	BUFFER_USAGE Usage = pBuffer->INL_GetUsage();
	int32 UsageIndex = (Usage == BUFFER_USAGE::DYNAMIC || Usage == BUFFER_USAGE::STAGING) ? 1 : 0;

	linked_list_push_front(&m_VBPool[UsageIndex][BufferIndex], pBuffer->INL_GetPoolLinkNode());
}

void D3D11BufferPool::ReleaseIB(D3D11Buffer* pBuffer)
{
	if (!pBuffer)
		return;

	int32 BufferIndex = GetBufferIndex(pBuffer->INL_GetBufferSize());
	if (-1 == BufferIndex)
	{
		SYS_LOG_E("D3D11BufferPool::ReleaseIB: Invalid index buffer size for release.");
		return;
	}

	BUFFER_USAGE Usage = pBuffer->INL_GetUsage();
	int32 UsageIndex = (Usage == BUFFER_USAGE::DYNAMIC || Usage == BUFFER_USAGE::STAGING) ? 1 : 0;

	linked_list_push_front(&m_IBPool[UsageIndex][BufferIndex], pBuffer->INL_GetPoolLinkNode());
}

void D3D11BufferPool::ExpandVB(size_t BufferSize, size_t NumBuffers, BUFFER_USAGE Usage)
{
	int32 BufferIndex = GetBufferIndex(BufferSize);
	if (-1 == BufferIndex)
	{
		SYS_LOG_E("D3D11BufferPool::ExpandVB: Invalid buffer size %zu bytes for vertex buffer expansion.", BufferSize);
		return;
	}

	int32 UsageIndex = (Usage == BUFFER_USAGE::DYNAMIC || Usage == BUFFER_USAGE::STAGING) ? 1 : 0;

	for (size_t i = 0; i < NumBuffers; ++i)
	{
		ID3D11Buffer* pD3D11Buffer = nullptr;
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = static_cast<UINT>(BufferSize);
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		HRESULT hr = m_pD3D11Device->CreateBuffer(&BufferDesc, nullptr, &pD3D11Buffer);
		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11BufferPool::ExpandVB: Failed to create vertex buffer of size %zu bytes.", BufferSize);
			continue;
		}
		
		D3D11Buffer* pBuffer = new D3D11Buffer(pD3D11Buffer, (UINT32)BufferSize, Usage);
		if (!pBuffer)
		{
			SYS_LOG_E("D3D11BufferPool::ExpandVB: Failed to allocate D3D11Buffer wrapper for vertex buffer.");
			pD3D11Buffer->Release();
			continue;
		}

		m_AllocatedVBCount[UsageIndex][BufferIndex]++;

		linked_list_push_front(&m_VBPool[UsageIndex][BufferIndex], pBuffer->INL_GetPoolLinkNode());
	}
}

void D3D11BufferPool::ExpandIB(size_t BufferSize, size_t NumBuffers, BUFFER_USAGE Usage)
{
	int32 BufferIndex = GetBufferIndex(BufferSize);
	if (-1 == BufferIndex)
	{
		SYS_LOG_E("D3D11BufferPool::ExpandIB: Invalid buffer size %zu bytes for index buffer expansion.", BufferSize);
		return;
	}

	int32 UsageIndex = (Usage == BUFFER_USAGE::DYNAMIC || Usage == BUFFER_USAGE::STAGING) ? 1 : 0;

	for (size_t i = 0; i < NumBuffers; ++i)
	{
		ID3D11Buffer* pD3D11Buffer = nullptr;
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = static_cast<UINT>(BufferSize);
		BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		HRESULT hr = m_pD3D11Device->CreateBuffer(&BufferDesc, nullptr, &pD3D11Buffer);
		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11BufferPool::ExpandIB: Failed to create index buffer of size %zu bytes.", BufferSize);
			continue;
		}
		
		D3D11Buffer* pBuffer = new D3D11Buffer(pD3D11Buffer, (UINT32)BufferSize, Usage);
		if (!pBuffer)
		{
			SYS_LOG_E("D3D11BufferPool::ExpandIB: Failed to allocate D3D11Buffer wrapper for index buffer.");
			pD3D11Buffer->Release();
			continue;
		}

		m_AllocatedIBCount[UsageIndex][BufferIndex]++;

		linked_list_push_front(&m_IBPool[UsageIndex][BufferIndex], pBuffer->INL_GetPoolLinkNode());
	}
}


