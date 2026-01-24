#pragma once


class D3D11RasterizerState
{
public:
	explicit D3D11RasterizerState(ID3D11RasterizerState* pRasterizerState);
	~D3D11RasterizerState() noexcept;

	__FORCEINLINE ID3D11RasterizerState* INL_GetD3D11RasterizerState() const noexcept { return m_pD3D11RasterizerState; }
	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept { return &m_HashNode; }

private:
	D3D11RasterizerState() = delete;
	D3D11RasterizerState(const D3D11RasterizerState&) = delete;
	D3D11RasterizerState& operator=(const D3D11RasterizerState&) = delete;

private:
	ID3D11RasterizerState* m_pD3D11RasterizerState;
	HASH_NODE m_HashNode;
};


//------------------------------------------------------------------------------
class D3D11BlendState
{
public:
	explicit D3D11BlendState(ID3D11BlendState* pBlendState);
	~D3D11BlendState() noexcept;

	__FORCEINLINE ID3D11BlendState* INL_GetD3D11BlendState() const noexcept { return m_pD3D11BlendState; }
	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept { return &m_HashNode; }

private:
	D3D11BlendState() = delete;
	D3D11BlendState(const D3D11BlendState&) = delete;
	D3D11BlendState& operator=(const D3D11BlendState&) = delete;

private:
	ID3D11BlendState* m_pD3D11BlendState;
	HASH_NODE m_HashNode;

};