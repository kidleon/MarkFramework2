#include "pch.h"
#include "D3D11SurfaceMaterialBlockPool.h"
#include "D3D11SurfaceMaterialBlock.h"


constexpr size_t SURFACE_MATERIAL_BLOCK_ALLOC_COUNT = 64;

LINKED_LIST D3D11SurfaceMaterialBlockPool::m_BlockPool = {};

void D3D11SurfaceMaterialBlockPool::Init()
{
	init_linked_list(&m_BlockPool);
	AllocPool();
}

void D3D11SurfaceMaterialBlockPool::Shutdown()
{
	while (!linked_list_empty(&m_BlockPool))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_BlockPool);

		if (!pNode)
			continue;

		D3D11_SURFACE_MATERIAL_BLOCK* pBlock = (D3D11_SURFACE_MATERIAL_BLOCK*)pNode->data;
		D3D11_DELETE(pBlock, D3D11_SURFACE_MATERIAL_BLOCK);
	}
}

D3D11_SURFACE_MATERIAL_BLOCK* D3D11SurfaceMaterialBlockPool::Alloc()
{
	if (linked_list_empty(&m_BlockPool))
		AllocPool();

	LINK_NODE* pNode = linked_list_pop_front(&m_BlockPool);
	D3D11_SURFACE_MATERIAL_BLOCK* pBlock = (D3D11_SURFACE_MATERIAL_BLOCK*)pNode->data;

	return pBlock;
}

void D3D11SurfaceMaterialBlockPool::Release(D3D11_SURFACE_MATERIAL_BLOCK* pBlock)
{
	if (!pBlock)
		return;

	pBlock->Reset();
	linked_list_push_front(&m_BlockPool, &pBlock->LinkNode);
}

void D3D11SurfaceMaterialBlockPool::AllocPool()
{
	for (size_t i = 0; i < SURFACE_MATERIAL_BLOCK_ALLOC_COUNT; ++i)
	{
		D3D11_SURFACE_MATERIAL_BLOCK* pBlock = D3D11_NEW(D3D11_SURFACE_MATERIAL_BLOCK);
		pBlock->LinkNode.data = pBlock;
		linked_list_push_front(&m_BlockPool, &pBlock->LinkNode);
	}
}