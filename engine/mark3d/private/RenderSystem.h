#ifndef __RENDER_SYSTEM_H__
#define __RENDER_SYSTEM_H__

#include "IRenderSystem.h"


class SurfaceMaterialPool;

class RenderSystem : public IRenderSystem
{
	static RenderSystem* m_pInstance;

public:
	RenderSystem();

	BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	) override;

	void Shutdown() override;

protected:
	virtual ~RenderSystem() noexcept;
	virtual void OnDestroy() override;

};


#endif // __RENDER_SYSTEM_H__
