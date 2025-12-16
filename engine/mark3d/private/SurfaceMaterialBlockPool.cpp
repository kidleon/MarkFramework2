#include "pch.h"
#include "SurfaceMaterialBlockPool.h"
#include "SurfaceMaterialBlock.h"


constexpr size_t SURFACE_MATERIAL_BLOCK_ALLOC_COUNT = 64;

LINKED_LIST SurfaceMaterialBlockPool::m_BlockPool = {};

void SurfaceMaterialBlockPool::Init()
{
	init_linked_list(&m_BlockPool);
	AllocPool();
}

void SurfaceMaterialBlockPool::Shutdown()
{
	while (!linked_list_empty(&m_BlockPool))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_BlockPool);

		if (!pNode)
			continue;

		SURFACE_MATERIAL_BLOCK* pBlock = (SURFACE_MATERIAL_BLOCK*)pNode->data;
		MARK_DELETE(pBlock, SURFACE_MATERIAL_BLOCK);
	}
}

SURFACE_MATERIAL_BLOCK* SurfaceMaterialBlockPool::Alloc()
{
	if (linked_list_empty(&m_BlockPool))
		AllocPool();

	LINK_NODE* pNode = linked_list_pop_front(&m_BlockPool);
	SURFACE_MATERIAL_BLOCK* pBlock = (SURFACE_MATERIAL_BLOCK*)pNode->data;

	return pBlock;
}

void SurfaceMaterialBlockPool::Release(SURFACE_MATERIAL_BLOCK* pBlock)
{
	if (!pBlock)
		return;

	pBlock->Reset();
	linked_list_push_front(&m_BlockPool, &pBlock->LinkNode);
}

void SurfaceMaterialBlockPool::AllocPool()
{
	for (size_t i = 0; i < SURFACE_MATERIAL_BLOCK_ALLOC_COUNT; ++i)
	{
		SURFACE_MATERIAL_BLOCK* pBlock = MARK_NEW(SURFACE_MATERIAL_BLOCK);
		pBlock->LinkNode.data = pBlock;
		linked_list_push_front(&m_BlockPool, &pBlock->LinkNode);
	}
}