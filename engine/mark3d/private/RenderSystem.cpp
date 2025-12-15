#include "pch.h"
#include "RenderSystem.h"
#include "SurfaceMaterialPool.h"


RenderSystem::~RenderSystem() noexcept
{

}

void RenderSystem::OnDestroy()
{
	delete this;
}

BOOL RenderSystem::Initialize(
	HWND hWnd,
	uint32 ScreenWidth,
	uint32 ScreenHeight,
	BOOL Fullscreen
)
{
	m_pSurfaceMaterialPool = new SurfaceMaterialPool();
	if (!m_pSurfaceMaterialPool)
	{
		delete m_pSurfaceMaterialPool;
		return FALSE;
	}
	m_pSurfaceMaterialPool->Init(128);

	return TRUE;
}

void RenderSystem::Shutdown()
{

}

BOOL RenderSystem::CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture)
{
	return FALSE;
}

BOOL RenderSystem::CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture)
{
	return FALSE;
}