#include "pch.h"
#include "D3D11RenderQueuePool.h"


D3D11RenderQueuePool::D3D11RenderQueuePool()
{
}

D3D11RenderQueuePool::~D3D11RenderQueuePool() noexcept
{
	Shutdown();
}

BOOL D3D11RenderQueuePool::Init(size_t InitialCapacity)
{
	init_spin_lock(&m_Lock);

	init_linked_list(&m_PoolList);

	for (size_t i = 0; i < InitialCapacity; ++i)
	{
		D3D11RenderQueue* pRQ = D3D11_POOL_NEW(D3D11RenderQueue);
		if (!pRQ)
		{
			SYS_LOG_E("D3D11RenderQueuePool::Init: Failed to allocate D3D11RenderQueue");

			return FALSE;
		}

		pRQ->Init(256); // Initial capacity per RQ
		
		linked_list_push_back(&m_PoolList, pRQ->INL_GetLinkNode());
	}

	return TRUE;
}

void D3D11RenderQueuePool::Shutdown()
{
	while (!linked_list_empty(&m_PoolList))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_PoolList);
		D3D11RenderQueue* pRQ = (D3D11RenderQueue*)pNode->data;
		D3D11_POOL_DELETE(pRQ, D3D11RenderQueue);
	}
}

D3D11RenderQueue* D3D11RenderQueuePool::GetRQ()
{
	D3D11_AUTO_SYNC sync(&m_Lock);

	if (linked_list_empty(&m_PoolList))
	{
		SYS_LOG_E("D3D11RenderQueuePool::GetRQ: Failed to allocate D3D11RenderQueue");
		return nullptr;
	}

	// 풀에서 하나 꺼내기
	LINK_NODE* pNode = linked_list_pop_front(&m_PoolList);
	D3D11RenderQueue* pRQ = (D3D11RenderQueue*)pNode->data;
	return pRQ;
}

void D3D11RenderQueuePool::ReleaseRQ(D3D11RenderQueue* pRQ)
{
	if (!pRQ)
		return;

	pRQ->Clear();

	D3D11_AUTO_SYNC sync(&m_Lock);

	// 풀에 다시 넣기
	linked_list_push_back(&m_PoolList, pRQ->INL_GetLinkNode());
}