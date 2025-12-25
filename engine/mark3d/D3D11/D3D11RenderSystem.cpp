#include "pch.h"
#include "D3D11RenderSystem.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderTarget.h"


D3D11RenderSystem::~D3D11RenderSystem() noexcept
{
	Shutdown();
}

BOOL D3D11RenderSystem::Initialize(
	HWND hWnd,
	uint32 ScreenWidth,
	uint32 ScreenHeight,
	int32 MinLogLevel,
	BOOL Fullscreen
)
{
	BOOL DebugDevice = FALSE;

#if defined(_DEBUG) || defined(DEBUG)
	DebugDevice = TRUE;
#endif // defined(_DEBUG)

	m_pRenderDevice = MARK_NEW(D3D11RenderDevice)();

	if (!m_pRenderDevice->CreateDevice(hWnd, ScreenWidth, ScreenHeight, DebugDevice))
	{
		SYS_LOG_E("D3D11RenderSystem::Initialize: Failed to create D3D11RenderDevice");
		MARK_DELETE(m_pRenderDevice, D3D11RenderDevice);

		return FALSE;
	}

	return TRUE;
}

void D3D11RenderSystem::Shutdown()
{
	if (m_pRenderDevice)
	{
		MARK_DELETE(m_pRenderDevice, D3D11RenderDevice);
		m_pRenderDevice = nullptr;
	}
}

BOOL D3D11RenderSystem::CreateRenderCamera(
	const RENDERCAMERA_CREATE_DESC& Desc,
	IRenderCamera** ppOut
)
{
	D3D11RenderTarget* pRenderTarget = m_pRenderDevice->INL_GetBackBuffer_RenderTarget();
	pRenderTarget->AddRef();

	D3D11RenderCamera* pRenderCamera = MARK_POOL_NEW(D3D11RenderCamera)(
		pRenderTarget,
		Desc.CameraMode
	);

	pRenderCamera->SetCameraMode(Desc.CameraMode);
	if (Desc.CameraMode == CAMERA_MODE::PERSPECTIVE)
	{
		pRenderCamera->SetPerspective(
			Desc.FOVY,
			Desc.AspectRatio,
			Desc.NearZ,
			Desc.FarZ
		);
	}
	else if (Desc.CameraMode == CAMERA_MODE::ORTHO)
	{
		pRenderCamera->SetOrtho(
			Desc.OrthoWidth,
			Desc.OrthoHeight,
			Desc.NearZ,
			Desc.FarZ
		);
	}

	pRenderCamera->SetView(
		FLOAT3{ 0.0f, 0.0f, 0.0f },
		FLOAT3{ 0.0f, 0.0f, -1.0f },
		FLOAT3{ 0.0f, 1.0f, 0.0f }
	);

	pRenderCamera->SetClearTarget(
		Desc.ClearFlags,
		Desc.ClearColor,
		Desc.Depth,
		Desc.Sencil
	);

	pRenderCamera->SetViewportLayer(Desc.CameraOrder);

	*ppOut = pRenderCamera;

	return TRUE;
}