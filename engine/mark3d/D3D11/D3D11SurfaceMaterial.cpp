#include "pch.h"
#include "D3D11SurfaceMaterial.h"
#include "D3D11ShaderProgram.h"
#include "D3D11Texture1D.h"
#include "D3D11Texture2D.h"
#include "D3D11ConstantBufferImpl.h"

#include "D3D11ConstantBufferImpl.h"
#include "D3D11SurfaceMaterialBlock.h"
#include "D3D11SurfaceMaterialBlockPool.h"


D3D11SurfaceMaterial::D3D11SurfaceMaterial(D3D11_SURFACE_MATERIAL_BLOCK* pMaterialBlock) noexcept
	: m_pMaterialBlock(pMaterialBlock)
	, m_CurrentPass(-1)
{
}

D3D11SurfaceMaterial::~D3D11SurfaceMaterial() noexcept
{
	if (m_pMaterialBlock)
	{
		D3D11SurfaceMaterialBlockPool::Release(m_pMaterialBlock);
		m_pMaterialBlock = nullptr;
	}
}

void D3D11SurfaceMaterial::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11SurfaceMaterial);
}

int32 D3D11SurfaceMaterial::AddPass(const char* szPassName) noexcept
{
	if (m_pMaterialBlock->NumRenderPass >= MAX_RENDER_PASS)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::AddPass - Maximum render pass limit reached.");
		return -1;
	}

	// 새로운 렌더 패스 추가
	int32 NewPassIndex = (int32)m_pMaterialBlock->NumRenderPass;
	m_pMaterialBlock->RenderPasses[m_pMaterialBlock->NumRenderPass] = D3D11_RENDER_PASS();

	if (szPassName && fstrlen(szPassName) > 0)
		m_pMaterialBlock->RenderPasses[m_pMaterialBlock->NumRenderPass].Name = NameHash(szPassName);
	else
		m_pMaterialBlock->RenderPasses[m_pMaterialBlock->NumRenderPass].Name = NameHash(0u);

	m_pMaterialBlock->NumRenderPass++;

	return NewPassIndex;
}

int32 D3D11SurfaceMaterial::GetNumPass() const noexcept
{
	return (int32)m_pMaterialBlock->NumRenderPass;
}

void D3D11SurfaceMaterial::BeginPass(int32 Pass) noexcept
{
	if(0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::BeginPass - Invalid pass index: %d", Pass);
		m_CurrentPass = -1;
		return;
	}

	m_CurrentPass = Pass;
}

void D3D11SurfaceMaterial::EndPass() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::EndPass - No active pass to end.");
		return;
	}

	m_CurrentPass = -1;
}

void D3D11SurfaceMaterial::SetVertexShader(IShaderProgram* pVS)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetVertexShader(m_CurrentPass, pVS);
}

void D3D11SurfaceMaterial::SetPixelShader(IShaderProgram* pPS)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetPixelShader(m_CurrentPass, pPS);
}

IShaderProgram* D3D11SurfaceMaterial::GetVertexShader() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return nullptr;
	}

	return GetVertexShader(m_CurrentPass);
}

IShaderProgram* D3D11SurfaceMaterial::GetPixelShader() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return nullptr;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[m_CurrentPass];

	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();

	return pRenderPass->pPixelShader;
}

static __INLINE int32 FindTextureSlotIndexByName(D3D11_RENDER_PASS* pRenderPass, const NameHash& Name)
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

static __INLINE int32 FindTextureSlotIndexByBindIndex(D3D11_RENDER_PASS* pRenderPass, int32 BindIndex)
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

void D3D11SurfaceMaterial::SetConstantBuffer(int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetConstantBuffer(m_CurrentPass, SlotIndex, Name, pCBuffer);
}

void D3D11SurfaceMaterial::SetSamplerState(int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetSamplerState(m_CurrentPass, SamplerIndex, SamplerState);
}

void D3D11SurfaceMaterial::SetFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetFilter(m_CurrentPass, SamplerIndex, MinFilter, MagFilter, MipFilter);
}

void D3D11SurfaceMaterial::SetMinFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMinFilter(m_CurrentPass, SamplerIndex, MinFilter);
}

