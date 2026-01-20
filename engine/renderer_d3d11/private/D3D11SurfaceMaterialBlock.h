#pragma once


class D3D11ShaderProgram;

struct D3D11_SURFACE_RENDER_PASS
{
	D3D11ShaderProgram* pVertexShader;
	D3D11ShaderProgram* pPixelShader;

	FLOAT4 Color;
};

struct D3D11_SURFACE_MATERIAL_BLOCK
{
	D3D11_SURFACE_RENDER_PASS RenderPasses[MAX_RENDER_PASS];
	size_t NumPasses;

	LINK_NODE LinkNode;
};