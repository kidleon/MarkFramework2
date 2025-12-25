#ifndef __D3D11_RENDER_SYSTEM_H__
#define __D3D11_RENDER_SYSTEM_H__

#include "RenderSystem.h"


interface IRenderCamera;
class D3D11RenderDevice;

class D3D11RenderSystem final : public RenderSystem
{
public:
	virtual BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		int32 MinLogLevel,
		BOOL Fullscreen
	) final;

	virtual void Shutdown() final;

	virtual BOOL CreateRenderCamera(
		const RENDERCAMERA_CREATE_DESC& Desc,
		IRenderCamera** ppOut
	) final;

private:
	virtual ~D3D11RenderSystem() noexcept;

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;

};


#endif // __D3D11_RENDER_SYSTEM_H__
