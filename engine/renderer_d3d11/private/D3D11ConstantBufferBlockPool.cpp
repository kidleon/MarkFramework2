#include "pch.h"
#include "D3D11ConstantBufferBlockPool.h"
#include "D3D11ConstantBufferBlock.h"


LINKED_LIST D3D11ConstantBufferBlockPool::m_CBufferBlockPool[(size_t)CBUFFER_INDEX::EMAX] = {};


static constexpr size_t CBUFFER_CAPA_SIZE[] =
{
	64,     // CB_64B
	128,    // CB_128B
	256,    // CB_256B
	512,    // CB_512B
	1024,   // CB_1K
	2048,   // CB_2K
	4096,   // CB_4K
	8192,   // CB_8K
	16384,  // CB_16K
	32768,  // CB_32K
	65536   // CB_64K
};

static constexpr size_t CBUFFER_ALLOC_COUNT[] =
{
	32, // CB_64B
	32, // CB_128B
	32, // CB_256B
	16, // CB_512B
	16, // CB_1K
	16, // CB_2K
	16, // CB_4K
	16, // CB_8K
	8, // CB_16K
	8, // CB_32K
	8 // CB_64
};

static int32 GetSizeIndexBySize(size_t size)
{
	for (int i = 0; i < 11; ++i)
	{
		if (size <= CBUFFER_CAPA_SIZE[i])
			return i;
	}

	return -1;
}

void D3D11ConstantBufferBlockPool::Init()
{
	for(int i = 0; i < (size_t)CBUFFER_INDEX::EMAX; ++i)
	{
		init_linked_list(&m_CBufferBlockPool[i]);
		AllocPool((CBUFFER_INDEX)i);
	}

}

D3D11_CONSTANT_BUFFER_BLOCK* D3D11ConstantBufferBlockPool::Alloc(size_t BufferSize)
{
	const int32 SizeIndex = GetSizeIndexBySize(BufferSize);
	if (-1 == SizeIndex)
	{
		SYS_LOG_E("ConstantBufferBlockPool::Alloc - Requested buffer size %zu exceeds maximum supported size.", BufferSize);
		return nullptr;
	}

	if (linked_list_empty(&m_CBufferBlockPool[SizeIndex]))
		AllocPool((CBUFFER_INDEX)SizeIndex);

	LINK_NODE* pNode = linked_list_pop_front(&m_CBufferBlockPool[SizeIndex]);
	D3D11_CONSTANT_BUFFER_BLOCK* pBlock = (D3D11_CONSTANT_BUFFER_BLOCK*)pNode->data;
	
	return pBlock;
}

void D3D11ConstantBufferBlockPool::Release(D3D11_CONSTANT_BUFFER_BLOCK* pCBBlock)
{
	if (!pCBBlock)
		return;

#if defined(_DEBUG) || defined(DEBUG)
	int32 sizeIndex = GetSizeIndexBySize(pCBBlock->BufferSize);
	if (-1 == sizeIndex)
	{
		SYS_LOG_E("ConstantBufferBlockPool::Release - Released buffer size %zu exceeds maximum supported size.", BufferSize);
		return;
	}
#endif // defeind(_DEBUG) || defined(DEBUG)

	LINKED_LIST* pLinkedList = &m_CBufferBlockPool[pCBBlock->BufferSizeIndex];
	linked_list_push_front(pLinkedList, &pCBBlock->LinkNode);
}

void D3D11ConstantBufferBlockPool::Shutdown()
{
	for (int i = 0; i < (size_t)CBUFFER_INDEX::EMAX; ++i)
	{
		LINKED_LIST* pLinkedList = &m_CBufferBlockPool[i];
		while (!linked_list_empty(pLinkedList))
		{
			LINK_NODE* pNode = linked_list_pop_front(pLinkedList);
			D3D11_CONSTANT_BUFFER_BLOCK* pCBBlock = (D3D11_CONSTANT_BUFFER_BLOCK*)pNode->data;
			if (pCBBlock)
			{
				if (pCBBlock->pData)
				{
					crt_free_align(pCBBlock->pData);
					pCBBlock->pData = nullptr;
				}

				D3D11_DELETE(pCBBlock, D3D11_CONSTANT_BUFFER_BLOCK);
			}
		}
	}
}

void D3D11ConstantBufferBlockPool::AllocPool(CBUFFER_INDEX Index)
{
	const size_t AllocCount = CBUFFER_ALLOC_COUNT[(int)Index];
	const size_t AllocSize = CBUFFER_CAPA_SIZE[(int)Index];

	LINKED_LIST* pLinkedList = &m_CBufferBlockPool[(size_t)Index];

	for(size_t i = 0; i < AllocCount; ++i)
	{
		D3D11_CONSTANT_BUFFER_BLOCK* pCBBlock = D3D11_NEW(D3D11_CONSTANT_BUFFER_BLOCK);
		pCBBlock->BufferSize = AllocSize;
		pCBBlock->BufferSizeIndex = (size_t)Index;
		pCBBlock->pData = crt_malloc_align(AllocSize, 16);
		linked_list_push_front(pLinkedList, &pCBBlock->LinkNode);
	}
}
