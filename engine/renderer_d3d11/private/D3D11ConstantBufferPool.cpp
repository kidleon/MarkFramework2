#include "pch.h"
#include "D3D11RenderDef.h"
#include "D3D11ConstantBufferPool.h"
#include "D3D11ConstantBuffer.h"
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
	8192,   // 8 KB
	16384,	// 16 KB
	32768,	// 32 KB
	65536,	// 64 KB
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
	8,
	8,
	8
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

D3D11ConstantBufferPool::D3D11ConstantBufferPool(D3D11RenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice)
{
}

D3D11ConstantBufferPool::~D3D11ConstantBufferPool() noexcept
{
	Shutdown();
}

BOOL D3D11ConstantBufferPool::Init()
{
	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
		init_linked_list(&m_FreeList[i]);

	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
		init_linked_list(&m_UsedList[i]);

	D3D11_BUFFER_DESC Desc = {};

	ID3D11Device* pD3D11Device = m_pRenderDevice->INL_GetD3D11Device();

	for (int i = 0; i < POOL_BLOCK_TYPE_COUNT; ++i)
	{
		if (!CreateD3D11ConstantBuffer(
			&m_FreeList[i], 
			POOL_BLOCK_SIZE[i], 
			POOL_BLOCK_COUNT[i]
		))
		{
			SYS_LOG_E("D3D11ConstantBufferPool::Init: Failed to create constant buffers %u", POOL_BLOCK_SIZE[i]);
			return FALSE;
		}
	}

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
				D3D11_POOL_DELETE(pConstantBuffer, D3D11ConstantBuffer);
			}
		}

		// Free free list
		while (!linked_list_empty(&m_FreeList[i]))
		{
			LINK_NODE* pNode = linked_list_pop_front(&m_FreeList[i]);
			D3D11ConstantBuffer* pConstantBuffer = (D3D11ConstantBuffer*)pNode->data;
			if (pConstantBuffer)
			{
				D3D11_POOL_DELETE(pConstantBuffer, D3D11ConstantBuffer);
			}
		}
	}

	m_pRenderDevice = nullptr;
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

	size_t TypeIndex = GetPoolBlockTypeIndex(pConstantBuffer->INL_GetBufferSize());
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

BOOL D3D11ConstantBufferPool::CreateD3D11ConstantBuffer(LINKED_LIST* pStoreList, size_t SizeInBytes, size_t Count)
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

		if (!m_pRenderDevice->CreateBuffer(&Desc, &pD3D11Buffer))
			continue;

		D3D11ConstantBuffer* pConstantBuffer = D3D11_POOL_NEW(D3D11ConstantBuffer)(SizeInBytes, pD3D11Buffer);
		linked_list_push_back(pStoreList, pConstantBuffer->INL_GetLinkNode());
	}

	return TRUE;
}
