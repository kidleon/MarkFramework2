#include "pch.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderCommandExecutor.h"


long D3D11RenderContext::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11RenderContext::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11RenderContext);
	}
	return NewRefCnt;
}

long D3D11RenderContext::RefCnt()
{
	return m_RefCnt;
}

void D3D11RenderContext::BeginFrame() noexcept
{
}

void D3D11RenderContext::EndFrame() noexcept
{
}

void D3D11RenderContext::BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept
{
	D3D11RenderQueue* pRQ = D3D11RenderQueuePool::Get().GetRQ();
	if (!pRQ)
		return;

	m_pCurRQ = pRQ;
	m_pCurRQ->SetRenderCamera(static_cast<D3D11RenderCamera*>(pRenderCamera));
}

void D3D11RenderContext::EndRenderCamera() noexcept
{
	if (!m_pCurRQ)
		return;

	D3D11RenderCommandExecutor::Get().Push(m_pCurRQ);
	m_pCurRQ = nullptr;
}