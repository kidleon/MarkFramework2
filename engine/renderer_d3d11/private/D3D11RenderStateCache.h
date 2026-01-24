#pragma once


class D3D11RenderStateCache
{
	static D3D11RenderStateCache* s_pInstance;

public:
	D3D11RenderStateCache();
	~D3D11RenderStateCache() noexcept;

	void Init();
	void Shutdown();

	void Register(const RS_RASTERIZER_STATE& RasterizerState) noexcept;
	void Register(const RS_BLEND_STATE& BlendState) noexcept;

	ID3D11RasterizerState* Find(const RS_RASTERIZER_STATE& RasterizerState) noexcept;
	ID3D11BlendState* Find(const RS_BLEND_STATE& BlendState) noexcept;

private:
	HASH_TABLE* m_pRasterizerStateCache;
	HASH_TABLE* m_pBlendStateCache;

};
