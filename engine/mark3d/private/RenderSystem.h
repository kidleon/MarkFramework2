#ifndef __RENDER_SYSTEM_H__
#define __RENDER_SYSTEM_H__

#include "IRenderSystem.h"


class SurfaceMaterialPool;

class RenderSystem : public IRenderSystem
{
public:
	RenderSystem() = default;

	BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	) override;

	void Shutdown() override;

	BOOL CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture) override;
	BOOL CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture) override;

protected:
	virtual ~RenderSystem() noexcept;
	virtual void OnDestroy() override;

private:
	SurfaceMaterialPool* m_pSurfaceMaterialPool = nullptr;

};


#endif // __RENDER_SYSTEM_H__
