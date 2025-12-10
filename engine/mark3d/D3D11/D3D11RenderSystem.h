#ifndef __RENDER_SYSTEM_D3D11_H__
#define __RENDER_SYSTEM_D3D11_H__

#include "IRenderSystem.h"


class D3D11RenderDevice;

class D3D11RenderSystem : public IRenderSystem
{
	DECLARATION_IUNKNOWN_INTERFACE(D3D11RenderSystem);

public:
	D3D11RenderSystem();

	virtual BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	) override;

	virtual void Shutdown() override;

	virtual BOOL CreateTexture1D(const TEXTURE1D_CREATE_DESC* pDesc, ITexture1D** ppTexture) override;

	virtual BOOL CreateTexture2D(const TEXTURE2D_CREATE_DESC* pDesc, ITexture2D** ppTexture) override;

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;

};

#endif // __RENDER_SYSTEM_D3D11_H__
