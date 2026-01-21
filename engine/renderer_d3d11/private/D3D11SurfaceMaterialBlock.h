#pragma once


class D3D11ShaderProgram;

constexpr size_t MAX_RENDER_PASS_NAME = 64;
struct D3D11_SURFACE_RENDER_PASS
{
	char PassName[MAX_RENDER_PASS_NAME];
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