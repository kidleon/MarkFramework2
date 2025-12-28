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

	return TRUE;
}

void D3D11Application::OnUpdate()
{

}

void D3D11Application::OnDestroy()
{
	CHECK_RELEASE(m_pRenderSystem);
	CleanupRenderModule();
}