void D3D11SurfaceMaterial::SetMagFilter(int32 SamplerIndex, SAMPLER_FILTER MagFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMagFilter(m_CurrentPass, SamplerIndex, MagFilter);
}

void D3D11SurfaceMaterial::SetMipFilter(int32 SamplerIndex, SAMPLER_FILTER MipFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMipFilter(m_CurrentPass, SamplerIndex, MipFilter);
}

void D3D11SurfaceMaterial::SetAddressUVW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressUVW(m_CurrentPass, SamplerIndex, AddressU, AddressV, AddressW);
}

void D3D11SurfaceMaterial::SetAddressUV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressUV(m_CurrentPass, SamplerIndex, AddressU, AddressV);
}

void D3D11SurfaceMaterial::SetAddressU(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressU(m_CurrentPass, SamplerIndex, AddressU);
}

void D3D11SurfaceMaterial::SetAddressV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressV(m_CurrentPass, SamplerIndex, AddressV);
}

void D3D11SurfaceMaterial::SetAddressW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressW(m_CurrentPass, SamplerIndex, AddressW);
}

void D3D11SurfaceMaterial::SetAnisotropy(int32 SamplerIndex, uint8 MaxAnisotropy)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAnisotropy(m_CurrentPass, SamplerIndex, MaxAnisotropy);
}

void D3D11SurfaceMaterial::SetBorderColor(int32 SamplerIndex, BORDER_COLOR BorderColor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBorderColor(m_CurrentPass, SamplerIndex, BorderColor);
}

void D3D11SurfaceMaterial::SetComparisonFunc(int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetComparisonFunc(m_CurrentPass, SamplerIndex, ComparisonFunc);
}

void D3D11SurfaceMaterial::SetLODParams(int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetLODParams(m_CurrentPass, SamplerIndex, MipLODBias, MinLOD, MaxLOD);
}

void D3D11SurfaceMaterial::SetTexture1D(int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetTexture1D(m_CurrentPass, TextureSlot, Name, pTexture);
}

void D3D11SurfaceMaterial::SetTexture2D(int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetTexture2D(m_CurrentPass, TextureSlot, Name, pTexture);
}

void D3D11SurfaceMaterial::SetBlendState(const RS_BLEND_STATE& BlendState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendState(m_CurrentPass, BlendState);
}

void D3D11SurfaceMaterial::SetBlendStateOption(BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendStateOption(m_CurrentPass, AlphaToCoverageEnable, IndependentBlendEnable);
}

void D3D11SurfaceMaterial::SetBlendTarget(int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTarget(m_CurrentPass, BlendTargetIndex, BlendTarget);
}

void D3D11SurfaceMaterial::EnableBlendTarget(int32 BlendTargetIndex, BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	EnableBlendTarget(m_CurrentPass, BlendTargetIndex, Enable);
}

void D3D11SurfaceMaterial::SetBlendTargetFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactorOp(m_CurrentPass, BlendTargetIndex, SrcFactor, DstFactor, BlendOp);
}

void D3D11SurfaceMaterial::SetBlendTargetFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactor(m_CurrentPass, BlendTargetIndex, SrcFactor, DstFactor);
}

void D3D11SurfaceMaterial::SetBlendTargetOp(int32 BlendTargetIndex, BLEND_OP BlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetOp(m_CurrentPass, BlendTargetIndex, BlendOp);
}

void D3D11SurfaceMaterial::SetBlendTargetAlphaFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetAlphaFactorOp(m_CurrentPass, BlendTargetIndex, SrcAlphaFactor, DstAlphaFactor, AlphaBlendOp);
}

void D3D11SurfaceMaterial::SetBlendTargetAlphaFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetAlphaFactor(m_CurrentPass, BlendTargetIndex, SrcAlphaFactor, DstAlphaFactor);
}

void D3D11SurfaceMaterial::SetBlendTargetOpAlpha(int32 BlendTargetIndex, BLEND_OP AlphaBlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetOpAlpha(m_CurrentPass, BlendTargetIndex, AlphaBlendOp);
}

void D3D11SurfaceMaterial::SetBlendTargetFactorValue(int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactorValue(m_CurrentPass, BlendTargetIndex, FLOAT4{ R, G, B, A });
}

