#include "pch.h"
#include "RenderSystem.h"
#include "SurfaceMaterialPool.h"


RenderSystem::RenderSystem()
{
	if (!m_pInstance)
		m_pInstance = this;
}

RenderSystem::~RenderSystem() noexcept
{
	if (m_pInstance == this)
		m_pInstance = nullptr;
	SurfaceMaterialPool::Shutdown();
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
	SurfaceMaterialPool::Init(128);

	return TRUE;
}

void RenderSystem::Shutdown()
{

}

BOOL RenderSystem::CreateMaterial(ISurfaceMaterial** ppMaterial)
{
	return FALSE;
}

void RenderSystem::ReleaseMaterial(ISurfaceMaterial* pMaterial)
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