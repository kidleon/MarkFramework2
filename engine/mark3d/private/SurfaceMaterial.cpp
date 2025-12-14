#include "pch.h"
#include "SurfaceMaterial.h"
#include "IShaderProgram.h"
#include "ITexture1D.h"
#include "ITexture2D.h"
#include "IConstantBuffer.h"


SurfaceMaterial::~SurfaceMaterial() noexcept
{
}

void SurfaceMaterial::OnDestroy()
{
	MARK_DELETE(this, SurfaceMaterial);
}

int32 SurfaceMaterial::AddPass() noexcept
{
	if (m_NumRenderPass >= MAX_RENDER_PASS)
	{
		SYS_LOG_E("SurfaceMaterial::AddPass - Maximum render pass limit reached.");
		return -1;
	}

	// 새로운 렌더 패스 추가
	int32 NewPassIndex = (int32)m_NumRenderPass;
	m_RenderPass[m_NumRenderPass] = RENDER_PASS();
	m_NumRenderPass++;

	return NewPassIndex;
}

int32 SurfaceMaterial::GetNumPass() const noexcept
{
	return (int32)m_NumRenderPass;
}

void SurfaceMaterial::BeginPass(int32 Pass) noexcept
{
	if(0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		SYS_LOG_E("SurfaceMaterial::BeginPass - Invalid pass index: %d", Pass);
		m_CurrentPass = -1;
		return;
	}

	m_CurrentPass = Pass;
}

void SurfaceMaterial::EndPass() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::EndPass - No active pass to end.");
		return;
	}

	m_CurrentPass = -1;
}

void SurfaceMaterial::SetVertexShader(IShaderProgram* pVS)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[m_CurrentPass];

	if (pRenderPass->pVertexShader)
	{
		pRenderPass->pVertexShader->Release();
		pRenderPass->pVertexShader = nullptr;
	}

	pRenderPass->pVertexShader = pVS;
	if (pRenderPass->pVertexShader)
		pRenderPass->pVertexShader->AddRef();
}

void SurfaceMaterial::SetPixelShader(IShaderProgram* pPS)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[m_CurrentPass];
	if (pRenderPass->pPixelShader)
	{
		pRenderPass->pPixelShader->Release();
		pRenderPass->pPixelShader = nullptr;
	}

	pRenderPass->pPixelShader = pPS;
	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();
}

IShaderProgram* SurfaceMaterial::GetVertexShader() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return nullptr;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[m_CurrentPass];

	if (pRenderPass->pVertexShader)
		pRenderPass->pVertexShader->AddRef();

	return pRenderPass->pVertexShader;
}

IShaderProgram* SurfaceMaterial::GetPixelShader() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return nullptr;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[m_CurrentPass];

	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();

	return pRenderPass->pPixelShader;
}


__FORCEINLINE int32 FindTextureSlotIndexByName(RENDER_PASS* pRenderPass, const NameHash& Name)
{
	for (size_t i = 0; i < pRenderPass->NumTextureState; ++i)
	{
		if (pRenderPass->TextureState[i].Name == Name)
		{
			return (int32)i;
		}
	}
	return -1;
}

__FORCEINLINE int32 FindTextureSlotIndexByBindIndex(RENDER_PASS* pRenderPass, int32 BindIndex)
{
	for (size_t i = 0; i < pRenderPass->NumTextureState; ++i)
	{
		if (pRenderPass->TextureState[i].BindIndex == BindIndex)
		{
			return (int32)i;
		}
	}
	return -1;
}


void SurfaceMaterial::SetTexture1D(const NameHash& Name, ITexture1D* pTexture)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[m_CurrentPass];

	int32 findIndex = FindTextureSlotIndexByName(pRenderPass, Name);
	if (0 <= findIndex)
	{
		if (pRenderPass->TextureState[findIndex].pTexture1D)
		{
			pRenderPass->TextureState[findIndex].pTexture1D->Release();
			pRenderPass->TextureState[findIndex].pTexture1D = nullptr;
		}

		if (pRenderPass->TextureState[findIndex].pTexture2D)
		{
			pRenderPass->TextureState[findIndex].pTexture2D->Release();
			pRenderPass->TextureState[findIndex].pTexture2D = nullptr;
		}

		if (pTexture)
			pTexture->AddRef();
		pRenderPass->TextureState[findIndex].pTexture1D = pTexture;

		return;
	}

	if (pRenderPass->NumTextureState >= MAX_TEXTURE_SLOT)
	{
		SYS_LOG_E("SurfaceMaterial::SetTexture1D - Maximum texture slot limit reached.");
		return;
	}

	TEXTURE_STATE* pNewTextureState = &pRenderPass->TextureState[pRenderPass->NumTextureState];
	pNewTextureState->Name = Name;

	if (pRenderPass->pPixelShader)
	{
		int32 bindIndex = pRenderPass->pPixelShader->GetBindIndexByName(Name);
		pNewTextureState->BindIndex = bindIndex;
	}
	else
	{
		pNewTextureState->BindIndex = -1;
	}

	if (pTexture)
		pTexture->AddRef();
	pNewTextureState->pTexture1D = pTexture;
}

void SurfaceMaterial::SetTexture2D(const NameHash& Name, ITexture2D* pTexture)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[m_CurrentPass];

	int32 findIndex = FindTextureSlotIndexByName(pRenderPass, Name);
	if (0 <= findIndex)
	{
		if (pRenderPass->TextureState[findIndex].pTexture1D)
		{
			pRenderPass->TextureState[findIndex].pTexture1D->Release();
			pRenderPass->TextureState[findIndex].pTexture1D = nullptr;
		}

		if (pRenderPass->TextureState[findIndex].pTexture2D)
		{
			pRenderPass->TextureState[findIndex].pTexture2D->Release();
			pRenderPass->TextureState[findIndex].pTexture2D = nullptr;
		}

		if (pTexture)
			pTexture->AddRef();
		pRenderPass->TextureState[findIndex].pTexture2D = pTexture;

		return;
	}

	if (MAX_TEXTURE_SLOT <= pRenderPass->NumTextureState)
	{
		SYS_LOG_E("SurfaceMaterial::SetTexture1D - Maximum texture slot limit reached.");
		return;
	}

	TEXTURE_STATE* pNewTextureState = &pRenderPass->TextureState[pRenderPass->NumTextureState];
	pNewTextureState->Name = Name;

	if (pRenderPass->pPixelShader)
	{
		int32 bindIndex = pRenderPass->pPixelShader->GetBindIndexByName(Name);
		pNewTextureState->BindIndex = bindIndex;
	}
	else
	{
		pNewTextureState->BindIndex = -1;
	}

	if (pTexture)
		pTexture->AddRef();
	pNewTextureState->pTexture2D = pTexture;
}

void SurfaceMaterial::SetStructure(const NameHash& Name, const void* pData, uint32 DataSize)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetColor(const COLOR_VAULE& ColorVaule)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetBlendState(BOOL Enable, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::EnableDepth(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::EnableZWrite(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetDepthState(BOOL Enable, DEPTH_WRITE_MASK WriteMask, COMPARISON_FUNC Func)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetDepthBias(int32 DepthBias)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetStencilState(BOOL Enable, uint8 ReadMask, uint8 WriteMask, COMPARISON_FUNC Func)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetSamplerState()
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetCullMode(CULL_MODE Mode)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}

void SurfaceMaterial::SetWireframeMode(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}
}