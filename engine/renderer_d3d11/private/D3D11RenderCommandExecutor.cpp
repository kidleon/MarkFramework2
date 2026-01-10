#include "pch.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderTarget.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderFrame.h"


D3D11RenderCommandExecutor* D3D11RenderCommandExecutor::s_pInstance = nullptr;

D3D11RenderCommandExecutor::D3D11RenderCommandExecutor(D3D11RenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice)
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11RenderCommandExecutor::~D3D11RenderCommandExecutor() noexcept
{
	if (!m_RenderFrameQueue.empty())
	{
		const D3D11_RENDER_FRAME* pRenderFrame = m_RenderFrameQueue.front();
		while (pRenderFrame)
		{
			ResetFrame(const_cast<D3D11_RENDER_FRAME*>(pRenderFrame));
			m_RenderFrameQueue.pop_front();

			if (m_RenderFrameQueue.empty())
				break;

			pRenderFrame = m_RenderFrameQueue.front();
		}
	}
	
	if (s_pInstance == this)
		s_pInstance = nullptr;
}

void D3D11RenderCommandExecutor::Push(const D3D11_RENDER_FRAME* pRenderFrame) noexcept
{
	m_RenderFrameQueue.push_back(const_cast<D3D11_RENDER_FRAME*>(pRenderFrame));
}

void D3D11RenderCommandExecutor::ResetFrame(D3D11_RENDER_FRAME* pRenderFrame)
{
	for (size_t i = 0; i < pRenderFrame->NumRQs; ++i)
		pRenderFrame->RQs[i].Reset();
	pRenderFrame->NumRQs = 0;
}

void D3D11RenderCommandExecutor::Execute() noexcept
{
	ID3D11DeviceContext* pContext = m_pRenderDevice->INL_GetD3D11Context();
	if (!pContext)
		return;

	const RENDER_SETTINGS& RenderSettings = D3D11Common::GetRenderSettings();

	if (m_RenderFrameQueue.empty())
		return;

	D3D11_RENDER_FRAME* pRenderFrame = m_RenderFrameQueue.front();
	m_RenderFrameQueue.pop_front();

	for (size_t i = 0; i < pRenderFrame->NumRQs; ++i)
	{
		D3D11_RENDER_QUEUE_GROUP* pRQGroup = &pRenderFrame->RQs[i];

		// 렌더 카메라 설정
		const D3D11RenderCamera* pCamera = pRQGroup->INL_GetRenderCamera();
		if (!pCamera)
			continue;

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

	ResetFrame(pRenderFrame);

	IDXGISwapChain* pSwapChain = m_pRenderDevice->INL_GetSwapChain();
	HRESULT hr = pSwapChain->Present(RenderSettings.VSyncEnabled, 0); // 프레임 업데이트!
	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderCommandExecutor::Execute - IDXGISwapChain::Present failed. hr = 0x%08X", hr);
	}
}

