#include "pch.h"
#include "SurfaceMaterialPool.h"
#include "SurfaceMaterial.h"


SurfaceMaterialPool::~SurfaceMaterialPool()
{
	while (!linked_list_empty(&m_FreeList))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
		SurfaceMaterial* pMaterial = static_cast<SurfaceMaterial*>(pNode->data);
		pMaterial->Release();
	}
}

void SurfaceMaterialPool::Init(size_t InitialCapacity)
{
	m_Capacity = InitialCapacity;

	init_linked_list(&m_FreeList);

	AllocBlock();
}

SurfaceMaterial* SurfaceMaterialPool::Alloc()
{
	if (linked_list_empty(&m_FreeList))
		AllocBlock();

	if (linked_list_empty(&m_FreeList))
	{
		SYS_LOG_E("SurfaceMaterialPool::Alloc - Failed to allocate SurfaceMaterial.");
		return nullptr;
	}

	LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);

	SurfaceMaterial* pSurfaceMaterial = static_cast<SurfaceMaterial*>(pNode->data);
	pSurfaceMaterial->AddRef();

	return pSurfaceMaterial;
}

void SurfaceMaterialPool::Release(SurfaceMaterial* pMaterial)
{
	if (!pMaterial) return;

	pMaterial->Reset();
	pMaterial->Release();

	linked_list_push_front(&m_FreeList, pMaterial->INL_GetLinkNode());
}

void SurfaceMaterialPool::AllocBlock()
{
	for (size_t i = 0; i < m_Capacity; ++i)
	{
		SurfaceMaterial* pMaterial = MARK_NEW(SurfaceMaterial);

		LINK_NODE* pNode = pMaterial->INL_GetLinkNode();
		pNode->data = (void*)pMaterial;

		linked_list_push_front(&m_FreeList, pMaterial->INL_GetLinkNode());
	}
}
