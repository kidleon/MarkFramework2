#ifndef __D3D11_RENDER_STATE_CACHE_H__
#define __D3D11_RENDER_STATE_CACHE_H__


struct D3D11BlendState;
struct D3D11SamplerState;
struct D3D11DepthStencilState;
struct D3D11RasterizerState;

class D3D11RenderStateCache
{
public:
	D3D11RenderStateCache() = default;
	~D3D11RenderStateCache() noexcept;

	void Init();
	void Shutdown();

	void Add(uint64 Hash, D3D11BlendState* pState);
	void Add(uint64 Hash, D3D11SamplerState* pState);
	void Add(uint64 Hash, D3D11DepthStencilState* pState);
	void Add(uint64 Hash, D3D11RasterizerState* pState);

	BOOL TryGet(uint64 Hash, D3D11BlendState** ppOut);
	BOOL TryGet(uint64 Hash, D3D11SamplerState** ppOut);
	BOOL TryGet(uint64 Hash, D3D11DepthStencilState** ppOut);
	BOOL TryGet(uint64 Hash, D3D11RasterizerState** ppOut);

private:
	HASH_TABLE* m_pCache_BlendState = nullptr;
	HASH_TABLE* m_pCache_SamplerState = nullptr;
	HASH_TABLE* m_pCache_DepthStencilState = nullptr;
	HASH_TABLE* m_pCache_RasterizerState = nullptr;

};




#endif // __D3D11_RENDER_STATE_CACHE_H__
