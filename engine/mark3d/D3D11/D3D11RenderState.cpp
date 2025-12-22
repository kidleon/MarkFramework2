#include "pch.h"
#include "D3D11RenderState.h"


D3D11BlendState::D3D11BlendState() noexcept
{
	HASH_NODE* pNode = INL_GetHashNode();
	pNode->data = this;
}

D3D11BlendState::~D3D11BlendState() noexcept
{
	if (pD3D11BlendState)
	{
		pD3D11BlendState->Release();
		pD3D11BlendState = nullptr;
	}
}

void D3D11BlendState::OnDestroy() 
{
	MARK_POOL_DELETE(this, D3D11BlendState);
}


D3D11SamplerState::D3D11SamplerState() noexcept
{
	HASH_NODE* pNode = INL_GetHashNode();
	pNode->data = this;
}

D3D11SamplerState::~D3D11SamplerState() noexcept
{
	if (pD3D11SamplerState)
	{
		pD3D11SamplerState->Release();
		pD3D11SamplerState = nullptr;
	}
}

void D3D11SamplerState::OnDestroy() 
{
	MARK_POOL_DELETE(this, D3D11SamplerState);
}


D3D11DepthStencilState::D3D11DepthStencilState() noexcept
{
	HASH_NODE* pNode = INL_GetHashNode();
	pNode->data = this;
}

D3D11DepthStencilState::~D3D11DepthStencilState() noexcept
{
	if (pD3D11DepthStencilState)
	{
		pD3D11DepthStencilState->Release();
		pD3D11DepthStencilState = nullptr;
	}
}

void D3D11DepthStencilState::OnDestroy() 
{
	MARK_POOL_DELETE(this, D3D11DepthStencilState);
}


D3D11RasterizerState::D3D11RasterizerState() noexcept
{
	HASH_NODE* pNode = INL_GetHashNode();
	pNode->data = this;
}

D3D11RasterizerState::~D3D11RasterizerState() noexcept
{
	if (pD3D11RasterizerState)
	{
		pD3D11RasterizerState->Release();
		pD3D11RasterizerState = nullptr;
	}
}

void D3D11RasterizerState::OnDestroy() 
{
	MARK_POOL_DELETE(this, D3D11RasterizerState);
}