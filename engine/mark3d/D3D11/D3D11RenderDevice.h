#ifndef __RENDER_DEVICE_D3D11_H__
#define __RENDER_DEVICE_D3D11_H__

#include "RenderState.h"
#include "TRenderState.h"


struct D3D11VertexShader;
struct D3D11PixelShader;
struct D3D11ComputeShader;
struct D3D11SamplerState;
struct D3D11BlendState;
struct D3D11RasterizerState;
struct D3D11DepthStencilState;

class D3D11RenderStateCache;
class D3D11InputLayoutCache;
class D3D11InputLayout;
class D3D11ConstantBufferPool;
class D3D11ConstantBuffer;
class D3D11ShaderCache;
class D3D11Shader;
class D3D11Texture1D;
class D3D11Texture2D;
class D3D11RenderTarget;

class D3D11RenderDevice
{
public:
	D3D11RenderDevice() = default;
	~D3D11RenderDevice() noexcept;

	BOOL CreateDevice(HWND hWnd, uint32 Width, uint32 Height, BOOL DebugDevice);

	BOOL CreateBuffer(const D3D11_BUFFER_DESC* pDesc, ID3D11Buffer** ppBuffer);

	BOOL CreateConstantBuffer(size_t BufferSize, D3D11ConstantBuffer** ppCB);
	void ReleaseConstantBuffer(D3D11ConstantBuffer** ppCB);

	BOOL GetOrCreateShader(
		const D3D11_SHADER_COMPILE_DESC* pDesc,
		D3D11Shader** ppShader
	);

	BOOL GetOrCreateInputLayout(
		const D3D11_INPUTLAYOUT_DESC* pDesc,
		UINT NumElements,
		D3D11InputLayout** ppIL
	);

	BOOL FillTexture1D(
		const D3D11_TEXTURE1D_CREATE_DESC* pDesc,
		D3D11Texture1D* pTexture1D
	);

	BOOL FillTexture2D(
		const D3D11_TEXTURE2D_CREATE_DESC* pDesc,
		D3D11Texture2D* pTexture2D
	);

	BOOL CreateRenderTarget(
		const D3D11_RENDERTARGET_CREATE_DESC* pDesc,
		D3D11RenderTarget** ppRT
	);

	BOOL GetOrCreateSamplerState(const RS_SAMPLER_STATE& Desc, D3D11SamplerState** ppOut);
	BOOL GetOrCreateSamplerState(const TRenderState<RS_SAMPLER_STATE>& TDesc, D3D11SamplerState** ppOut);
	BOOL GetOrCreateSamplerState(uint64 Hash, const RS_SAMPLER_STATE& Desc, D3D11SamplerState** ppOut);

	BOOL GetOrCreateBlendState(const RS_BLEND_STATE& Desc, D3D11BlendState** ppOut);
	BOOL GetOrCreateBlendState(const TRenderState<RS_BLEND_STATE>& TDesc, D3D11BlendState** ppOut);
	BOOL GetOrCreateBlendState(uint64 Hash, const RS_BLEND_STATE& Desc, D3D11BlendState** ppOut);

	BOOL GetOrCreateRasterizerState(const RS_RASTERIZER_STATE& Desc, D3D11RasterizerState** ppOut);
	BOOL GetOrCreateRasterizerState(const TRenderState<RS_RASTERIZER_STATE>& TDesc, D3D11RasterizerState** ppOut);
	BOOL GetOrCreateRasterizerState(uint64 Hash, const RS_RASTERIZER_STATE& Desc, D3D11RasterizerState** ppOut);

	BOOL GetOrCreateDepthStencilState(const RS_DEPTH_STENCIL_STATE& Desc, D3D11DepthStencilState** ppOut);
	BOOL GetOrCreateDepthStencilState(const TRenderState<RS_DEPTH_STENCIL_STATE>& TDesc, D3D11DepthStencilState** ppOut);
	BOOL GetOrCreateDepthStencilState(uint64 Hash, const RS_DEPTH_STENCIL_STATE& Desc, D3D11DepthStencilState** ppOut);

	__FORCEINLINE ID3D11Device* INL_GetD3D11Device() const noexcept { return m_pD3D11Device; }
	__FORCEINLINE ID3D11DeviceContext* INL_GetD3D11Context() const noexcept { return m_pImmediateContext; }
	__FORCEINLINE IDXGISwapChain* INL_GetSwapChain() const noexcept { return m_pSwapChain; }

	__FORCEINLINE ID3D11RenderTargetView* INL_GetBackBuffer_RenderTargetView() const noexcept { return m_pRenderTargetView; }
	__FORCEINLINE ID3D11DepthStencilView* INL_GetBackBuffer_DepthStencilView() const noexcept { return m_pDepthStencilView; }
	__FORCEINLINE ID3D11Texture2D* INL_GetBackBuffer_DepthStencilTexture() const noexcept { return m_pDepthStencilTexture; }

	__FORCEINLINE D3D11RenderTarget* INL_GetBackBuffer_RenderTarget() const noexcept { return m_pBackBuffer_RT; }

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

	D3D11RenderTarget* m_pBackBuffer_RT = nullptr;

	D3D11InputLayoutCache* m_pInputLayoutCache = nullptr;
	D3D11ConstantBufferPool* m_pConstantBufferPool = nullptr;
	D3D11ShaderCache* m_pShaderCache = nullptr;
	D3D11RenderStateCache* m_pRenderStateCache = nullptr;

	volatile UINT16 m_VertexShaderIndex = 0;
	volatile UINT16 m_PixelShaderIndex = 0;
	volatile UINT16 m_ComputeShaderIndex = 0;
	volatile UINT8 m_BlendStateIndex = 0;
	volatile UINT8 m_SamplerStateIndex = 0;
	volatile UINT8 m_RasterizerStateIndex = 0;
	volatile UINT8 m_DepthStencilStateIndex = 0;
};


#endif // __RENDER_DEVICE_D3D11_H__
