#ifndef __RENDER_SYSTEM_D3D11_H__
#define __RENDER_SYSTEM_D3D11_H__

#include "IRenderSystem.h"


class RenderSystem_D3D11 : public IRenderSystem
{
	DECLARATION_IUNKNOWN_INTERFACE(RenderSystem_D3D11);

public:
	RenderSystem_D3D11();

	virtual BOOL Initialize(
		HWND hWnd,
		uint32 width,
		uint32 height,
		BOOL fullscreen
	) override;

	virtual void Shutdown() override;

};

#endif // __RENDER_SYSTEM_D3D11_H__
