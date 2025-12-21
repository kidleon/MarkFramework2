#include "pch.h"
#include "D3D11SurfaceMaterialBlock.h"
#include "D3D11ShaderProgram.h"
#include "D3D11Texture1D.h"
#include "D3D11Texture2D.h"
#include "D3D11ConstantBufferImpl.h"


void D3D11_SURFACE_MATERIAL_BLOCK::Reset()
{
	NumRenderPass = 0;

	LinkNode.next = LinkNode.prev = nullptr;
	LinkNode.data = this;

	for (int32 p = 0; p < MAX_RENDER_PASS; ++p)
	{
		D3D11_RENDER_PASS* pRenderPass = &RenderPasses[p];

		pRenderPass->Name = 0;
		pRenderPass->Pass = -1;

		// 쉐이더들 해제
		if (pRenderPass->pVertexShader)
		{
			pRenderPass->pVertexShader->Release();
			pRenderPass->pVertexShader = nullptr;
		}

		if (pRenderPass->pPixelShader)
		{
			pRenderPass->pPixelShader->Release();
			pRenderPass->pPixelShader = nullptr;
		}

		// 텍스처 해제
		for (int32 t = 0; t < MAX_TEXTURE_SLOT; ++t)
		{
			if (pRenderPass->TextureState[t].TextureType == TEXTURE_TYPE::TEX_1D)
			{
				if (pRenderPass->TextureState[t].pTexture1D)
				{
					pRenderPass->TextureState[t].pTexture1D->Release();
					pRenderPass->TextureState[t].pTexture1D = nullptr;
				}
			}
			else if (pRenderPass->TextureState[t].TextureType == TEXTURE_TYPE::TEX_2D)
			{
				if (pRenderPass->TextureState[t].pTexture2D)
				{
					pRenderPass->TextureState[t].pTexture2D->Release();
					pRenderPass->TextureState[t].pTexture2D = nullptr;
				}
			}

			pRenderPass->TextureState[t].BindIndex = 0;
			pRenderPass->TextureState[t].Name = 0;
			pRenderPass->TextureState[t].TextureType = TEXTURE_TYPE::TEX_UNKNOWN;
		}

		pRenderPass->NumTextureState = 0;

		// 상수버퍼 해제
		for (int32 c = 0; c < MAX_CONSTANT_SLOT; ++c)
		{
			if (pRenderPass->ConstantState[c].pCB)
			{
				pRenderPass->ConstantState[c].pCB->Release();
				pRenderPass->ConstantState[c].pCB = nullptr;
			}
			pRenderPass->ConstantState[c].BindIndex = 0;
			pRenderPass->ConstantState[c].Name = 0;
		}

		// 샘플러 상태 초기화
		for (int32 s = 0; s < MAX_SAMPLER_SLOT; ++s)
			pRenderPass->SamplerState[s] = RS_SAMPLER_STATE();

		// 블렌드 상태 초기화
		pRenderPass->BlendState = RS_BLEND_STATE();

		// 깊이-스텐실 상태 초기화
		pRenderPass->DepthStencilState = RS_DEPTH_STENCIL_STATE();

		// 래스터라이저 상태 초기화
		pRenderPass->RasterizerState = RS_RASTERIZER_STATE();
	}
}