#pragma once
#include "D3D11ShaderProgram.h"


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

	UINT32 SampleMask;
	UINT32 StencilRef;

	FLOAT4 Color;

	void Reset()
	{
		fstrcpy(PassName, "NoNamePass");
		CHECK_RELEASE(pVertexShader);
		CHECK_RELEASE(pPixelShader);

		pRasterizerState = nullptr;
		RasterizerStateHash = 0;
		pBlendState = nullptr;
		BlendStateHash = 0;
		BlendFactor = FLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
		pDepthStencilState = nullptr;
		DepthStencilStateHash = 0;
		SampleMask = 0xFFFFFFFF;
		StencilRef = 0;
		Color = FLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	}
};

struct D3D11_SURFACE_MATERIAL_BLOCK
{
	D3D11_SURFACE_RENDER_PASS RenderPasses[MAX_RENDER_PASS];
	size_t NumPasses;

	LINK_NODE LinkNode;

	void Reset()
	{
		NumPasses = 0;
		for (size_t i = 0; i < MAX_RENDER_PASS; ++i)
		{
			RenderPasses[i].Reset();
		}
	}
};