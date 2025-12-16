#include "pch.h"
#include "RenderSystem.h"
#include "SurfaceMaterialBlockPool.h"
#include "ConstantBufferBlockPool.h"
#include "SurfaceMaterial.h"


RenderSystem* RenderSystem::m_pInstance = nullptr;

RenderSystem::RenderSystem()
{
	if (!m_pInstance)
		m_pInstance = this;
}

RenderSystem::~RenderSystem() noexcept
{
	if (m_pInstance == this)
		m_pInstance = nullptr;

	Shutdown();
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
	SurfaceMaterialBlockPool::Init();
	ConstantBufferBlockPool::Init();

	return TRUE;
}

void RenderSystem::Shutdown()
{
	SurfaceMaterialBlockPool::Shutdown();
	ConstantBufferBlockPool::Shutdown();
}

/*
BOOL RenderSystem::CreateMaterial(ISurfaceMaterial** ppMaterial)
{
	SURFACE_MATERIAL_BLOCK* pBlock = SurfaceMaterialBlockPool::Alloc();
	if (!pBlock)
	{
		(*ppMaterial) = nullptr;
		return FALSE;
	}

	SurfaceMaterial* pMaterial = MARK_POOL_NEW(SurfaceMaterial)(pBlock);
	(*ppMaterial) = static_cast<ISurfaceMaterial*>(pMaterial);

	return TRUE;
}

BOOL RenderSystem::CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture)
{
	return FALSE;
}

BOOL RenderSystem::CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture)
{
	return FALSE;
}
*/