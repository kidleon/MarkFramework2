#ifndef __D3D11_RENDER_STATE_H__
#define __D3D11_RENDER_STATE_H__


struct D3D11StateBase : public IUNKNOWN_HASH_IMPL
{
	uint64 Hash;
	UINT16 StateIndex;
	UINT8 PADDING[6];
};

struct D3D11BlendState : public D3D11StateBase
{
	ID3D11BlendState* pD3D11BlendState = nullptr;

	D3D11BlendState() noexcept;
	~D3D11BlendState() noexcept;

protected:
	void OnDestroy() final;

};

struct D3D11SamplerState : public D3D11StateBase
{
	ID3D11SamplerState* pD3D11SamplerState = nullptr;

	D3D11SamplerState() noexcept;
	~D3D11SamplerState() noexcept;

protected:
	void OnDestroy() final;

};

struct D3D11DepthStencilState : public D3D11StateBase
{
	ID3D11DepthStencilState* pD3D11DepthStencilState = nullptr;

	D3D11DepthStencilState() noexcept;
	~D3D11DepthStencilState() noexcept;

protected:
	void OnDestroy() final;

};

struct D3D11RasterizerState : public D3D11StateBase
{
	ID3D11RasterizerState* pD3D11RasterizerState = nullptr;

	D3D11RasterizerState() noexcept;
	~D3D11RasterizerState() noexcept;

protected:
	void OnDestroy() final;

};

struct RENDER_PIPELINE_STATE_KEY
{
	union
	{
		uint64 RenderStateID = 0;
		struct
		{
			uint64 BlendStateIndex : 16; // 0 ~ 65535
			uint64 RasterizerStateIndex : 16; // 0 ~ 65535
			uint64 DepthStencilStateIndex : 16; // 0 ~ 65535
			uint64 RESERVED : 16;
		};
	};
};

struct D3D11RenderPipelineState : public D3D11StateBase
{
	D3D11BlendState* pBlendState = nullptr;
	D3D11RasterizerState* pRasterizerState = nullptr;
	D3D11DepthStencilState* pDepthStencilState = nullptr;

	D3D11RenderPipelineState() noexcept;
	~D3D11RenderPipelineState() noexcept;

protected:
	void OnDestroy() final;
};

struct D3D11DynamicRenderPipelineState
{
	D3D11SamplerState* pSamplerState = nullptr;


};



#endif // __D3D11_RENDER_STATE_H__
