#include "pch.h"
#include "SurfaceMaterial.h"
#include "IShaderProgram.h"
#include "ITexture1D.h"
#include "ITexture2D.h"
#include "IConstantBuffer.h"

#include "ConstantBuffer.h"
#include "SurfaceMaterialPool.h"


SurfaceMaterial::~SurfaceMaterial() noexcept
{
	Reset();
}

void SurfaceMaterial::OnDestroy()
{
	MARK_POOL_DELETE(this, SurfaceMaterial);
}

int32 SurfaceMaterial::AddPass(const char* szPassName) noexcept
{
	if (m_NumRenderPass >= MAX_RENDER_PASS)
	{
		SYS_LOG_E("SurfaceMaterial::AddPass - Maximum render pass limit reached.");
		return -1;
	}

	// 새로운 렌더 패스 추가
	int32 NewPassIndex = (int32)m_NumRenderPass;
	m_RenderPass[m_NumRenderPass] = RENDER_PASS();

	if (szPassName && fstrlen(szPassName) > 0)
		m_RenderPass[m_NumRenderPass].Name = NameHash(szPassName);
	else
		m_RenderPass[m_NumRenderPass].Name = NameHash(0u);

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

	SetVertexShader(m_CurrentPass, pVS);
}

void SurfaceMaterial::SetPixelShader(IShaderProgram* pPS)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetPixelShader(m_CurrentPass, pPS);
}

IShaderProgram* SurfaceMaterial::GetVertexShader() noexcept
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return nullptr;
	}

	return GetVertexShader(m_CurrentPass);
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

static __INLINE int32 FindTextureSlotIndexByName(RENDER_PASS* pRenderPass, const NameHash& Name)
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

static __INLINE int32 FindTextureSlotIndexByBindIndex(RENDER_PASS* pRenderPass, int32 BindIndex)
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

void SurfaceMaterial::SetConstantBuffer(int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetConstantBuffer(m_CurrentPass, SlotIndex, Name, pCBuffer);
}

void SurfaceMaterial::SetSamplerState(int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetSamplerState(m_CurrentPass, SamplerIndex, SamplerState);
}

void SurfaceMaterial::SetFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetFilter(m_CurrentPass, SamplerIndex, MinFilter, MagFilter, MipFilter);
}

void SurfaceMaterial::SetMinFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMinFilter(m_CurrentPass, SamplerIndex, MinFilter);
}

void SurfaceMaterial::SetMagFilter(int32 SamplerIndex, SAMPLER_FILTER MagFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMagFilter(m_CurrentPass, SamplerIndex, MagFilter);
}

void SurfaceMaterial::SetMipFilter(int32 SamplerIndex, SAMPLER_FILTER MipFilter)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMipFilter(m_CurrentPass, SamplerIndex, MipFilter);
}

void SurfaceMaterial::SetAddressUVW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressUVW(m_CurrentPass, SamplerIndex, AddressU, AddressV, AddressW);
}

void SurfaceMaterial::SetAddressUV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressUV(m_CurrentPass, SamplerIndex, AddressU, AddressV);
}

void SurfaceMaterial::SetAddressU(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressU(m_CurrentPass, SamplerIndex, AddressU);
}

void SurfaceMaterial::SetAddressV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressV(m_CurrentPass, SamplerIndex, AddressV);
}

void SurfaceMaterial::SetAddressW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAddressW(m_CurrentPass, SamplerIndex, AddressW);
}

void SurfaceMaterial::SetAnisotropy(int32 SamplerIndex, uint8 MaxAnisotropy)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAnisotropy(m_CurrentPass, SamplerIndex, MaxAnisotropy);
}

void SurfaceMaterial::SetBorderColor(int32 SamplerIndex, BORDER_COLOR BorderColor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBorderColor(m_CurrentPass, SamplerIndex, BorderColor);
}

void SurfaceMaterial::SetComparisonFunc(int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetComparisonFunc(m_CurrentPass, SamplerIndex, ComparisonFunc);
}

void SurfaceMaterial::SetLODParams(int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetLODParams(m_CurrentPass, SamplerIndex, MipLODBias, MinLOD, MaxLOD);
}

void SurfaceMaterial::SetTexture1D(int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetTexture1D(m_CurrentPass, TextureSlot, Name, pTexture);
}

