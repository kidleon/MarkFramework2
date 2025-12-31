#ifndef __D3D11_RENDER_SYSTEM_H__
#define __D3D11_RENDER_SYSTEM_H__


interface IRenderCamera;
class D3D11RenderDevice;
class D3D11RenderManager;
class D3D11RenderContext;

class D3D11RenderSystem final : public IRenderSystem
{
public:
	// IUNKNOWN interface
	long AddRef() final;
	long Release() final;
	long RefCnt() final;

	virtual BOOL Initialize(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	) final;

	virtual void Shutdown() final;

	virtual BOOL CreateRenderCamera(const RENDERCAMERA_CREATE_DESC& Desc, IRenderCamera** ppOut) final;
	virtual BOOL CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& Desc, IPrimitiveBuffer** ppOut) final;

	IRenderContext* GetRenderContext() const noexcept final;

private:
	virtual ~D3D11RenderSystem() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	D3D11RenderDevice* m_pRenderDevice = nullptr;
	D3D11RenderManager* m_pRenderMgr = nullptr;
	D3D11RenderContext* m_pRenderContext = nullptr;
	
};


#endif // __D3D11_RENDER_SYSTEM_H__
