#include "pch.h"
#include "Application.h"


Application::~Application() noexcept
{
	OnDestroy();
}

BOOL Application::OnInit(HWND hWnd, int width, int height)
{
	m_hWnd = hWnd;
	m_Width = width;
	m_Height = height;

	m_hRenderSystemModule = LoadLibrary("./RenderSystem_D3D11.dll");
	if (!m_hRenderSystemModule)
	{
		SYS_LOG_ERR("Failed to load RenderSystem_D3D11.dll");
		return FALSE;
	}

	 GetProcAddress(m_hRenderSystemModule, "CreateRenderSystem");

	return TRUE;
}

void Application::OnUpdate()
{
}

void Application::OnDestroy()
{
}