void SurfaceMaterial::SetTexture2D(int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetTexture2D(m_CurrentPass, TextureSlot, Name, pTexture);
}

void SurfaceMaterial::SetBlendState(const RS_BLEND_STATE& BlendState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendState(m_CurrentPass, BlendState);
}

void SurfaceMaterial::SetBlendStateOption(BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendStateOption(m_CurrentPass, AlphaToCoverageEnable, IndependentBlendEnable);
}

void SurfaceMaterial::SetBlendTarget(int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTarget(m_CurrentPass, BlendTargetIndex, BlendTarget);
}

void SurfaceMaterial::EnableBlendTarget(int32 BlendTargetIndex, BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	EnableBlendTarget(m_CurrentPass, BlendTargetIndex, Enable);
}

void SurfaceMaterial::SetBlendTargetFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactorOp(m_CurrentPass, BlendTargetIndex, SrcFactor, DstFactor, BlendOp);
}

void SurfaceMaterial::SetBlendTargetFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactor(m_CurrentPass, BlendTargetIndex, SrcFactor, DstFactor);
}

void SurfaceMaterial::SetBlendTargetOp(int32 BlendTargetIndex, BLEND_OP BlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetOp(m_CurrentPass, BlendTargetIndex, BlendOp);
}

void SurfaceMaterial::SetBlendTargetAlphaFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetAlphaFactorOp(m_CurrentPass, BlendTargetIndex, SrcAlphaFactor, DstAlphaFactor, AlphaBlendOp);
}

void SurfaceMaterial::SetBlendTargetAlphaFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetAlphaFactor(m_CurrentPass, BlendTargetIndex, SrcAlphaFactor, DstAlphaFactor);
}

void SurfaceMaterial::SetBlendTargetOpAlpha(int32 BlendTargetIndex, BLEND_OP AlphaBlendOp)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetOpAlpha(m_CurrentPass, BlendTargetIndex, AlphaBlendOp);
}

void SurfaceMaterial::SetBlendTargetFactorValue(int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactorValue(m_CurrentPass, BlendTargetIndex, FLOAT4{ R, G, B, A });
}

void SurfaceMaterial::SetBlendTargetFactorValue(int32 BlendTargetIndex, const FLOAT4& Factor)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetBlendTargetFactorValue(m_CurrentPass, BlendTargetIndex, Factor);
}

void SurfaceMaterial::SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthStencilState(m_CurrentPass, DepthStencilState);
}

void SurfaceMaterial::EnableDepth(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	EnableDepth(m_CurrentPass, Enable);
}

void SurfaceMaterial::EnableZWrite(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	EnableZWrite(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetDepthState(BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthState(m_CurrentPass, ZEnable, ZWriteEnable, DepthFunc);
}

void SurfaceMaterial::SetStencilState(BOOL Enable, uint8 ReadMask, uint8 WriteMask)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetStencilState(m_CurrentPass, Enable, ReadMask, WriteMask);
}

void SurfaceMaterial::SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetRasterizerState(m_CurrentPass, RasterizerState);
}

void SurfaceMaterial::SetFillMode(FILL_MODE Mode)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetFillMode(m_CurrentPass, Mode);
}

void SurfaceMaterial::SetCullMode(CULL_MODE Mode)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetCullMode(m_CurrentPass, Mode);
}

