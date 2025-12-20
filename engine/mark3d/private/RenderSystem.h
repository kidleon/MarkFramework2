#ifndef __RENDER_SYSTEM_H__
#define __RENDER_SYSTEM_H__

#include "IRenderSystem.h"


class RenderSystem final : public IRenderSystem
{
	static RenderSystem* m_pInstance;

public:
	RenderSystem();

	BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		int32 MinLogLevel,
		BOOL Fullscreen
	) final;

	void Shutdown() final;

protected:
	virtual ~RenderSystem() noexcept;
	virtual void OnDestroy() final;

};

#endif // __RENDER_SYSTEM_H__
