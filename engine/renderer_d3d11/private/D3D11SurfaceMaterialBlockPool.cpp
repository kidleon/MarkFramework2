#include "pch.h"
#include "D3D11SurfaceMaterialBlockPool.h"
#include "D3D11SurfaceMaterialBlock.h"
#include "D3D11ShaderProgram.h"


D3D11SurfaceMaterialBlockPool* D3D11SurfaceMaterialBlockPool::s_pInstance = nullptr;

D3D11SurfaceMaterialBlockPool::D3D11SurfaceMaterialBlockPool()
{
	if (!s_pInstance)
	{
		s_pInstance = this;
	}
}

D3D11SurfaceMaterialBlockPool::~D3D11SurfaceMaterialBlockPool() noexcept
{
	Cleanup();
	s_pInstance = nullptr;
}

BOOL D3D11SurfaceMaterialBlockPool::Init(size_t InitialCapacity)
{
	ExpandPool(InitialCapacity);
	return TRUE;
}

void D3D11SurfaceMaterialBlockPool::Cleanup()
{
	LINK_NODE* pNode = m_FreeList.head;

	while (pNode)
	{
		LINK_NODE* pNextNode = pNode->next;
		D3D11_SURFACE_MATERIAL_BLOCK* pBlock = (D3D11_SURFACE_MATERIAL_BLOCK*)pNode->data;
		D3D11_POOL_FREE(pBlock);
		pNode = pNextNode;
	}

	m_FreeList.head = nullptr;
	m_FreeList.tail = nullptr;

	pNode = m_UsedList.head;

	while (pNode)
	{
		LINK_NODE* pNextNode = pNode->next;
		D3D11_SURFACE_MATERIAL_BLOCK* pBlock = (D3D11_SURFACE_MATERIAL_BLOCK*)pNode->data;
		D3D11_POOL_FREE(pBlock);
		pNode = pNextNode;
	}

	m_UsedList.head = nullptr;
	m_UsedList.tail = nullptr;
}

D3D11_SURFACE_MATERIAL_BLOCK* D3D11SurfaceMaterialBlockPool::Acquire()
{
	if (m_FreeList.size == 0)
		ExpandPool(16);

	LINK_NODE* pNode = m_FreeList.head;
	if (!pNode)
		return nullptr;
	
	// Remove from free list
	if (pNode->next)
	{
		m_FreeList.head = pNode->next;
		m_FreeList.head->prev = nullptr;
	}
	else
	{
		m_FreeList.head = nullptr;
		m_FreeList.tail = nullptr;
	}

	m_FreeList.size--;

	// Add to used list
	pNode->next = nullptr;
	pNode->prev = m_UsedList.tail;

	if (m_UsedList.tail)
		m_UsedList.tail->next = pNode;
	else
		m_UsedList.head = pNode;
	m_UsedList.tail = pNode;
	m_UsedList.size++;

	return (D3D11_SURFACE_MATERIAL_BLOCK*)pNode->data;
}

void D3D11SurfaceMaterialBlockPool::Release(D3D11_SURFACE_MATERIAL_BLOCK* pBlock)
{
	if (!pBlock) return;

	for(int32 p = 0; p < (int32)pBlock->NumPasses; ++p)
	{
		CHECK_RELEASE(pBlock->RenderPasses[p].pVertexShader);
		CHECK_RELEASE(pBlock->RenderPasses[p].pPixelShader);
	}

	// Remove from used list
	linked_list_remove_node(&m_UsedList, &pBlock->LinkNode);

	// Add to free list
	linked_list_push_back(&m_FreeList, &pBlock->LinkNode);
}

void D3D11SurfaceMaterialBlockPool::ExpandPool(size_t Count)
{
	for (size_t i = 0; i < Count; ++i)
	{
		D3D11_SURFACE_MATERIAL_BLOCK* pBlock = (D3D11_SURFACE_MATERIAL_BLOCK*)D3D11_POOL_ALLOC(sizeof(D3D11_SURFACE_MATERIAL_BLOCK));
		memset(pBlock, 0, sizeof(D3D11_SURFACE_MATERIAL_BLOCK));

		pBlock->LinkNode.data = pBlock;

		pBlock->NumPasses = 0;
		linked_list_push_back(&m_FreeList, &pBlock->LinkNode);
	}
}

