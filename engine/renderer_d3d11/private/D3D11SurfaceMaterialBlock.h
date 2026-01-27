#pragma once


class D3D11ShaderProgram;

constexpr size_t MAX_RENDER_PASS_NAME = 64;
struct D3D11_SURFACE_RENDER_PASS
{
	char PassName[MAX_RENDER_PASS_NAME];
	D3D11ShaderProgram* pVertexShader;
	D3D11ShaderProgram* pPixelShader;

	ID3D11RasterizerState* pRasterizerState;
	UINT64 RasterizerStateHash;

	ID3D11BlendState* pBlendState;
	UINT64 BlendStateHash;
	FLOAT4 BlendFactor;

	ID3D11DepthStencilState* pDepthStencilState;
	UINT64 DepthStencilStateHash;

	UINT16 RasterizerStateIndex;
	UINT16 BlendStateIndex;
	UINT16 DepthStencilStateIndex;
	UINT16 SamplerStateIndex;

	UINT32 SampleMask;
	UINT32 StencilRef;

	FLOAT4 Color;
};

struct D3D11_SURFACE_MATERIAL_BLOCK
{
	D3D11_SURFACE_RENDER_PASS RenderPasses[MAX_RENDER_PASS];
	size_t NumPasses;

	LINK_NODE LinkNode;
};