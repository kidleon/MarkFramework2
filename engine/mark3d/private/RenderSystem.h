#ifndef __RENDER_SYSTEM_H__
#define __RENDER_SYSTEM_H__

#include "IRenderSystem.h"


class SurfaceMaterialPool;

class RenderSystem : public IRenderSystem
{
	static RenderSystem* m_pInstance = nullptr;

public:
	RenderSystem();

	BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	) override;

	void Shutdown() override;

	BOOL CreateMaterial(ISurfaceMaterial** ppMaterial) override;
	void ReleaseMaterial(ISurfaceMaterial* pMaterial) override;

	BOOL CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture) override;
	BOOL CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture) override;

protected:
	virtual ~RenderSystem() noexcept;
	virtual void OnDestroy() override;

};


#endif // __RENDER_SYSTEM_H__
