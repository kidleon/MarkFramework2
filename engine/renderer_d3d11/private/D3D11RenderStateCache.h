#pragma once


class D3D11RasterizerState;
class D3D11BlendState;
class D3D11DepthStencilState;

class D3D11RenderStateCache
{
	static D3D11RenderStateCache* s_pInstance;

public:
	D3D11RenderStateCache();
	~D3D11RenderStateCache() noexcept;

	void Init();
	void Shutdown();

	D3D11RasterizerState* Register(const RS_RASTERIZER_STATE& RasterizerState) noexcept;
	D3D11BlendState* Register(const RS_BLEND_STATE& BlendState) noexcept;
	D3D11DepthStencilState* Register(const RS_DEPTH_STENCIL_STATE& DepthStencilState) noexcept;

	D3D11RasterizerState* Find_RS(const RS_RASTERIZER_STATE& RasterizerState) noexcept;
	D3D11RasterizerState* Find_RS(uint64 Hash) noexcept;

	D3D11BlendState* Find_BS(const RS_BLEND_STATE& BlendState) noexcept;
	D3D11BlendState* Find_BS(uint64 Hash) noexcept;

	D3D11DepthStencilState* Find_DSS(const RS_DEPTH_STENCIL_STATE& DepthStencilState) noexcept;
	D3D11DepthStencilState* Find_DSS(uint64 Hash) noexcept;

	inline static D3D11RenderStateCache* Get() { return s_pInstance; }

private:
	HASH_TABLE* m_pRasterizerStateCache;
	HASH_TABLE* m_pBlendStateCache;
	HASH_TABLE* m_pDepthStencilStateCache;
	HASH_TABLE* m_pSamplerStateCache;

};