void D3D11SurfaceMaterial::SetBlendTargetFactorValue(int32 BlendTargetIndex, const FLOAT4& Factor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactorValue(m_CurrentPass, BlendTargetIndex, Factor);
}

void D3D11SurfaceMaterial::SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthStencilState(m_CurrentPass, DepthStencilState);
}

void D3D11SurfaceMaterial::EnableDepth(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	EnableDepth(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::EnableZWrite(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	EnableZWrite(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetDepthState(BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthState(m_CurrentPass, ZEnable, ZWriteEnable, DepthFunc);
}

void D3D11SurfaceMaterial::SetStencilState(BOOL Enable, uint8 ReadMask, uint8 WriteMask)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetStencilState(m_CurrentPass, Enable, ReadMask, WriteMask);
}

void D3D11SurfaceMaterial::SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetRasterizerState(m_CurrentPass, RasterizerState);
}

void D3D11SurfaceMaterial::SetFillMode(FILL_MODE Mode)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetFillMode(m_CurrentPass, Mode);
}

void D3D11SurfaceMaterial::SetCullMode(CULL_MODE Mode)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetCullMode(m_CurrentPass, Mode);
}

void D3D11SurfaceMaterial::SetWireframeMode(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetWireframeMode(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetFrontCounterClockwise(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetFrontCounterClockwise(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetScissorEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetScissorEnable(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetMultisampleEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMultisampleEnable(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetAntialiasedLineEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAntialiasedLineEnable(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetDepthBiasEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthBiasEnable(m_CurrentPass, Enable);
}

void D3D11SurfaceMaterial::SetDepthBiasParams(int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthBiasParams(m_CurrentPass, DepthBias, DepthBiasClamp, SlopeScaledDepthBias);
}

// 직접 PASS 접근
void D3D11SurfaceMaterial::SetVertexShader(int32 Pass, IShaderProgram* pVS)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		SYS_LOG_E("D3D11SurfaceMaterial::SetConstantBuffer - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];

	if (pRenderPass->pVertexShader)
	{
		pRenderPass->pVertexShader->Release();
		pRenderPass->pVertexShader = nullptr;
	}

	if (pRenderPass->pVertexShader)
		pRenderPass->pVertexShader->AddRef();

	pRenderPass->pVertexShader = static_cast<D3D11ShaderProgram*>(pVS);
}

void D3D11SurfaceMaterial::SetPixelShader(int32 Pass, IShaderProgram* pPS)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetConstantBuffer - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetConstantBuffer - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];

	if (pRenderPass->pPixelShader)
	{
		pRenderPass->pPixelShader->Release();
		pRenderPass->pPixelShader = nullptr;
	}

	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();

	pRenderPass->pPixelShader = static_cast<D3D11ShaderProgram*>(pPS);
}

IShaderProgram* D3D11SurfaceMaterial::GetVertexShader(int32 Pass) noexcept
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::GetVertexShader - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::GetVertexShader - Invalid pass index: %d", Pass);
		return nullptr;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];

	if (pRenderPass->pVertexShader)
		pRenderPass->pVertexShader->AddRef();

	return pRenderPass->pVertexShader;
}

IShaderProgram* D3D11SurfaceMaterial::GetPixelShader(int32 Pass) noexcept
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::GetPixelShader - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::GetPixelShader - Invalid pass index: %d", Pass);
		return nullptr;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];

	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();

	return pRenderPass->pPixelShader;
}

void D3D11SurfaceMaterial::SetConstantBuffer(int32 Pass, int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetConstantBuffer - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetConstantBuffer - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_CONSTANT_SLOT <= SlotIndex || 0 > SlotIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetConstantBuffer - Invalid slot index index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetConstantBuffer - Invalid slot index: %d", SlotIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->ConstantState[SlotIndex].Name = Name;

	if (pRenderPass->ConstantState[SlotIndex].pCB)
	{
		pRenderPass->ConstantState[SlotIndex].pCB->Release();
		pRenderPass->ConstantState[SlotIndex].pCB = nullptr;
	}

	pCBuffer->AddRef();
	pRenderPass->ConstantState[SlotIndex].pCB = pCBuffer;
}

