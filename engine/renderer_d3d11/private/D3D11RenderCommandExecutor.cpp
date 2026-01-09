#include "pch.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderTarget.h"
#include "D3D11RenderCamera.h"


D3D11RenderCommandExecutor* D3D11RenderCommandExecutor::s_pInstance = nullptr;

D3D11RenderCommandExecutor::D3D11RenderCommandExecutor(D3D11RenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice)
	, m_CurrentFrameIndex(0)
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11RenderCommandExecutor::~D3D11RenderCommandExecutor() noexcept
{
	for (size_t g = 0; g < MAX_RQ_GROUPS; ++g)
	{
		for (size_t i = 0; i < m_RQGroups[g].lstRenderQueue.size(); ++i)
		{
			D3D11RenderQueue* pRQ = m_RQGroups[g].lstRenderQueue[i];
			if (pRQ)
			{
				pRQ->Reset();
				D3D11RenderQueuePool::Get().ReleaseRQ(pRQ);
			}
		}
		m_RQGroups[g].lstRenderQueue.clear();
	}

	if (s_pInstance == this)
		s_pInstance = nullptr;
}

void D3D11RenderCommandExecutor::Push(const RENDER_FRAME* pRenderFrame) noexcept
{
	m_RenderFrameQueue.push_back(const_cast<RENDER_FRAME*>(pRenderFrame));
}

void D3D11RenderCommandExecutor::ResetFrame(size_t Frame)
{
	for (size_t i = 0; i < m_RQGroups[Frame].lstRenderQueue.size(); ++i)
	{
		D3D11RenderQueue* pRQ = m_RQGroups[Frame].lstRenderQueue[i];
		if (pRQ)
		{
			D3D11RenderQueuePool::Get().ReleaseRQ(pRQ);
		}
	}

	m_RQGroups[Frame].lstRenderQueue.clear();
}

void D3D11RenderCommandExecutor::Execute() noexcept
{
	ID3D11DeviceContext* pContext = m_pRenderDevice->INL_GetD3D11Context();
	if (!pContext)
		return;

	const RENDER_SETTINGS& RenderSettings = D3D11Common::GetRenderSettings();

	size_t CurrentFrameIndex = m_CurrentFrameIndex;
	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_RQ_GROUPS; // 다음 프레임 인덱스로 변경

	const RenderQueueGroup& RQGroup = m_RQGroups[CurrentFrameIndex];
	const TArray<D3D11RenderQueue*, TA_POOL>& lstRenderQueue = RQGroup.lstRenderQueue;

	for (size_t i = 0; i < lstRenderQueue.size(); ++i)
	{
		D3D11RenderQueue* pRQ = lstRenderQueue[i];
		if (!pRQ)
			continue;

		// 렌더 카메라 설정
		D3D11RenderCamera* pCamera = pRQ->INL_GetRenderCamera();
		if (pCamera)
		{
			D3D11RenderTarget* pRT = pCamera->INL_GetRenderTarget();
			if (pRT)
			{
				// 렌더 타겟 바인딩
				ID3D11RenderTargetView* pRTV = pRT->INL_GetRTV();
				ID3D11DepthStencilView* pDSV = pRT->INL_GetDSV();
				pContext->OMSetRenderTargets(1, &pRTV, pDSV);

				const D3D11RenderCamera::CLEAR_TARGET_DESC& ClearDesc = pCamera->INL_GetClearTargetDesc();

				if (ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::COLOR)
				{
					pContext->ClearRenderTargetView(
						pRTV,
						ClearDesc.ClearColor.v
					);
				}

				UINT ClearFlags = 0;
				if (ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::DEPTH && ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::STENCIL)
				{
					ClearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
				}
				else if (ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::DEPTH && !(ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::STENCIL))
				{
					ClearFlags = D3D11_CLEAR_DEPTH;
				}
				else if (!(ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::DEPTH) && (ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::STENCIL))
				{
					ClearFlags = D3D11_CLEAR_STENCIL;
				}

				if (pDSV && ClearFlags > 0)
				{
					pContext->ClearDepthStencilView(
						pDSV,
						ClearFlags,
						ClearDesc.Depth,
						static_cast<UINT8>(ClearDesc.Stencil)
					);
				}

				// 뷰포트 설정
				D3D11_VIEWPORT viewport = {};
				viewport.TopLeftX = 0.0f;
				viewport.TopLeftY = 0.0f;
				viewport.Width = static_cast<FLOAT>(pRT->INL_GetColorWidth());
				viewport.Height = static_cast<FLOAT>(pRT->INL_GetColorHeight());
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				pContext->RSSetViewports(1, &viewport);
			}
		}
	}

	ResetFrame(CurrentFrameIndex);

	IDXGISwapChain* pSwapChain = m_pRenderDevice->INL_GetSwapChain();
	HRESULT hr = pSwapChain->Present(RenderSettings.VSyncEnabled, 0); // 프레임 업데이트!
	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderCommandExecutor::Execute - IDXGISwapChain::Present failed. hr = 0x%08X", hr);
	}
}


/*
// 불투명 렌더 명령 실행
for (size_t cmdIdx = 0; cmdIdx < pRQ->m_OpaqueCmdList.size(); ++cmdIdx)
{
	BASE_RENDER_COMMAND* pCmd = pRQ->m_OpaqueCmdList[cmdIdx];
	if (pCmd && pCmd->ExecuteFunc)
	{
		pCmd->ExecuteFunc(pContext, pCmd);
	}
}
*/