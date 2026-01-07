#include "pch.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderQueue.h"


D3D11RenderQueuePool* D3D11RenderQueuePool::s_pInstance = nullptr;

D3D11RenderQueuePool::D3D11RenderQueuePool()
	: m_FreeList{}
	, m_UsedList{}
{
}

D3D11RenderQueuePool::~D3D11RenderQueuePool() noexcept
{
	Shutdown();
}

void D3D11RenderQueuePool::Init()
{
	init_linked_list(&m_FreeList);
	init_linked_list(&m_UsedList);

	AllocRQ(16);
}

void D3D11RenderQueuePool::Shutdown() noexcept
{
	while (!linked_list_empty(&m_UsedList))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_UsedList);
		D3D11RenderQueue* pRQ = static_cast<D3D11RenderQueue*>(pNode->data);
		D3D11_POOL_DELETE(pRQ, D3D11RenderQueue);
	}
	
	while (!linked_list_empty(&m_FreeList))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
		D3D11RenderQueue* pRQ = static_cast<D3D11RenderQueue*>(pNode->data);
		D3D11_POOL_DELETE(pRQ, D3D11RenderQueue);
	}
}

D3D11RenderQueue* D3D11RenderQueuePool::GetRQ() noexcept
{
	if (linked_list_empty(&m_FreeList))
		AllocRQ(4);

	LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
	D3D11RenderQueue* pRQ = static_cast<D3D11RenderQueue*>(pNode->data);

	linked_list_push_back(&m_UsedList, pNode);

	return pRQ;

}

void D3D11RenderQueuePool::ReleaseRQ(D3D11RenderQueue* pRQ) noexcept
{
	if (!pRQ) return;

	LINK_NODE* pNode = pRQ->INL_GetLinkNode();
	pRQ->Reset();

	// UsedList에서 제거
	linked_list_remove_node(&m_UsedList, pNode);
	
	// FreeList에 추가
	linked_list_push_back(&m_FreeList, pNode);
}

void D3D11RenderQueuePool::AllocRQ(size_t Count) noexcept
{
	for (size_t i = 0; i < Count; ++i)
	{
		D3D11RenderQueue* pRQ = D3D11_POOL_NEW(D3D11RenderQueue)();
		linked_list_push_back(&m_FreeList, pRQ->INL_GetLinkNode());
	}
}