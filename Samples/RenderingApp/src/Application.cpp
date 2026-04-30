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

	return TRUE;
}

void Application::OnUpdate()
{
}

void Application::OnDestroy()
{
}
