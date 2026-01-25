#include "pch.h"
#include "D3D11RenderCommandExecutor.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderTarget.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderFrame.h"
#include "D3D11ConstantBufferAllocator.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11ShaderDef.h"
#include "D3D11ShaderProgram.h"
#include "D3D11RenderCommand.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11InputLayoutCache.h"


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
	pRenderFrame->Reset();
}

void D3D11RenderCommandExecutor::Execute() noexcept
{
	ID3D11DeviceContext* pDeviceContext = m_pRenderDevice->INL_GetD3D11Context();
	if (!pDeviceContext)
		return;

	const RENDER_SETTINGS& RenderSettings = D3D11_COMMON::GetRenderSettings();

	if (m_RenderFrameQueue.empty())
		return;

	D3D11ConstantBufferAllocator::Get()->ResetTemp();

	D3D11_RENDER_FRAME* pRenderFrame = m_RenderFrameQueue.front();
	m_RenderFrameQueue.pop_front();

	// 리소스 커맨드 처리
	for(size_t i = 0; i < pRenderFrame->ResourceCommands.size(); ++i)
	{
		D3D11_RESOURCE_COMMAND* pResCmd = pRenderFrame->ResourceCommands[i];
		if (!pResCmd)
			continue;

		switch (pResCmd->CommandFunc)
		{
			case D3D11_RESOURCE_COMMAND::COMMAND_FUNC::UPDATE_PRIMITIVE_BUFFER: // 프리미티브 버퍼 업데이트
			{
				D3D11PrimitiveBuffer* pPB = pResCmd->pPrimitiveBuffer;
				if (pPB)
				{
					pPB->UploadToGPU_VB(pDeviceContext);
					pPB->UploadToGPU_IB(pDeviceContext);
				}
			} break;
		default:
			break;
		}
	}

	for (size_t i = 0; i < pRenderFrame->NumRQs; ++i)
	{
		D3D11_RENDER_QUEUE_GROUP* pRQGroup = &pRenderFrame->RQs[i];

		// 렌더 카메라 설정
		const D3D11RenderCamera* pCamera = pRQGroup->INL_GetRenderCamera();
		if (!pCamera)
			continue;

		// 카메라 상수 버퍼 설정
		D3D11_CAMERA_CONSTANT CameraCB = {};
		CameraCB.ViewMatrix = mat4_transpose((MATRIX4*)&pCamera->INL_GetViewMatrix());
		CameraCB.InvViewMatrix = mat4_transpose((MATRIX4*)&mat4_inverse(&CameraCB.ViewMatrix));
		CameraCB.ProjectionMatrix = mat4_transpose((MATRIX4*)&pCamera->INL_GetProjectionMatrix());

		const D3D11RenderCamera::VIEW_DESC& ViewDesc = pCamera->INL_GetViewDesc();
		CameraCB.CameraPosition = { ViewDesc.EyePos.x, ViewDesc.EyePos.y, ViewDesc.EyePos.z, 1.0f };

		// 임시 상수 버퍼 할당
		D3D11ConstantBuffer* pCB = D3D11ConstantBufferAllocator::Get()->AcquireTemp(sizeof(D3D11_CAMERA_CONSTANT));
		if (!pCB)
		{
			SYS_LOG_E("D3D11RenderCommandExecutor::Execute - D3D11ConstantBufferAllocator::AcquireTemp failed.");
			continue;
		}

		// GPU 업로드
		if (!pCB->UploadToGPU(pDeviceContext, &CameraCB, sizeof(D3D11_CAMERA_CONSTANT)))
			continue;

		ID3D11Buffer* pCameraCB = pCB->INL_GetD3D11Buffer();
		pDeviceContext->VSSetConstantBuffers(0, 1, &pCameraCB);
		pDeviceContext->PSSetConstantBuffers(0, 1, &pCameraCB);
		
		D3D11RenderTarget* pRT = pCamera->INL_GetRenderTarget();
		if (pRT)
		{
			// 렌더 타겟 바인딩
			ID3D11RenderTargetView* pRTV = pRT->INL_GetRTV();
			ID3D11DepthStencilView* pDSV = pRT->INL_GetDSV();
			pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);

			const D3D11RenderCamera::CLEAR_TARGET_DESC& ClearDesc = pCamera->INL_GetClearTargetDesc();
			if (ClearDesc.ClearBuffers & (uint32)CLEAR_BUFFER::COLOR)
			{
				pDeviceContext->ClearRenderTargetView(
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
				pDeviceContext->ClearDepthStencilView(
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
			pDeviceContext->RSSetViewports(1, &viewport);

			// SHADER CONSTANT BUFFER 설정

			// PER FRAME CB
			D3D11_RENDER_QUEUE* pRQ = pRQGroup->INL_GetOpaqueRQ();
			pRQ->Sort();

			D3D11ShaderProgram* pSetVS = nullptr;
			D3D11ShaderProgram* pSetPS = nullptr;
			D3D11PrimitiveBuffer* pSetPB = nullptr;
			ID3D11RasterizerState* pSetRS = nullptr;

			UINT32 SetInputVertexFormat = 0;
			ID3D11InputLayout* pSetIL = nullptr;

			size_t NumCommands = pRQ->INL_GetNumCommands();
			for (size_t c = 0; c < NumCommands; ++c)
			{
				D3D11_DRAW_COMMAND* pCmd = pRQ->INL_GetCommandAt(c);
				if (!pCmd) continue;

				// 셰이더 프로그램 바인딩

				// 버텍스 셰이더
				if (pSetVS != pCmd->RenderPipeline.pVertexShader)
				{
					CHECK_RELEASE(pSetVS);

					pSetVS = pCmd->RenderPipeline.pVertexShader;
					pSetVS->AddRef();

					ID3D11VertexShader* pVS = pSetVS ? pSetVS->INL_GetVertexShader() : nullptr;
					pDeviceContext->VSSetShader(pVS, nullptr, 0);

					UINT32 InputVertexFormat = pSetVS->INL_GetInputVertexFormat();
					if (SetInputVertexFormat != InputVertexFormat)
					{
						SetInputVertexFormat = InputVertexFormat;

						pSetIL = D3D11InputLayoutCache::Get()->Find(SetInputVertexFormat);
						pDeviceContext->IASetInputLayout(pSetIL);
					}
				}

				// 픽셀 셰이더
				if (pSetPS != pCmd->RenderPipeline.pPixelShader)
				{
					CHECK_RELEASE(pSetPS);

					pSetPS = pCmd->RenderPipeline.pPixelShader;
					pSetPS->AddRef();

					ID3D11PixelShader* pPS = pSetPS ? pSetPS->INL_GetPixelShader() : nullptr;
					pDeviceContext->PSSetShader(pPS, nullptr, 0);
				}

				D3D11ConstantBuffer* pObjCB = D3D11ConstantBufferAllocator::Get()->AcquireTemp(sizeof(D3D11_OBJECT_CONSTANT));
				if (!pObjCB)
				{
					SYS_LOG_E("D3D11RenderCommandExecutor::Execute - D3D11ConstantBufferAllocator::AcquireTemp failed.");
					continue;
				}

				D3D11_OBJECT_CONSTANT ObjCB = {};
				pObjCB->UploadToGPU(pDeviceContext, &pCmd->ObjectConstant, sizeof(D3D11_OBJECT_CONSTANT));

				ID3D11Buffer* pObjCBBuffer = pObjCB->INL_GetD3D11Buffer();

				pDeviceContext->VSSetConstantBuffers(1, 1, &pObjCBBuffer);
				pDeviceContext->PSSetConstantBuffers(1, 1, &pObjCBBuffer);

				// 렌더 스테이트 설정
				if (pSetRS != pCmd->RenderPipeline.pRasterizerState)
				{
					pSetRS = pCmd->RenderPipeline.pRasterizerState;
					pDeviceContext->RSSetState(pSetRS);
				}

				if (!pCmd->pPrimitiveBuffer)
				{
					SYS_LOG_W("D3D11RenderCommandExecutor::Execute - pCmd->pPrimitiveBuffer is nullptr.");
					continue;
				}

				// 프리미티브 버퍼 바인딩
				if (pSetPB != pCmd->pPrimitiveBuffer)
				{
					CHECK_RELEASE(pSetPB);

					pSetPB = pCmd->pPrimitiveBuffer;
					if (pSetPB)
						pSetPB->AddRef();
				}

				const D3D11PrimitiveBuffer::PRIMITIVE_DESC& PrimitiveDesc = pSetPB->INL_GetPrimitiveDesc(pCmd->DrawPrimitiveIndex);

				if (!PrimitiveDesc.VertexCount || !PrimitiveDesc.IndexCount)
				{
					SYS_LOG_W("D3D11RenderCommandExecutor::Execute - PrimitiveDesc has zero VertexCount or IndexCount.");
					continue;
				}

				ID3D11Buffer* pVB = pSetPB->INL_GetD3D11VertexBuffer();
				ID3D11Buffer* pIB = pSetPB->INL_GetD3D11IndexBuffer();

				UINT32 VertexOffset = PrimitiveDesc.VertexOffset;
				UINT32 IndexOffset = PrimitiveDesc.IndexOffset;

				UINT32 VertexStride = PrimitiveDesc.VertexStride;
				UINT32 IndexStride = PrimitiveDesc.IndexStride;

				pDeviceContext->IASetVertexBuffers(
					0,
					1,
					&pVB,
					&VertexStride,
					&VertexOffset
				);

				DXGI_FORMAT IndexFormat = (IndexStride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
				pDeviceContext->IASetIndexBuffer(pIB, IndexFormat, IndexOffset);

				D3D11_PRIMITIVE_TOPOLOGY D3D11PrimitiveTopology = D3D11_IMPL_PRIMITIVE_TOPOLOGY[(UINT32)PrimitiveDesc.PrimitiveType];
				pDeviceContext->IASetPrimitiveTopology(D3D11PrimitiveTopology);

				// 드로우 콜
				pDeviceContext->DrawIndexed(
					PrimitiveDesc.IndexCount,
					0,
					0
				);
			}

			CHECK_RELEASE(pSetPB);
			CHECK_RELEASE(pSetVS);
			CHECK_RELEASE(pSetPS);
		}
	}

	IDXGISwapChain* pSwapChain = m_pRenderDevice->INL_GetSwapChain();
	HRESULT hr = pSwapChain->Present(RenderSettings.VSyncEnabled, 0); // 프레임 업데이트!
	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11RenderCommandExecutor::Execute - IDXGISwapChain::Present failed. hr = 0x%08X", hr);
	}
	
	ResetFrame(pRenderFrame);
}

