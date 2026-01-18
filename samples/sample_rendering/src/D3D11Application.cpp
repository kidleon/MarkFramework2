#include "pch.h"
#include "D3D11Application.h"
#include "RenderSystemFactory.h"


D3D11Application::~D3D11Application() noexcept
{
	OnDestroy();
}

BOOL D3D11Application::OnInit(HWND hWnd, int width, int height)
{
	m_hWnd = hWnd;
	m_Width = width;
	m_Height = height;

	ENGINE_CREATE_DESC CreateDesc = {};
	CreateDesc.hWnd = hWnd;
	CreateDesc.ScreenWidth = width;
	CreateDesc.ScreenHeight = height;
	CreateDesc.Fullscreen = FALSE;
	CreateDesc.RenderAPI = RENDER_API::D3D11;
	BOOL result = CreateAndInitRenderModule(CreateDesc, &m_pRenderSystem);
	if (!result)
	{
		// 실패 처리
		return FALSE;
	}

	RENDERCAMERA_CREATE_DESC CameraDesc = {};
	CameraDesc.CameraMode = CAMERA_MODE::PERSPECTIVE;
	CameraDesc.FOVY = 3.14159265f / 4.0f;
	CameraDesc.AspectRatio = static_cast<FLOAT>(width) / static_cast<FLOAT>(height);
	CameraDesc.NearZ = 0.1f;
	CameraDesc.FarZ = 1000.0f;
	CameraDesc.ClearColor = FLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f };
	CameraDesc.ClearFlags = static_cast<UINT32>(CLEAR_BUFFER::ALL);
	CameraDesc.Depth = 1.0f;
	CameraDesc.Stencil = 0;
	CameraDesc.CameraOrder = 0;
	m_pRenderSystem->CreateRenderCamera(CameraDesc, &m_pRenderCamera);

	PRIMITIVEBUFFER_CREATE_DESC PBDesc = {};
	PBDesc.Usage = BUFFER_USAGE::DEFAULT;
	PBDesc.VertexBufferSize = 256;
	PBDesc.IndexBufferSize = 256;

	IPrimitiveBuffer* pPrimitiveBuffer = nullptr;
	m_pRenderSystem->CreatePrimitiveBuffer(PBDesc, &pPrimitiveBuffer);

	pPrimitiveBuffer->Release();

	return TRUE;
}

void D3D11Application::OnUpdate()
{
	if (!m_pRenderSystem || !m_pRenderCamera)
		return;

	IRenderContext* pRenderContext = nullptr;
	if (!m_pRenderSystem->GetOrCreateRenderContext(&pRenderContext))
		return;

	pRenderContext->BeginFrame();
	pRenderContext->BeginRenderCamera(m_pRenderCamera);
	pRenderContext->EndRenderCamera();
	pRenderContext->EndFrame();
	pRenderContext->Release();
	
	m_pRenderSystem->Update();
	
}

void D3D11Application::OnDestroy()
{
	CHECK_RELEASE(m_pRenderCamera);
	CHECK_RELEASE(m_pRenderSystem);
	CleanupRenderModule();
}