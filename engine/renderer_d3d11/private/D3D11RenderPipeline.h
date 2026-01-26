#pragma once


class D3D11ShaderProgram;

struct D3D11_RENDER_PIPELINE
{
	D3D11ShaderProgram* pVertexShader;
	D3D11ShaderProgram* pPixelShader;
	ID3D11RasterizerState* pRasterizerState;
	ID3D11BlendState* pBlendState;
	ID3D11DepthStencilState* pDepthStencilState;
	FLOAT4 Color;
};

struct D3D11_DYNAMIC_RENDER_PIPELINE
{
	FLOAT4 BlendFactor;
	UINT32 SampleMask;
	UINT32 StencilRef;
};