void D3D11SurfaceMaterial::SetSamplerState(int32 Pass, int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetSamplerState - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetSamplerState - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetSamplerState - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetSamplerState - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex] = SamplerState;
}

void D3D11SurfaceMaterial::SetFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetFilter - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetFilter - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].MinFilter = MinFilter;
	pRenderPass->SamplerState[SamplerIndex].MagFilter = MagFilter;
	pRenderPass->SamplerState[SamplerIndex].MipFilter = MipFilter;
}

void D3D11SurfaceMaterial::SetMinFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMinFilter - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMinFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMinFilter - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMinFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].MinFilter = MinFilter;
}

void D3D11SurfaceMaterial::SetMagFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MagFilter)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMagFilter - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMagFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMagFilter - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMagFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].MagFilter = MagFilter;
}

void D3D11SurfaceMaterial::SetMipFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MipFilter)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMipFilter - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMipFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMipFilter - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMipFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].MipFilter = MipFilter;
}

void D3D11SurfaceMaterial::SetAddressUVW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressUVW - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressUVW - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressUVW - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressUVW - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressU = AddressU;
	pRenderPass->SamplerState[SamplerIndex].AddressV = AddressV;
	pRenderPass->SamplerState[SamplerIndex].AddressW = AddressW;
}

void D3D11SurfaceMaterial::SetAddressUV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressUV - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressUV - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressUV - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressUV - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressU = AddressU;
	pRenderPass->SamplerState[SamplerIndex].AddressV = AddressV;
}

void D3D11SurfaceMaterial::SetAddressU(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressU - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressU - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressU - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressU - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressU = AddressU;
}

void D3D11SurfaceMaterial::SetAddressV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressV - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressV - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressV - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressV - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressV = AddressV;
}

void D3D11SurfaceMaterial::SetAddressW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressW - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressW - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAddressW - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAddressW - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressW = AddressW;
}

void D3D11SurfaceMaterial::SetAnisotropy(int32 Pass, int32 SamplerIndex, uint8 MaxAnisotropy)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAnisotropy - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAnisotropy - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAnisotropy - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAnisotropy - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];

	MaxAnisotropy = T_MAX(0u, T_MIN((uint8)MAX_ANISOTROPY_LEVEL, MaxAnisotropy));
	pRenderPass->SamplerState[SamplerIndex].MaxAnisotropy = MaxAnisotropy;
}

void D3D11SurfaceMaterial::SetBorderColor(int32 Pass, int32 SamplerIndex, BORDER_COLOR BorderColor)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBorderColor - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBorderColor - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBorderColor - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBorderColor - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].BorderColor = BorderColor;
}

void D3D11SurfaceMaterial::SetComparisonFunc(int32 Pass, int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetComparisonFunc - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetComparisonFunc - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetComparisonFunc - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetComparisonFunc - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].ComparisonFunc = ComparisonFunc;
}

void D3D11SurfaceMaterial::SetLODParams(int32 Pass, int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetLODParams - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetLODParams - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetLODParams - Invalid sampler index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetLODParams - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->SamplerState[SamplerIndex].MipLODBias = MipLODBias;
	pRenderPass->SamplerState[SamplerIndex].MinLOD = MinLOD;
	pRenderPass->SamplerState[SamplerIndex].MaxLOD = MaxLOD;
}

void D3D11SurfaceMaterial::SetTexture1D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetTexture1D - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetTexture1D - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_TEXTURE_SLOT <= TextureSlot || 0 > TextureSlot)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetTexture1D - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetTexture1D - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	
	if (pRenderPass->TextureState[TextureSlot].TextureType == TEXTURE_TYPE::TEX_1D)
	{
		// 기존 텍스처 해제
		if (pRenderPass->TextureState[TextureSlot].pTexture1D)
		{
			pRenderPass->TextureState[TextureSlot].pTexture1D->Release();
			pRenderPass->TextureState[TextureSlot].pTexture1D = nullptr;
		}
	}
	else if (pRenderPass->TextureState[TextureSlot].TextureType == TEXTURE_TYPE::TEX_2D)
	{
		// 기존 텍스처 해제
		if (pRenderPass->TextureState[TextureSlot].pTexture2D)
		{
			pRenderPass->TextureState[TextureSlot].pTexture2D->Release();
			pRenderPass->TextureState[TextureSlot].pTexture2D = nullptr;
		}
	}

	pRenderPass->TextureState[TextureSlot].Name = Name;
	pRenderPass->TextureState[TextureSlot].TextureType = TEXTURE_TYPE::TEX_1D;
	if (pTexture)
		pTexture->AddRef();
	pRenderPass->TextureState[TextureSlot].pTexture1D = pTexture;
}

