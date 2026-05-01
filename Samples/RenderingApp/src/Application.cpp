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

	mark::EngineCreateDesc CreateDesc = {};

	CreateDesc.ScreenWidth = static_cast<uint32_t>(width);
	CreateDesc.ScreenHeight = static_cast<uint32_t>(height);
	CreateDesc.WindowHandle = m_hWnd;

	if (!CreateMark3DEngine(CreateDesc, &m_pMark3D))
		return FALSE;


	return TRUE;
}

void Application::OnUpdate()
{
}

void Application::OnDestroy()
{
	CHECK_RELEASE(m_pMark3D);
}
