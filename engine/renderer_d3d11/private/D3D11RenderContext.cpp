#include "pch.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11RenderFrame.h"


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
	m_CurrentFrameIndex = (m_LastFrameIndex + 1) % MAX_RENDER_FRAME;
}

void D3D11RenderContext::EndFrame() noexcept
{
	// 현재 프레임의 렌더 큐들을 렌더 커맨드 실행기로 전달
	D3D11RenderCommandExecutor::Get().Push(&m_RenderFrames[m_CurrentFrameIndex]);
	m_LastFrameIndex = m_CurrentFrameIndex;
	m_CurrentFrameIndex = -1;
}

void D3D11RenderContext::BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept
{
	__ASSERT(0 <= m_CurrentFrameIndex, "Invalid render frame index.");

	// 현재 프레임의 렌더 큐에서 해당 렌더 카메라에 대한 렌더 큐가 있는지 검색
	int32 find_index = m_RenderFrames[m_CurrentFrameIndex].OpaqueRQs.find_index(
		[pRenderCamera](const D3D11RenderQueue* pRQ)
		{
			return pRQ->INL_GetRenderCamera() == pRenderCamera;
		}
	);

	// 있으면 해당 렌더 큐를 사용, 없으면 새로 생성
	if (-1 != find_index)
	{
		m_pCurOpaqueRQ = m_RenderFrames[m_CurrentFrameIndex].OpaqueRQs[find_index];
	}
	else
	{
		D3D11RenderQueue* pNewRQ = D3D11RenderQueuePool::Get().GetRQ();
		pNewRQ->SetRenderCamera(static_cast<D3D11RenderCamera*>(pRenderCamera));
		m_RenderFrames[m_CurrentFrameIndex].OpaqueRQs.push_back(pNewRQ);
		m_pCurOpaqueRQ = pNewRQ;
	}

	// 투명 렌더 큐도 동일하게 처리
	find_index = m_RenderFrames[m_CurrentFrameIndex].TransparentRQs.find_index(
		[pRenderCamera](const D3D11RenderQueue* pRQ)
		{
			return pRQ->INL_GetRenderCamera() == pRenderCamera;
		}
	);

	if (-1 != find_index)
	{
		m_pCurTransparentRQ = m_RenderFrames[m_CurrentFrameIndex].TransparentRQs[find_index];
	}
	else
	{
		D3D11RenderQueue* pNewRQ = D3D11RenderQueuePool::Get().GetRQ();
		pNewRQ->SetRenderCamera(static_cast<D3D11RenderCamera*>(pRenderCamera));
		m_RenderFrames[m_CurrentFrameIndex].TransparentRQs.push_back(pNewRQ);
		m_pCurTransparentRQ = pNewRQ;
	}
}

void D3D11RenderContext::EndRenderCamera() noexcept
{
	m_pCurOpaqueRQ = nullptr;
	m_pCurTransparentRQ = nullptr;
}