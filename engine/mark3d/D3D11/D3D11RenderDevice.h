#ifndef __RENDER_DEVICE_D3D11_H__
#define __RENDER_DEVICE_D3D11_H__


struct D3D11VertexShader;
struct D3D11PixelShader;
struct D3D11ComputeShader;
struct D3D11InputLayout;
struct D3D11SamplerState;
struct D3D11BlendState;
struct D3D11RasterizerState;
struct D3D11DepthStencilState;
//class D3D11InputLayoutCache;

class D3D11RenderDevice
{
public:
	D3D11RenderDevice() = default;
	~D3D11RenderDevice() noexcept;

	BOOL CreateDevice(HWND hWnd, uint32 Width, uint32 Height, BOOL DebugDevice);
	BOOL CreateBuffer(const D3D11_BUFFER_DESC* pDesc, ID3D11Buffer** ppOut);
	/*
	BOOL CreateVertexShader(const D3D11_SHADER_COMPILE_DESC& Desc, D3D11VertexShader** ppOut);
	BOOL CreatePixelShader(const D3D11_SHADER_COMPILE_DESC& Desc, D3D11PixelShader** ppOut);
	BOOL CreateInputLayout(const D3D11_INPUTLAYOUT_DESC& Desc, D3D11InputLayout** ppOut);

	
	BOOL CreateSamplerState(const RS_SAMPLER_STATE& Desc, D3D11SamplerState** ppOut);
	BOOL CreateBlendState(const RS_BLEND_STATE& Desc, D3D11BlendState** ppOut);
	BOOL CreateRasterizerState(const RS_RASTERIZER_STATE& Desc, D3D11RasterizerState** ppOut);
	BOOL CreateDepthStencilState(const RS_DEPTH_STENCIL_STATE& Desc, D3D11DepthStencilState** ppOut);
	*/

	__FORCEINLINE ID3D11Device* INL_GetD3D11Device() const noexcept
	{
		return m_pD3D11Device;
	}

private:
	void DestroyDevice() noexcept;

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

//	D3D11InputLayoutCache* m_pInputLayoutCache = nullptr;

};


#endif // __RENDER_DEVICE_D3D11_H__