void SurfaceMaterial::SetWireframeMode(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetWireframeMode(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetFrontCounterClockwise(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetFrontCounterClockwise(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetScissorEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetScissorEnable(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetMultisampleEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetMultisampleEnable(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetAntialiasedLineEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetAntialiasedLineEnable(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetDepthBiasEnable(BOOL Enable)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthBiasEnable(m_CurrentPass, Enable);
}

void SurfaceMaterial::SetDepthBiasParams(int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias)
{
	if (-1 == m_CurrentPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetVertexShader - No active pass. Call BeginPass first.");
		return;
	}

	SetDepthBiasParams(m_CurrentPass, DepthBias, DepthBiasClamp, SlopeScaledDepthBias);
}

// 직접 PASS 접근
void SurfaceMaterial::SetVertexShader(int32 Pass, IShaderProgram* pVS)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		SYS_LOG_E("SurfaceMaterial::SetConstantBuffer - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];

	if (pRenderPass->pVertexShader)
	{
		pRenderPass->pVertexShader->Release();
		pRenderPass->pVertexShader = nullptr;
	}

	if (pRenderPass->pVertexShader)
		pRenderPass->pVertexShader->AddRef();

	pRenderPass->pVertexShader = pVS;
}

void SurfaceMaterial::SetPixelShader(int32 Pass, IShaderProgram* pPS)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetConstantBuffer - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetConstantBuffer - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];

	if (pRenderPass->pPixelShader)
	{
		pRenderPass->pPixelShader->Release();
		pRenderPass->pPixelShader = nullptr;
	}

	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();

	pRenderPass->pPixelShader = pPS;
}

IShaderProgram* SurfaceMaterial::GetVertexShader(int32 Pass) noexcept
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::GetVertexShader - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::GetVertexShader - Invalid pass index: %d", Pass);
		return nullptr;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];

	if (pRenderPass->pVertexShader)
		pRenderPass->pVertexShader->AddRef();

	return pRenderPass->pVertexShader;
}

IShaderProgram* SurfaceMaterial::GetPixelShader(int32 Pass) noexcept
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::GetPixelShader - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::GetPixelShader - Invalid pass index: %d", Pass);
		return nullptr;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];

	if (pRenderPass->pPixelShader)
		pRenderPass->pPixelShader->AddRef();

	return pRenderPass->pPixelShader;
}

void SurfaceMaterial::SetConstantBuffer(int32 Pass, int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetConstantBuffer - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetConstantBuffer - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_CONSTANT_SLOT <= SlotIndex || 0 > SlotIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetConstantBuffer - Invalid slot index index");
		SYS_LOG_E("SurfaceMaterial::SetConstantBuffer - Invalid slot index: %d", SlotIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->ConstantState[SlotIndex].Name = Name;

	if (pRenderPass->ConstantState[SlotIndex].pCB)
	{
		pRenderPass->ConstantState[SlotIndex].pCB->Release();
		pRenderPass->ConstantState[SlotIndex].pCB = nullptr;
	}

	pCBuffer->AddRef();
	pRenderPass->ConstantState[SlotIndex].pCB = pCBuffer;
}

void SurfaceMaterial::SetSamplerState(int32 Pass, int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetSamplerState - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetSamplerState - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetSamplerState - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetSamplerState - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex] = SamplerState;
}

void SurfaceMaterial::SetFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetFilter - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetFilter - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].MinFilter = MinFilter;
	pRenderPass->SamplerState[SamplerIndex].MagFilter = MagFilter;
	pRenderPass->SamplerState[SamplerIndex].MipFilter = MipFilter;
}

void SurfaceMaterial::SetMinFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMinFilter - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetMinFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMinFilter - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetMinFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].MinFilter = MinFilter;
}

void SurfaceMaterial::SetMagFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MagFilter)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMagFilter - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetMagFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMagFilter - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetMagFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].MagFilter = MagFilter;
}

void SurfaceMaterial::SetMipFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MipFilter)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMipFilter - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetMipFilter - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMipFilter - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetMipFilter - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].MipFilter = MipFilter;
}

void SurfaceMaterial::SetAddressUVW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressUVW - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAddressUVW - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressUVW - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetAddressUVW - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressU = AddressU;
	pRenderPass->SamplerState[SamplerIndex].AddressV = AddressV;
	pRenderPass->SamplerState[SamplerIndex].AddressW = AddressW;
}

void SurfaceMaterial::SetAddressUV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressUV - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAddressUV - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressUV - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetAddressUV - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressU = AddressU;
	pRenderPass->SamplerState[SamplerIndex].AddressV = AddressV;
}

void SurfaceMaterial::SetAddressU(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressU - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAddressU - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressU - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetAddressU - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressU = AddressU;
}

void SurfaceMaterial::SetAddressV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressV - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAddressV - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressV - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetAddressV - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressV = AddressV;
}

void SurfaceMaterial::SetAddressW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressW - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAddressW - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAddressW - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetAddressW - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].AddressW = AddressW;
}

