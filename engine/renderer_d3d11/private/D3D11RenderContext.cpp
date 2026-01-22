#include "pch.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11RenderCommand.h"
#include "D3D11RenderCommandPool.h"
#include "D3D11RenderFrame.h"
#include "D3D11SurfaceMaterial.h"
#include "D3D11PrimitiveBuffer.h"


D3D11RenderContext::~D3D11RenderContext() noexcept
{
	for (int32 i = 0; i < MAX_RENDER_FRAME; ++i)
	{
		m_RenderFrames[i].Reset();
	}
}

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
		D3D11_DELETE(this, D3D11RenderContext);
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
		if (m_RenderFrames[m_CurrentFrameIndex].NumRQs >= D3D11_RENDER_FRAME::MAX_RQ_GROUPS)
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

void D3D11RenderContext::SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial)
{
	CHECK_RELEASE(m_pCurSurfaceMaterial);

	if (pSurfaceMaterial)
	{
		m_pCurSurfaceMaterial = static_cast<D3D11SurfaceMaterial*>(pSurfaceMaterial);
		m_pCurSurfaceMaterial->AddRef();
	}
}

void D3D11RenderContext::SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer)
{
	CHECK_RELEASE(m_pCurPrimitiveBuffer);
	if (pPrimitiveBuffer)
	{
		m_pCurPrimitiveBuffer = static_cast<D3D11PrimitiveBuffer*>(pPrimitiveBuffer);
		m_pCurPrimitiveBuffer->AddRef();
	}
}

void D3D11RenderContext::DrawPrimitive(int32 PrimitiveIndex)
{
	if (!m_pCurSurfaceMaterial || !m_pCurPrimitiveBuffer || !m_pCurRQs)
		return;

	int32 NumPass = m_pCurSurfaceMaterial->INL_GetNumPass();

	if (0 >= NumPass)
		return;

	for (int32 p = 0; p < NumPass; ++p)
	{
		D3D11_DRAW_COMMAND* pDrawCommand = D3D11RenderCommandPool::Get()->Acquire();

		m_pCurPrimitiveBuffer->AddRef();
		pDrawCommand->pPrimitiveBuffer = m_pCurPrimitiveBuffer;
		pDrawCommand->DrawPrimitiveIndex = PrimitiveIndex;

		pDrawCommand->RenderPipeline.pVertexShader = m_pCurSurfaceMaterial->INL_GetVertexShader(p);
		pDrawCommand->RenderPipeline.pPixelShader = m_pCurSurfaceMaterial->INL_GetPixelShader(p);
		pDrawCommand->RenderPipeline.Color = m_pCurSurfaceMaterial->INL_GetColor(p);

		pDrawCommand->SortKey.Pass = p;

		pDrawCommand->SortKey.VertexShaderIndex = pDrawCommand->RenderPipeline.pVertexShader ?
			pDrawCommand->RenderPipeline.pVertexShader->INL_GetShaderIndex() % MAX_VERTEX_SHADER_INDEX : 0;

		pDrawCommand->SortKey.PixelShaderIndex = pDrawCommand->RenderPipeline.pPixelShader ?
			pDrawCommand->RenderPipeline.pPixelShader->INL_GetShaderIndex() % MAX_PIXEL_SHADER_INDEX : 0;

		pDrawCommand->SortKey.RenderStateHash = 0; // TODO: 렌더 상태 해시 계산
		pDrawCommand->SortKey.Depth = 0; // TODO: 깊이 값 설정

		// 일단 임시로 불투명 렌더 큐에 추가
		D3D11_RENDER_QUEUE* pRQ = m_pCurRQs->INL_GetOpaqueRQ();
		pRQ->Add(RENDER_QUEUE_TYPE::RQ_OPAQUE, pDrawCommand);
	}
}