#pragma once


class D3D11ShaderProgram;

struct D3D11_RENDER_PIPELINE
{
	D3D11ShaderProgram* pVertexShader;
	D3D11ShaderProgram* pPixelShader;
	FLOAT4 Color;
};

struct D3D11_DYNAMIC_RENDER_PIPELINE
{
	FLOAT4 BlendFactor;
	uint32 SampleMask;
	uint32 PADDING;
};