void SurfaceMaterial::SetAnisotropy(int32 Pass, int32 SamplerIndex, uint8 MaxAnisotropy)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAnisotropy - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAnisotropy - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAnisotropy - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetAnisotropy - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];

	MaxAnisotropy = T_MAX(0u, T_MIN((uint8)MAX_ANISOTROPY_LEVEL, MaxAnisotropy));
	pRenderPass->SamplerState[SamplerIndex].MaxAnisotropy = MaxAnisotropy;
}

void SurfaceMaterial::SetBorderColor(int32 Pass, int32 SamplerIndex, BORDER_COLOR BorderColor)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBorderColor - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBorderColor - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBorderColor - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetBorderColor - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].BorderColor = BorderColor;
}

void SurfaceMaterial::SetComparisonFunc(int32 Pass, int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetComparisonFunc - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetComparisonFunc - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetComparisonFunc - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetComparisonFunc - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].ComparisonFunc = ComparisonFunc;
}

void SurfaceMaterial::SetLODParams(int32 Pass, int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetLODParams - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetLODParams - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_SAMPLER_SLOT <= SamplerIndex || 0 > SamplerIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetLODParams - Invalid sampler index");
		SYS_LOG_E("SurfaceMaterial::SetLODParams - Invalid sampler index: %d", SamplerIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->SamplerState[SamplerIndex].MipLODBias = MipLODBias;
	pRenderPass->SamplerState[SamplerIndex].MinLOD = MinLOD;
	pRenderPass->SamplerState[SamplerIndex].MaxLOD = MaxLOD;
}

void SurfaceMaterial::SetTexture1D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetTexture1D - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetTexture1D - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_TEXTURE_SLOT <= TextureSlot || 0 > TextureSlot)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetTexture1D - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetTexture1D - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	
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

void SurfaceMaterial::SetTexture2D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetTexture2D - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetTexture2D - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_TEXTURE_SLOT <= TextureSlot || 0 > TextureSlot)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetTexture2D - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetTexture2D - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];

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

void SurfaceMaterial::SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendState - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendState - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState = BlendState;
}

void SurfaceMaterial::SetBlendStateOption(int32 Pass, BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendStateOption - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendStateOption - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.AlphaToCoverageEnable = AlphaToCoverageEnable;
	pRenderPass->BlendState.IndependentBlendEnable = IndependentBlendEnable;
}

void SurfaceMaterial::SetBlendTarget(int32 Pass, int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTarget - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTarget - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTarget - Invalid blend target index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTarget - Invalid blend target index: %d", BlendTargetIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex] = BlendTarget;
}

void SurfaceMaterial::EnableBlendTarget(int32 Pass, int32 BlendTargetIndex, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::EnableBlendTarget - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::EnableBlendTarget - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::EnableBlendTarget - Invalid blend target index");
		SYS_LOG_E("SurfaceMaterial::EnableBlendTarget - Invalid blend target index: %d", BlendTargetIndex);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendEnable = Enable;
}

void SurfaceMaterial::SetBlendTargetFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactorOp - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactorOp - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactorOp - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactorOp - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlend = SrcFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlend = DstFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOp = BlendOp;
}

void SurfaceMaterial::SetBlendTargetFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactor - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactor - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactor - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactor - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlend = SrcFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlend = DstFactor;
}

void SurfaceMaterial::SetBlendTargetOp(int32 Pass, int32 BlendTargetIndex, BLEND_OP BlendOp)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetOp - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetOp - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetOp - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetOp - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOp = BlendOp;
}

void SurfaceMaterial::SetBlendTargetAlphaFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetAlphaFactorOp - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlendAlpha = SrcAlphaFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlendAlpha = DstAlphaFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOpAlpha = AlphaBlendOp;
}

void SurfaceMaterial::SetBlendTargetAlphaFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetAlphaFactor - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].SrcBlendAlpha = SrcAlphaFactor;
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].DestBlendAlpha = DstAlphaFactor;
}

void SurfaceMaterial::SetBlendTargetOpAlpha(int32 Pass, int32 BlendTargetIndex, BLEND_OP BlendOpAlpha)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetOpAlpha - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetOpAlpha - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetOpAlpha - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetOpAlpha - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendTarget[BlendTargetIndex].BlendOpAlpha = BlendOpAlpha;
}

void SurfaceMaterial::SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendFactor[BlendTargetIndex] = FLOAT4{ R, G, B, A };
}

