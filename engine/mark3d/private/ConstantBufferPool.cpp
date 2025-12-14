#include "pch.h"
#include "ConstantBufferPool.h"
#include "ConstantBuffer.h"
#include "crt_memory.h"


size_t CBUFFER_CAPA_SIZE[11] =
{
	64,     // CB_CAPA_64B
	128,    // CB_SIZE_128B
	256,    // CB_SIZE_256B
	512,    // CB_SIZE_512B
	1024,   // CB_SIZE_1K
	2048,   // CB_SIZE_2K
	4096,   // CB_SIZE_4K
	8192,   // CB_SIZE_8K
	16384,  // CB_SIZE_16K
	32768,  // CB_SIZE_32K
	65536   // CB_SIZE_64K
};



ConstantBufferPool::~ConstantBufferPool()
{

}

__FORCEINLINE int32 GetSizeIndexBySize(size_t size)
{
	for (int i = 0; i < 11; ++i)
	{
		if (size <= CBUFFER_CAPA_SIZE[i])
			return i;
	}

	return -1;
}

void ConstantBufferPool::Init()
{
	for(int i = 0; i < (size_t)CBUFFER_INDEX::EMAX; ++i)
	{
		init_linked_list(&m_CBufferPool[i]);
		AllocPool((CBUFFER_INDEX)i);
	}

}

void* ConstantBufferPool::Alloc(size_t BufferSize)
{
	int32 sizeIndex = GetSizeIndexBySize(BufferSize);
	if(-1 == sizeIndex)
	{
		SYS_LOG_E("ConstantBufferPool::Alloc - Requested buffer size %zu exceeds maximum supported size.", BufferSize);
		return nullptr;
	}

	if (linked_list_empty(&m_CBufferPool[sizeIndex]))
		AllocPool((CBUFFER_INDEX)sizeIndex);

	LINK_NODE* pNode = linked_list_pop_front(&m_CBufferPool[sizeIndex]);
	CBUFFER_POOL* pCBufferPool = (CBUFFER_POOL*)pNode->data;
	return pCBufferPool->pData;

}

void ConstantBufferPool::Release(size_t BufferSize, void* pCBufferData)
{
	int32 sizeIndex = GetSizeIndexBySize(BufferSize);
	if (-1 == sizeIndex)
	{
		SYS_LOG_E("ConstantBufferPool::Release - Released buffer size %zu exceeds maximum supported size.", BufferSize);
		return;
	}

	CBUFFER_POOL* pCBufferPool = MARK_NEW(CBUFFER_POOL);
	pCBufferPool->BufferSize = CBUFFER_CAPA_SIZE[(size_t)sizeIndex];
	pCBufferPool->pData = pCBufferData;
	pCBufferPool->LinkNode.data = pCBufferPool;
	linked_list_push_front(&m_CBufferPool[sizeIndex], &pCBufferPool->LinkNode);

}

void ConstantBufferPool::Clear()
{

}

void ConstantBufferPool::AllocPool(CBUFFER_INDEX Index)
{
	size_t capa = CBUFFER_CAPA_SIZE[(int)Index];

	for(size_t i = 0; i < capa; ++i)
	{
		CBUFFER_POOL* pCBufferPool = MARK_NEW(CBUFFER_POOL);
		pCBufferPool->BufferSize = CBUFFER_CAPA_SIZE[(int)Index];
		pCBufferPool->pData = crt_malloc_align(pCBufferPool->BufferSize, 16);
		pCBufferPool->LinkNode.data = pCBufferPool;
		linked_list_push_front(&m_CBufferPool[(size_t)Index], &pCBufferPool->LinkNode);
	}

}
