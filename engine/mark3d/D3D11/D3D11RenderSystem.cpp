#include "pch.h"
#include "D3D11RenderSystem.h"

IMPLEMENTATION_IUNKNOWN_INTERFACE(D3D11RenderSystem);


D3D11RenderSystem::D3D11RenderSystem()
{
}

D3D11RenderSystem::~D3D11RenderSystem() noexcept
{
	Shutdown();
}

BOOL D3D11RenderSystem::Initialize(
	HWND hWnd,
	uint32 width,
	uint32 height,
	BOOL fullscreen
)
{
	return TRUE;
}

void D3D11RenderSystem::Shutdown()
{
}

void D3D11RenderSystem::OnDestroy()
{
	delete this;
}

BOOL D3D11RenderSystem::CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture)
{
	return TRUE;
}

BOOL D3D11RenderSystem::CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture)
{
	return TRUE;
}

