#ifndef __D3D11_RENDER_STATE_H__
#define __D3D11_RENDER_STATE_H__

#include "IUnknownImpl.h"


struct D3D11BlendState : public IUNKNOWN_HASH_IMPL
{
	ID3D11BlendState* pD3D11BlendState = nullptr;

	D3D11BlendState() noexcept;
	~D3D11BlendState() noexcept;
};

struct D3D11SamplerState : public IUNKNOWN_HASH_IMPL
{
	ID3D11SamplerState* pD3D11SamplerState = nullptr;

	D3D11SamplerState() noexcept;
	~D3D11SamplerState() noexcept;
};

struct D3D11DepthStencilState : public IUNKNOWN_HASH_IMPL
{
	ID3D11DepthStencilState* pD3D11DepthStencilState = nullptr;

	D3D11DepthStencilState() noexcept;
	~D3D11DepthStencilState() noexcept;
};

struct D3D11RasterizerState : public IUNKNOWN_HASH_IMPL
{
	ID3D11RasterizerState* pD3D11RasterizerState = nullptr;

	D3D11RasterizerState() noexcept;
	~D3D11RasterizerState() noexcept;
};


#endif // __D3D11_RENDER_STATE_H__
