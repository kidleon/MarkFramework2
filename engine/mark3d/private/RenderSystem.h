#ifndef __RENDER_SYSTEM_H__
#define __RENDER_SYSTEM_H__

#include "IRenderSystem.h"


class RenderSystem final : public IRenderSystem
{
	static RenderSystem* m_pInstance;

public:
	RenderSystem();

	// IUNKNOWN interface
	long AddRef() final;
	long Release() final;
	long RefCnt() final;

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
	
private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

};

#endif // __RENDER_SYSTEM_H__
