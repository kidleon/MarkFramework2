#include "pch.h"
#include "D3D11RenderState.h"


D3D11RasterizerState::D3D11RasterizerState(UINT64 Index, ID3D11RasterizerState* pRasterizerState)
	: m_pD3D11RasterizerState(pRasterizerState)
	, m_Index(Index)
	, m_HashNode{}
{
	m_HashNode.data = this;
}

D3D11RasterizerState::~D3D11RasterizerState() noexcept
{
	CHECK_RELEASE(m_pD3D11RasterizerState);
}

//------------------------------------------------------------------------------
D3D11BlendState::D3D11BlendState(UINT64 Index, ID3D11BlendState* pBlendState)
	: m_pD3D11BlendState(pBlendState)
	, m_Index(Index)
	, m_HashNode{}
{
	m_HashNode.data = this;
}

D3D11BlendState::~D3D11BlendState() noexcept
{
	CHECK_RELEASE(m_pD3D11BlendState);
}

//------------------------------------------------------------------------------
D3D11DepthStencilState::D3D11DepthStencilState(UINT64 Index, ID3D11DepthStencilState* pDepthStencilState)
	: m_pD3D11DepthStencilState(pDepthStencilState)
	, m_Index(Index)
	, m_HashNode{}
{
	m_HashNode.data = this;
}

D3D11DepthStencilState::~D3D11DepthStencilState() noexcept
{
	CHECK_RELEASE(m_pD3D11DepthStencilState);
}