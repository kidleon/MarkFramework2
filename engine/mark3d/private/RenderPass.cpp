#include "pch.h"
#include "RenderPass.h"
#include "IShaderProgram.h"
#include "ITexture1D.h"
#include "ITexture2D.h"
#include "IConstantBuffer.h"


RENDER_PASS::~RENDER_PASS() noexcept
{
	if (pVertexShader)
	{
		pVertexShader->Release();
		pVertexShader = nullptr;
	}

	if (pPixelShader)
	{
		pPixelShader->Release();
		pPixelShader = nullptr;
	}

	for(size_t i = 0; i < NumTextureState; ++i)
	{
		if (TextureState[i].pTexture1D)
		{
			TextureState[i].pTexture1D->Release();
			TextureState[i].pTexture1D = nullptr;
		}
	}
}