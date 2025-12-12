#include "pch.h"
#include "D3D11RenderDef.h"
#include "D3D11ConstantBufferPool.h"
#include "D3D11RenderResources.h"
#include "D3D11RenderDevice.h"


static constexpr size_t POOL_BLOCK_SIZE[POOL_BLOCK_TYPE_COUNT] =
{
	64,     // 64 bytes
	128,    // 128 bytes
	256,    // 256 bytes
	512,    // 512 bytes
	1024,   // 1 KB
	2048,   // 2 KB
	4096,   // 4 KB
	8192    // 8 KB
};


static constexpr size_t POOL_BLOCK_COUNT[POOL_BLOCK_TYPE_COUNT] =
{
	32, 
	32,
	32,
	32,
	16,
	16,
	16,
	16,
};

__FORCEINLINE size_t GetPoolBlockTypeIndex(size_t SizeInBytes)
{
	for (size_t i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
	{
		if (POOL_BLOCK_SIZE[i] == SizeInBytes)
			return i;
	}
	return SIZE_MAX;
}

D3D11ConstantBufferPool::~D3D11ConstantBufferPool() noexcept
{
	Shutdown();
}

BOOL D3D11ConstantBufferPool::Init(D3D11RenderDevice* pRenderDevice)
{
	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
		init_linked_list(&m_FreeList[i]);

	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
		init_linked_list(&m_UsedList[i]);

	D3D11_BUFFER_DESC Desc = {};

	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
	{
		if (!CreateD3D11ConstantBuffer(
			pRenderDevice->INL_GetD3D11Device(), 
			&m_FreeList[i], 
			POOL_BLOCK_SIZE[i], 
			POOL_BLOCK_COUNT[i]
		))
		{
			SYS_LOG_E("D3D11ConstantBufferPool::Init: Failed to create constant buffers %u", POOL_BLOCK_SIZE[i]);
			return FALSE;
		}
	}

	m_pRenderDevice = pRenderDevice;

	return TRUE;
}

void D3D11ConstantBufferPool::Shutdown() noexcept
{
	// Release all constant buffers
	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
	{
		// Free used list
		while (!linked_list_empty(&m_UsedList[i]))
		{
			LINK_NODE* pNode = linked_list_pop_front(&m_UsedList[i]);
			D3D11ConstantBuffer* pConstantBuffer = (D3D11ConstantBuffer*)pNode->data;
			if (pConstantBuffer)
			{
				CHECK_RELEASE(pConstantBuffer->pConstantBuffer);
				MARK_POOL_FREE(pConstantBuffer);
			}
		}

		// Free free list
		while (!linked_list_empty(&m_FreeList[i]))
		{
			LINK_NODE* pNode = linked_list_pop_front(&m_FreeList[i]);
			D3D11ConstantBuffer* pConstantBuffer = (D3D11ConstantBuffer*)pNode->data;
			if (pConstantBuffer)
			{
				CHECK_RELEASE(pConstantBuffer->pConstantBuffer);
				MARK_POOL_FREE(pConstantBuffer);
			}
		}
	}
}

D3D11ConstantBuffer* D3D11ConstantBufferPool::Allocate(size_t AllocSize)
{
	size_t TypeIndex = GetPoolBlockTypeIndex(AllocSize);
	if (SIZE_MAX == TypeIndex)
	{
		SYS_LOG_E("D3D11ConstantBufferPool::Allocate: Invalid allocation size %u", AllocSize);
		return nullptr;
	}
		
	if (linked_list_empty(&m_FreeList[TypeIndex]))
	{
		// 풀에 여유가 없으면 새로 생성
		if (!CreateD3D11ConstantBuffer(
			m_pRenderDevice->INL_GetD3D11Device(),
			&m_FreeList[TypeIndex],
			POOL_BLOCK_SIZE[TypeIndex],
			POOL_BLOCK_COUNT[TypeIndex]
		))
		{
			SYS_LOG_E("D3D11ConstantBufferPool::Allocate: Failed to create additional constant buffers %u", POOL_BLOCK_SIZE[TypeIndex]);
			return nullptr;
		}
	}

	// 프리 리스트에서 하나 꺼내기
	LINK_NODE* pNode = linked_list_pop_front(&m_FreeList[TypeIndex]);
	D3D11ConstantBuffer* pConstantBuffer = (D3D11ConstantBuffer*)pNode->data;

	// 유즈 리스트에 추가
	linked_list_push_back(&m_UsedList[TypeIndex], pConstantBuffer->INL_GetLinkNode());

	return pConstantBuffer;
}

void D3D11ConstantBufferPool::Release(D3D11ConstantBuffer* pConstantBuffer)
{
	if (!pConstantBuffer)
		return;

	size_t TypeIndex = GetPoolBlockTypeIndex(pConstantBuffer->SizeInBytes);
	if (SIZE_MAX == TypeIndex)
	{
		SYS_LOG_E("D3D11ConstantBufferPool::Release: Invalid constant buffer size %u", pConstantBuffer->SizeInBytes);
		return;
	}

	// 유즈 리스트에서 제거
	linked_list_remove_node(&m_UsedList[TypeIndex], pConstantBuffer->INL_GetLinkNode());

	// 프리 리스트에 추가
	linked_list_push_front(&m_FreeList[TypeIndex], pConstantBuffer->INL_GetLinkNode());
}

BOOL D3D11ConstantBufferPool::CreateD3D11ConstantBuffer(ID3D11Device* pDevice, LINKED_LIST* pStoreList, size_t SizeInBytes, size_t Count)
{
	D3D11_BUFFER_DESC Desc = {};

	Desc.ByteWidth = (UINT)SizeInBytes;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	for (size_t i = 0; i < Count; ++i)
	{
		ID3D11Buffer* pD3D11Buffer = nullptr;
		HRESULT hr = pDevice->CreateBuffer(
			&Desc,
			nullptr,
			&pD3D11Buffer
		);

		if (FAILED(hr))
			return FALSE;

		D3D11ConstantBuffer* pConstantBuffer = MARK_POOL_NEW(D3D11ConstantBuffer)();
		pConstantBuffer->pConstantBuffer = pD3D11Buffer;
		pConstantBuffer->SizeInBytes = Desc.ByteWidth;
		pConstantBuffer->INL_GetLinkNode()->data = (void*)pConstantBuffer;

		linked_list_push_back(pStoreList, pConstantBuffer->INL_GetLinkNode());
	}

	return TRUE;
}

