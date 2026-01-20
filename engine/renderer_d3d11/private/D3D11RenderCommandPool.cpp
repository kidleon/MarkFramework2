#include "pch.h"
#include "D3D11RenderCommandPool.h"
#include "D3D11RenderCommand.h"


D3D11RenderCommandPool* D3D11RenderCommandPool::s_pInstance = nullptr;

D3D11RenderCommandPool::D3D11RenderCommandPool()
{
	if (!s_pInstance)
	{
		s_pInstance = this;
	}
}

D3D11RenderCommandPool::~D3D11RenderCommandPool() noexcept
{
	Cleanup();
	s_pInstance = nullptr;
}

BOOL D3D11RenderCommandPool::Init(size_t InitialCapacity)
{
	ExpandPool(InitialCapacity);
	return TRUE;
}

void D3D11RenderCommandPool::Cleanup()
{
	LINK_NODE* pNode = m_FreeList.head;

	while (pNode)
	{
		LINK_NODE* pNextNode = pNode->next;
		D3D11_DRAW_COMMAND* pCommand = (D3D11_DRAW_COMMAND*)pNode->data;
		D3D11_POOL_FREE(pCommand);
		pNode = pNextNode;
	}

	m_FreeList.head = nullptr;
	m_FreeList.tail = nullptr;

	pNode = m_UsedList.head;
	while (pNode)
	{
		LINK_NODE* pNextNode = pNode->next;
		D3D11_DRAW_COMMAND* pCommand = (D3D11_DRAW_COMMAND*)pNode->data;
		D3D11_POOL_FREE(pCommand);
		pNode = pNextNode;
	}

	m_UsedList.head = nullptr;
	m_UsedList.tail = nullptr;
}

D3D11_DRAW_COMMAND* D3D11RenderCommandPool::Acquire()
{
	if (m_FreeList.size == 0)
	{
		ExpandPool(16);
	}

	LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
	if (!pNode)
		return nullptr;

	// Add to used list
	linked_list_push_back(&m_UsedList, pNode);

	return (D3D11_DRAW_COMMAND*)pNode->data;
}

void D3D11RenderCommandPool::Release(D3D11_DRAW_COMMAND* pCommand)
{
	if (!pCommand)
		return;

	// Remove from used list
	linked_list_remove_node(&m_UsedList, &pCommand->LinkNode);

	// Add to free list
	linked_list_push_front(&m_FreeList, &pCommand->LinkNode);
}

void D3D11RenderCommandPool::ExpandPool(size_t Count)
{
	for (size_t i = 0; i < Count; ++i)
	{
		D3D11_DRAW_COMMAND* pCommand = (D3D11_DRAW_COMMAND*)D3D11_POOL_ALLOC(sizeof(D3D11_DRAW_COMMAND));
		memset(pCommand, 0, sizeof(D3D11_DRAW_COMMAND));
		pCommand->LinkNode.data = pCommand;
		linked_list_push_front(&m_FreeList, &pCommand->LinkNode);
	}
}