void SurfaceMaterial::SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, const FLOAT4& Factor)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactorValue - Invalid pass index: %d", Pass);
		return;
	}

	if (MAX_BLEND_TARGET <= BlendTargetIndex || 0 > BlendTargetIndex)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index");
		SYS_LOG_E("SurfaceMaterial::SetBlendTargetFactorValue - Invalid texture slot index: %d", TextureSlot);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->BlendState.BlendFactor[BlendTargetIndex] = Factor;
}

void SurfaceMaterial::SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetDepthStencilState - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetDepthStencilState - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->DepthStencilState = DepthStencilState;
}

void SurfaceMaterial::EnableDepth(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::EnableDepth - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::EnableDepth - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->DepthStencilState.DepthEnable = Enable;
}

void SurfaceMaterial::EnableZWrite(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::EnableZWrite - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::EnableZWrite - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->DepthStencilState.DepthWriteEnable = Enable;
}

void SurfaceMaterial::SetDepthState(int32 Pass, BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetDepthState - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetDepthState - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->DepthStencilState.DepthEnable = ZEnable;
	pRenderPass->DepthStencilState.DepthWriteEnable = ZWriteEnable;
	pRenderPass->DepthStencilState.DepthFunc = DepthFunc;
}

void SurfaceMaterial::SetStencilState(int32 Pass, BOOL Enable, uint8 ReadMask, uint8 WriteMask)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetStencilState - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetStencilState - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->DepthStencilState.StencilEnable = Enable;
	pRenderPass->DepthStencilState.StencilReadMask = ReadMask;
	pRenderPass->DepthStencilState.StencilWriteMask = WriteMask;
}

void SurfaceMaterial::SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetRasterizerState - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetRasterizerState - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState = RasterizerState;
}

void SurfaceMaterial::SetFillMode(int32 Pass, FILL_MODE Mode)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetFillMode - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetFillMode - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.FillMode = Mode;
}

void SurfaceMaterial::SetCullMode(int32 Pass, CULL_MODE Mode)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetCullMode - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetCullMode - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.CullMode = Mode;
}

void SurfaceMaterial::SetWireframeMode(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetWireframeMode - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetWireframeMode - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.FillMode = Enable ? FILL_MODE::WIREFRAME : FILL_MODE::SOLID;
}

void SurfaceMaterial::SetFrontCounterClockwise(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetFrontCounterClockwise - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetFrontCounterClockwise - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.SetFrontCounterClockwise(Enable);
}

void SurfaceMaterial::SetScissorEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetScissorEnable - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetScissorEnable - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.SetScissorEnable(Enable);
}

void SurfaceMaterial::SetMultisampleEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetMultisampleEnable - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetMultisampleEnable - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.SetMultisampleEnable(Enable);
}

void SurfaceMaterial::SetAntialiasedLineEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetAntialiasedLineEnable - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetAntialiasedLineEnable - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.SetAntialiasedLineEnable(Enable);
}

void SurfaceMaterial::SetDepthBiasEnable(int32 Pass, BOOL Enable)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetDepthBiasEnable - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetDepthBiasEnable - Invalid pass index: %d", Pass);
		return;
	}
	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.SetDepthBiasEnable(Enable);

}

void SurfaceMaterial::SetDepthBiasParams(int32 Pass, int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias)
{
	if (0 > Pass || Pass >= (int32)m_NumRenderPass)
	{
		__ASSERT(FALSE, "SurfaceMaterial::SetDepthBiasParams - Invalid pass index");
		SYS_LOG_E("SurfaceMaterial::SetDepthBiasParams - Invalid pass index: %d", Pass);
		return;
	}

	RENDER_PASS* pRenderPass = &m_RenderPass[Pass];
	pRenderPass->RasterizerState.DepthBias = DepthBias;
	pRenderPass->RasterizerState.DepthBiasClamp = DepthBiasClamp;
	pRenderPass->RasterizerState.SlopeScaledDepthBias = SlopeScaledDepthBias;
}

void SurfaceMaterial::Reset()
{
	m_NumRenderPass = 0;
	m_LinkNode.next = m_LinkNode.prev = nullptr;
	m_LinkNode.data = this;

	for (int32 p = 0; p < MAX_RENDER_PASS; ++p)
	{
		RENDER_PASS* pRenderPass = &m_RenderPass[p];

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