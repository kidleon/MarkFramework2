#include "pch.h"
#include "SceneNodePool.h"
#include "SceneNode.h"
#include "CoreHeap.h"


SceneNodePool::~SceneNodePool()
{
	Shutdown();
}

void SceneNodePool::Initialize(size_t PageSize)
{
	ExpandPool(PageSize);
}

void SceneNodePool::Shutdown()
{
	LINK_NODE* pNode = m_UsedList.head;
	while (pNode)
	{
		LINK_NODE* pNextNode = pNode->next;
		SceneNode* pSceneNode = (SceneNode*)pNode->data;

		CHECK_RELEASE(pSceneNode);
		
		pNode = pNextNode;
	}
	m_UsedList.head = nullptr;
	m_UsedList.tail = nullptr;

	pNode = m_FreeList.head;
	while (pNode)
	{
		LINK_NODE* pNextNode = pNode->next;
		SceneNode* pSceneNode = (SceneNode*)pNode->data;

		CHECK_RELEASE(pSceneNode);

		pNode = pNextNode;
	}
	m_FreeList.head = nullptr;
	m_FreeList.tail = nullptr;
}

SceneNode* SceneNodePool::Acquire()
{
	if (m_FreeList.size == 0)
	{
		ExpandPool(16);
	}

	LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
	if (!pNode)
		return nullptr;

	SceneNode* pSceneNode = (SceneNode*)pNode->data;
	pSceneNode->AddRef();

	linked_list_push_back(&m_UsedList, pNode);

	return pSceneNode;
}

void SceneNodePool::Release(SceneNode* pNode)
{
	if (!pNode)
		return;

	LINK_NODE* pLinkNode = pNode->INL_GetPoolLinkNode();
	linked_list_remove_node(&m_UsedList, pLinkNode);

	linked_list_push_back(&m_FreeList, pLinkNode);

	pNode->Reset(FALSE);
	pNode->Release();
}

void SceneNodePool::ExpandPool(size_t Count)
{
	for (size_t i = 0; i < Count; ++i)
	{
		SceneNode* pSceneNode = CORE_NEW(SceneNode)();
		LINK_NODE* pLinkNode = pSceneNode->INL_GetPoolLinkNode();
		pLinkNode->data = pSceneNode;
		linked_list_push_back(&m_FreeList, pLinkNode);
	}
}