void D3D11SurfaceMaterial::SetTexture2D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetTexture2D - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetTexture2D - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_TEXTURE_SLOT <= TextureSlot || 0 > TextureSlot)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetTexture2D - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetTexture2D - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];

	if (pRenderPass->TextureState[TextureSlot].TextureType == TEXTURE_TYPE::TEX_1D)
	{
		// 기존 텍스처 해제
		if (pRenderPass->TextureState[TextureSlot].pTexture1D)
		{
			pRenderPass->TextureState[TextureSlot].pTexture1D->Release();
			pRenderPass->TextureState[TextureSlot].pTexture1D = nullptr;
		}
	}
	else if (pRenderPass->TextureState[TextureSlot].TextureType == TEXTURE_TYPE::TEX_2D)
	{
		// 기존 텍스처 해제
		if (pRenderPass->TextureState[TextureSlot].pTexture2D)
		{
			pRenderPass->TextureState[TextureSlot].pTexture2D->Release();
			pRenderPass->TextureState[TextureSlot].pTexture2D = nullptr;
		}
	}

	pRenderPass->TextureState[TextureSlot].Name = Name;
	pRenderPass->TextureState[TextureSlot].TextureType = TEXTURE_TYPE::TEX_2D;
	if (pTexture)
		pTexture->AddRef();
	pRenderPass->TextureState[TextureSlot].pTexture2D = pTexture;
}

void D3D11SurfaceMaterial::SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendState - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendState - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState = BlendState;
}

void D3D11SurfaceMaterial::SetBlendStateOption(int32 Pass, BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendStateOption - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendStateOption - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.AlphaToCoverageEnable = AlphaToCoverageEnable;
	pRenderPass->BlendState.IndependentBlendEnable = IndependentBlendEnable;
}

void D3D11SurfaceMaterial::SetBlendTarget(int32 Pass, int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTarget - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTarget - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTarget - Invalid blend target index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTarget - Invalid blend target index: %d", BlendTargetIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex] = BlendTarget;
}

void D3D11SurfaceMaterial::EnableBlendTarget(int32 Pass, int32 BlendTargetIndex, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::EnableBlendTarget - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::EnableBlendTarget - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::EnableBlendTarget - Invalid blend target index");
		SYS_LOG_E("D3D11SurfaceMaterial::EnableBlendTarget - Invalid blend target index: %d", BlendTargetIndex);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendEnable = Enable;
}

void D3D11SurfaceMaterial::SetBlendTargetFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactorOp - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactorOp - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactorOp - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactorOp - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlend = SrcFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlend = DstFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOp = BlendOp;
}

void D3D11SurfaceMaterial::SetBlendTargetFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactor - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactor - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactor - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactor - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlend = SrcFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlend = DstFactor;
}

void D3D11SurfaceMaterial::SetBlendTargetOp(int32 Pass, int32 BlendTargetIndex, BLEND_OP BlendOp)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetOp - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetOp - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetOp - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetOp - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOp = BlendOp;
}

void D3D11SurfaceMaterial::SetBlendTargetAlphaFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlendAlpha = SrcAlphaFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlendAlpha = DstAlphaFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOpAlpha = AlphaBlendOp;
}

void D3D11SurfaceMaterial::SetBlendTargetAlphaFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlendAlpha = SrcAlphaFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlendAlpha = DstAlphaFactor;
}

