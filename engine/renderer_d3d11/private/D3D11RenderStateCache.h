#pragma once


class D3D11RenderStateCache
{
	static D3D11RenderStateCache* s_pInstance;

public:
	D3D11RenderStateCache();
	~D3D11RenderStateCache() noexcept;

	void Init();
	void Shutdown();

	ID3D11RasterizerState* Register(const RS_RASTERIZER_STATE& RasterizerState) noexcept;
	void Register(const RS_BLEND_STATE& BlendState) noexcept;

	ID3D11RasterizerState* Find(const RS_RASTERIZER_STATE& RasterizerState) noexcept;
	ID3D11RasterizerState* Find(uint64 Hash) noexcept;

	ID3D11BlendState* Find(const RS_BLEND_STATE& BlendState) noexcept;

	inline static D3D11RenderStateCache* Get() { return s_pInstance; }

private:
	HASH_TABLE* m_pRasterizerStateCache;
	HASH_TABLE* m_pBlendStateCache;

};
