#ifndef __RENDER_DEVICE_D3D11_H__
#define __RENDER_DEVICE_D3D11_H__


class D3D11RenderTarget;

class D3D11RenderDevice
{
	static D3D11RenderDevice* s_pInstance;

public:
	D3D11RenderDevice();
	~D3D11RenderDevice() noexcept;

	BOOL CreateDevice(HWND hWnd, uint32 Width, uint32 Height, BOOL DebugDevice);
	BOOL CreateInputLayout(const D3D11_INPUTLAYOUT_DESC& Desc, ID3D11InputLayout** ppOut);
	BOOL CreateRasterizerState(const RS_RASTERIZER_STATE& RasterizerState, ID3D11RasterizerState** ppOut);
	BOOL CreateBlendState(const RS_BLEND_STATE& BlendState, ID3D11BlendState** ppOut);
	BOOL CreateDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState, ID3D11DepthStencilState** ppOut);

	__FORCEINLINE ID3D11Device* INL_GetD3D11Device() const noexcept { return m_pD3D11Device; }
	__FORCEINLINE ID3D11DeviceContext* INL_GetD3D11Context() const noexcept { return m_pImmediateContext; }
	__FORCEINLINE IDXGISwapChain* INL_GetSwapChain() const noexcept { return m_pSwapChain; }

	__FORCEINLINE ID3D11RenderTargetView* INL_GetBackBuffer_RenderTargetView() const noexcept { return m_pRenderTargetView; }
	__FORCEINLINE ID3D11DepthStencilView* INL_GetBackBuffer_DepthStencilView() const noexcept { return m_pDepthStencilView; }
	__FORCEINLINE ID3D11Texture2D* INL_GetBackBuffer_DepthStencilTexture() const noexcept { return m_pDepthStencilTexture; }

	__FORCEINLINE D3D11RenderTarget* INL_GetBackBuffer_RenderTarget() const noexcept { return m_pBackBuffer_RT; }

	static inline D3D11RenderDevice& Get() noexcept { return *s_pInstance; }

private:
	void DestroyDevice() noexcept;
	void ReportLiveObjects() noexcept;

private:
	HWND m_hWnd = nullptr;
	D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	ID3D11Device* m_pD3D11Device = nullptr;
	ID3D11Device1* m_pD3D11Device1 = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	ID3D11DeviceContext1* m_pImmediateContext1 = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	IDXGISwapChain1* m_pSwapChain1 = nullptr;

	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

	D3D11RenderTarget* m_pBackBuffer_RT = nullptr;
};


#endif // __RENDER_DEVICE_D3D11_H__
