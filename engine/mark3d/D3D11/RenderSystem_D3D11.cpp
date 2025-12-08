#include "pch.h"
#include "RenderSystem_D3D11.h"

IMPLEMENTATION_IUNKNOWN_INTERFACE(RenderSystem_D3D11);

RenderSystem_D3D11::RenderSystem_D3D11()
{
}

RenderSystem_D3D11::~RenderSystem_D3D11() noexcept
{
	Shutdown();
}

BOOL RenderSystem_D3D11::Initialize(
	HWND hWnd,
	uint32 width,
	uint32 height,
	BOOL fullscreen
)
{
	return TRUE;
}

void RenderSystem_D3D11::Shutdown()
{
}

void RenderSystem_D3D11::OnDestroy()
{
	delete this;
}


