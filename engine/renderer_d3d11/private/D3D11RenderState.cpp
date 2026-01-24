#include "pch.h"
#include "D3D11RenderState.h"


D3D11RasterizerState::D3D11RasterizerState(ID3D11RasterizerState* pRasterizerState)
	: m_pD3D11RasterizerState(pRasterizerState)
	, m_HashNode{}
{
	m_HashNode.data = this;
}

D3D11RasterizerState::~D3D11RasterizerState() noexcept
{
	CHECK_RELEASE(m_pD3D11RasterizerState);
}

//------------------------------------------------------------------------------
D3D11BlendState::D3D11BlendState(ID3D11BlendState* pBlendState)
	: m_pD3D11BlendState(pBlendState)
	, m_HashNode{}
{
	m_HashNode.data = this;
}

D3D11BlendState::~D3D11BlendState() noexcept
{
	CHECK_RELEASE(m_pD3D11BlendState);
}