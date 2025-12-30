#include "pch.h"
#include "D3D11RenderManager.h"
#include "D3D11RenderQueuePool.h"


D3D11RenderManager* D3D11RenderManager::s_pInstance = nullptr;

D3D11RenderManager::D3D11RenderManager()
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11RenderManager::~D3D11RenderManager() noexcept
{
	Shutdown();
	s_pInstance = nullptr;
}

BOOL D3D11RenderManager::Init()
{
	init_spin_lock(&m_RQLock);

	m_pRQPool = D3D11_POOL_NEW(D3D11RenderQueuePool);
	if (!m_pRQPool)
	{
		SYS_LOG_E("D3D11RenderManager::Init: Failed to allocate D3D11RenderQueuePool");
		return FALSE;
	}

	if (!m_pRQPool->Init(8)) // Initial RQ Pool capacity
	{
		SYS_LOG_E("D3D11RenderManager::Init: Failed to initialize D3D11RenderQueuePool");
		D3D11_POOL_DELETE(m_pRQPool, D3D11RenderQueuePool);
		m_pRQPool = nullptr;

		return FALSE;
	}

	return TRUE;
}

void D3D11RenderManager::Shutdown()
{
	if (m_pRQPool)
	{
		D3D11_POOL_DELETE(m_pRQPool, D3D11RenderQueuePool);
		m_pRQPool = nullptr;
	}
}

D3D11RenderQueue* D3D11RenderManager::PrepareRQ() noexcept
{
	D3D11RenderQueue* pRQ = m_pRQPool->GetRQ();
	return pRQ;
}

void D3D11RenderManager::PostRQ(D3D11RenderQueue* pRQ) noexcept
{
	D3D11_AUTO_SYNC sync(&m_RQLock);

	int index = m_ActiveRQList.find_index(pRQ);
	if (0 <= index)
	{
		SYS_LOG_W("D3D11RenderManager::PostRQ: D3D11RenderQueue is already posted to active list");
		return;
	}

	m_ActiveRQList.push_back(pRQ);
}