void D3D11SurfaceMaterial::SetBlendTargetOpAlpha(int32 Pass, int32 BlendTargetIndex, BLEND_OP BlendOpAlpha)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetOpAlpha - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetOpAlpha - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetOpAlpha - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetOpAlpha - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOpAlpha = BlendOpAlpha;
}

void D3D11SurfaceMaterial::SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendFactor[BlendTargetIndex] = FLOAT4{ R, G, B, A };
}

void D3D11SurfaceMaterial::SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, const FLOAT4& Factor)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->BlendState.BlendFactor[BlendTargetIndex] = Factor;
}

void D3D11SurfaceMaterial::SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetDepthStencilState - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetDepthStencilState - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->DepthStencilState = DepthStencilState;
}

void D3D11SurfaceMaterial::EnableDepth(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::EnableDepth - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::EnableDepth - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->DepthStencilState.DepthEnable = Enable;
}

void D3D11SurfaceMaterial::EnableZWrite(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::EnableZWrite - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::EnableZWrite - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->DepthStencilState.DepthWriteEnable = Enable;
}

void D3D11SurfaceMaterial::SetDepthState(int32 Pass, BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetDepthState - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetDepthState - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->DepthStencilState.DepthEnable = ZEnable;
	pRenderPass->DepthStencilState.DepthWriteEnable = ZWriteEnable;
	pRenderPass->DepthStencilState.DepthFunc = DepthFunc;
}

void D3D11SurfaceMaterial::SetStencilState(int32 Pass, BOOL Enable, uint8 ReadMask, uint8 WriteMask)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetStencilState - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetStencilState - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->DepthStencilState.StencilEnable = Enable;
	pRenderPass->DepthStencilState.StencilReadMask = ReadMask;
	pRenderPass->DepthStencilState.StencilWriteMask = WriteMask;
}

void D3D11SurfaceMaterial::SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetRasterizerState - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetRasterizerState - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState = RasterizerState;
}

void D3D11SurfaceMaterial::SetFillMode(int32 Pass, FILL_MODE Mode)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetFillMode - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetFillMode - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.FillMode = Mode;
}

void D3D11SurfaceMaterial::SetCullMode(int32 Pass, CULL_MODE Mode)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetCullMode - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetCullMode - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.CullMode = Mode;
}

void D3D11SurfaceMaterial::SetWireframeMode(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetWireframeMode - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetWireframeMode - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.FillMode = Enable ? FILL_MODE::WIREFRAME : FILL_MODE::SOLID;
}

void D3D11SurfaceMaterial::SetFrontCounterClockwise(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetFrontCounterClockwise - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetFrontCounterClockwise - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.SetFrontCounterClockwise(Enable);
}

void D3D11SurfaceMaterial::SetScissorEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetScissorEnable - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetScissorEnable - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.SetScissorEnable(Enable);
}

void D3D11SurfaceMaterial::SetMultisampleEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetMultisampleEnable - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetMultisampleEnable - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.SetMultisampleEnable(Enable);
}

void D3D11SurfaceMaterial::SetAntialiasedLineEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetAntialiasedLineEnable - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetAntialiasedLineEnable - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.SetAntialiasedLineEnable(Enable);
}

void D3D11SurfaceMaterial::SetDepthBiasEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetDepthBiasEnable - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetDepthBiasEnable - Invalid pass index: %d", Pass);
		return;
	}
	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.SetDepthBiasEnable(Enable);

}

void D3D11SurfaceMaterial::SetDepthBiasParams(int32 Pass, int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias)
{
	if (0 > Pass || Pass >= (int32)m_pMaterialBlock->NumRenderPass)
	{
		__ASSERT(FALSE, "D3D11SurfaceMaterial::SetDepthBiasParams - Invalid pass index");
		SYS_LOG_E("D3D11SurfaceMaterial::SetDepthBiasParams - Invalid pass index: %d", Pass);
		return;
	}

	D3D11_RENDER_PASS* pRenderPass = &m_pMaterialBlock->RenderPasses[Pass];
	pRenderPass->RasterizerState.DepthBias = DepthBias;
	pRenderPass->RasterizerState.DepthBiasClamp = DepthBiasClamp;
	pRenderPass->RasterizerState.SlopeScaledDepthBias = SlopeScaledDepthBias;
}

