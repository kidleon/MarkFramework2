#include "pch.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderTarget.h"


static UINT32 VIEWPORT_COUNTER = 0;

D3D11RenderCamera::D3D11RenderCamera(
	D3D11RenderTarget* pRenderTarget,
	CAMERA_MODE CameraMode
)
	: m_pRenderTarget(pRenderTarget)
	, m_CameraMode(CameraMode)
	, m_ClearTarget{ (UINT32)CLEAR_BUFFER::ALL, COLOR_BLUE, 1.0f, 0x00, 0 }
	, m_Perspective{ M_PI / 4.0f, 1.2f, 0.01f, 500.0f }
	, m_Ortho{ 1280.0f, 720.0f, 0.01f, 500.0f }
	, m_View{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }
	, m_ViewportID(VIEWPORT_COUNTER++)
{
}

D3D11RenderCamera::~D3D11RenderCamera()
{
	CHECK_RELEASE(m_pRenderTarget);
}

void D3D11RenderCamera::OnDestroy() noexcept
{
	MARK_POOL_DELETE(this, D3D11RenderCamera);
}

void D3D11RenderCamera::SetClearTarget(
	UINT32 ClearBuffers,
	const FLOAT4& ClearColor,
	FLOAT Depth,
	UINT32 Stencil
) noexcept
{
	m_ClearTarget.ClearBuffers = ClearBuffers;
	m_ClearTarget.ClearColor = ClearColor;
	m_ClearTarget.Depth = Depth;
	m_ClearTarget.Stencil = Stencil;
}

void D3D11RenderCamera::SetCameraMode(CAMERA_MODE CameraMode) noexcept
{
	m_CameraMode = CameraMode;
}

void D3D11RenderCamera::SetPerspective(
	FLOAT Fovy,
	FLOAT Aspect,
	FLOAT NearZ,
	FLOAT FarZ
) noexcept
{
	m_Perspective.Fovy = Fovy;
	m_Perspective.Aspect = Aspect;
	m_Perspective.NearZ = NearZ;
	m_Perspective.FarZ = FarZ;
}

void D3D11RenderCamera::SetOrtho(
	FLOAT ViewWidth,
	FLOAT ViewHeight,
	FLOAT NearZ,
	FLOAT FarZ
) noexcept
{
	m_Ortho.ViewWidth = ViewWidth;
	m_Ortho.ViewHeight = ViewHeight;
	m_Ortho.NearZ = NearZ;
	m_Ortho.FarZ = FarZ;
}

void D3D11RenderCamera::SetView(
	const FLOAT3& EyePos,
	const FLOAT3& EyeDir,
	const FLOAT3& EyeUp
)
{
	m_View.EyePos = EyePos;
	m_View.EyeDir = EyeDir;
	m_View.EyeUp = EyeUp;
	m_View.EyeRight = vec3_cross(&m_View.EyeDir, &m_View.EyeUp);
}

void D3D11RenderCamera::LookAt(const FLOAT3& EyePos, const FLOAT3& Target) noexcept
{
	m_View.EyePos = EyePos;
	vec3_sub((FLOAT3*)&Target, &m_View.EyePos, &m_View.EyeDir);
	vec3_normalize(&m_View.EyeDir);
	vec3_cross(&m_View.EyeDir, (FLOAT3*)&FLOAT3_UNIT_X, &m_View.EyeUp);
	vec3_cross(&m_View.EyeDir, &m_View.EyeUp, &m_View.EyeRight);
}

void D3D11RenderCamera::SetViewportLayer(INT8 Layer) noexcept
{
	m_ViewportLayer = Layer;
}