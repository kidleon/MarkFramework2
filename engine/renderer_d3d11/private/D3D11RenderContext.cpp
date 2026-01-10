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

__FORCEINLINE int32 FindRQ(
	const D3D11_RENDER_QUEUE_GROUP* pRQs,
	size_t NumRQs,
	IRenderCamera* pRenderCamera
) noexcept
{
	for (size_t i = 0; i < NumRQs; ++i)
	{
		if (pRQs[i].INL_GetRenderCamera() == pRenderCamera)
			return static_cast<int32>(i);
	}

	return -1;
}

void D3D11RenderContext::BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept
{
	__ASSERT(0 <= m_CurrentFrameIndex, "Invalid render frame index.");

	// 현재 프레임의 렌더 큐에서 해당 렌더 카메라에 대한 렌더 큐가 있는지 검색
	int32 find_index = FindRQ(
		m_RenderFrames[m_CurrentFrameIndex].RQs, 
		m_RenderFrames[m_CurrentFrameIndex].NumRQs, 
		pRenderCamera
	);

	// 있으면 해당 렌더 큐를 사용, 없으면 새로 생성
	if (-1 != find_index)
	{
		m_pCurRQs = &m_RenderFrames[m_CurrentFrameIndex].RQs[find_index];
	}
	else
	{
		if ((m_RenderFrames[m_CurrentFrameIndex].NumRQs + 1) >= D3D11_RENDER_FRAME::MAX_RQ_GROUPS)
		{
			SYS_LOG_E("D3D11RenderContext::BeginRenderCamera: Exceeded maximum render queue groups per frame.");
			m_pCurRQs = nullptr;
			return;
		}

		++m_RenderFrames[m_CurrentFrameIndex].NumRQs;
		int32 RQ_Index = static_cast<int32>(m_RenderFrames[m_CurrentFrameIndex].NumRQs - 1);
		m_pCurRQs = &m_RenderFrames[m_CurrentFrameIndex].RQs[RQ_Index];
	}

	m_pCurRQs->PrepareRQ(static_cast<D3D11RenderCamera*>(pRenderCamera));
}

void D3D11RenderContext::EndRenderCamera() noexcept
{
	m_pCurRQs = nullptr;
}