#ifndef __SURFACE_MATERIAL_H__
#define __SURFACE_MATERIAL_H__

#include "ISurfaceMaterial.h"
#include "RenderPass.h"


class SurfaceMaterial : public ISurfaceMaterial
{
	friend class SurfaceMaterialPool; // 오직 SurfaceMaterialPool 만이 SurfaceMaterial 인스턴스를 생성/해제할 수 있음

public:
	int32 AddPass(const char* szPassName) noexcept override;
	int32 GetNumPass() const noexcept override;

	void BeginPass(int32 Pass) noexcept override;
	void EndPass() noexcept override;

	void SetVertexShader(IShaderProgram* pVS) override;
	void SetPixelShader(IShaderProgram* pPS) override;

	IShaderProgram* GetVertexShader() noexcept override;
	IShaderProgram* GetPixelShader() noexcept override;

	void SetConstantBuffer(int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) override;

	void SetSamplerState(int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) override;
	void SetFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter) override;
	void SetMinFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter) override;
	void SetMagFilter(int32 SamplerIndex, SAMPLER_FILTER MagFilter) override;
	void SetMipFilter(int32 SamplerIndex, SAMPLER_FILTER MipFilter) override;

	void SetAddressUVW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW) override;
	void SetAddressUV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV) override;
	void SetAddressU(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU) override;
	void SetAddressV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV) override;
	void SetAddressW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW) override;
	void SetAnisotropy(int32 SamplerIndex, uint8 MaxAnisotropy) override;
	void SetBorderColor(int32 SamplerIndex, BORDER_COLOR BorderColor) override;
	void SetComparisonFunc(int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc) override;
	void SetLODParams(int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD) override;

	void SetTexture1D(int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture) override;
	void SetTexture2D(int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture) override;

	void SetBlendState(const RS_BLEND_STATE& BlendState) override;
	void SetBlendStateOption(BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable) override;

	void SetBlendTarget(int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget) override;
	void EnableBlendTarget(int32 BlendTargetIndex, BOOL Enable) override;

	void SetBlendTargetFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp) override;
	void SetBlendTargetFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor) override;
	void SetBlendTargetOp(int32 BlendTargetIndex, BLEND_OP BlendOp) override;

	void SetBlendTargetAlphaFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp) override;
	void SetBlendTargetAlphaFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor) override;
	void SetBlendTargetOpAlpha(int32 BlendTargetIndex, BLEND_OP AlphaBlendOp) override;
	void SetBlendTargetFactorValue(int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A) override;
	void SetBlendTargetFactorValue(int32 BlendTargetIndex, const FLOAT4& Factor) override;

	void SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState) override;
	void EnableDepth(BOOL Enable) override;
	void EnableZWrite(BOOL Enable) override;
	void SetDepthState(BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc) override;
	void SetStencilState(BOOL Enable, uint8 ReadMask, uint8 WriteMask) override;

	void SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState) override;
	void SetFillMode(FILL_MODE Mode) override;
	void SetCullMode(CULL_MODE Mode) override;
	void SetWireframeMode(BOOL Enable) override;
	void SetFrontCounterClockwise(BOOL Enable) override;
	void SetScissorEnable(BOOL Enable) override;
	void SetMultisampleEnable(BOOL Enable) override;
	void SetAntialiasedLineEnable(BOOL Enable) override;
	void SetDepthBiasEnable(BOOL Enable) override;
	void SetDepthBiasParams(int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias) override;


	// 직접 PASS 접근
	void SetVertexShader(int32 Pass, IShaderProgram* pVS) override;
	void SetPixelShader(int32 Pass, IShaderProgram* pPS) override;

	IShaderProgram* GetVertexShader(int32 Pass) noexcept override;
	IShaderProgram* GetPixelShader(int32 Pass) noexcept override;

	void SetConstantBuffer(int32 Pass, int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) override;

	void SetSamplerState(int32 Pass, int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) override;
	void SetFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter) override;
	void SetMinFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter) override;
	void SetMagFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MagFilter) override;
	void SetMipFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MipFilter) override;

	void SetAddressUVW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW) override;
	void SetAddressUV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV) override;
	void SetAddressU(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU) override;
	void SetAddressV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV) override;
	void SetAddressW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW) override;
	void SetAnisotropy(int32 Pass, int32 SamplerIndex, uint8 MaxAnisotropy) override;
	void SetBorderColor(int32 Pass, int32 SamplerIndex, BORDER_COLOR BorderColor) override;
	void SetComparisonFunc(int32 Pass, int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc) override;
	void SetLODParams(int32 Pass, int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD) override;

	void SetTexture1D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture) override;
	void SetTexture2D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture) override;

	void SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState) override;
	void SetBlendStateOption(int32 Pass, BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable) override;

	void SetBlendTarget(int32 Pass, int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget) override;
	void EnableBlendTarget(int32 Pass, int32 BlendTargetIndex, BOOL Enable) override;

	void SetBlendTargetFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp) override;
	void SetBlendTargetFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor) override;
	void SetBlendTargetOp(int32 Pass, int32 BlendTargetIndex, BLEND_OP BlendOp) override;

	void SetBlendTargetAlphaFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp) override;
	void SetBlendTargetAlphaFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor) override;
	void SetBlendTargetOpAlpha(int32 Pass, int32 BlendTargetIndex, BLEND_OP AlphaBlendOp) override;
	void SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A) override;
	void SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, const FLOAT4& Factor) override;

	void SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) override;
	void EnableDepth(int32 Pass, BOOL Enable) override;
	void EnableZWrite(int32 Pass, BOOL Enable) override;
	void SetDepthState(int32 Pass, BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc) override;
	void SetStencilState(int32 Pass, BOOL Enable, uint8 ReadMask, uint8 WriteMask) override;

	void SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState) override;
	void SetFillMode(int32 Pass, FILL_MODE Mode) override;
	void SetCullMode(int32 Pass, CULL_MODE Mode) override;
	void SetWireframeMode(int32 Pass, BOOL Enable) override;
	void SetFrontCounterClockwise(int32 Pass, BOOL Enable) override;
	void SetScissorEnable(int32 Pass, BOOL Enable) override;
	void SetMultisampleEnable(int32 Pass, BOOL Enable) override;
	void SetAntialiasedLineEnable(int32 Pass, BOOL Enable) override;
	void SetDepthBiasEnable(int32 Pass, BOOL Enable) override;
	void SetDepthBiasParams(int32 Pass, int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias) override;

	void Reset();

	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept
	{
		return &m_LinkNode;
	}

protected:
	SurfaceMaterial() = default;
	~SurfaceMaterial() noexcept;
	virtual void OnDestroy() override;

private:
	RENDER_PASS m_RenderPass[MAX_RENDER_PASS];
	size_t m_NumRenderPass = 0;

	int32 m_CurrentPass = -1;
	int32 PADDING = 0;

	LINK_NODE m_LinkNode = {};

};


#endif // __SURFACE_MATERIAL_H__