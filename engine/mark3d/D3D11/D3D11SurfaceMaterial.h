#ifndef __SURFACE_MATERIAL_H__
#define __SURFACE_MATERIAL_H__

#include "ISurfaceMaterial.h"
#include "D3D11RenderPass.h"


struct D3D11_SURFACE_MATERIAL_BLOCK;

class D3D11SurfaceMaterial final : public ISurfaceMaterial
{
public:
	D3D11SurfaceMaterial(D3D11_SURFACE_MATERIAL_BLOCK* pMaterialBlock) noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT32 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// ISurfaceMaterial interface
	int32 AddPass(const char* szPassName) noexcept final;
	int32 GetNumPass() const noexcept final;

	void BeginPass(int32 Pass) noexcept final;
	void EndPass() noexcept final;

	void SetVertexShader(IShaderProgram* pVS) final;
	void SetPixelShader(IShaderProgram* pPS) final;

	IShaderProgram* GetVertexShader() noexcept final;
	IShaderProgram* GetPixelShader() noexcept final;

	void SetConstantBuffer(int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) final;

	void SetSamplerState(int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) final;
	void SetFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter) final;
	void SetMinFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter) final;
	void SetMagFilter(int32 SamplerIndex, SAMPLER_FILTER MagFilter) final;
	void SetMipFilter(int32 SamplerIndex, SAMPLER_FILTER MipFilter) final;

	void SetAddressUVW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW) final;
	void SetAddressUV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV) final;
	void SetAddressU(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU) final;
	void SetAddressV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV) final;
	void SetAddressW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW) final;
	void SetAnisotropy(int32 SamplerIndex, uint8 MaxAnisotropy) final;
	void SetBorderColor(int32 SamplerIndex, BORDER_COLOR BorderColor) final;
	void SetComparisonFunc(int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc) final;
	void SetLODParams(int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD) final;

	void SetTexture1D(int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture) final;
	void SetTexture2D(int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture) final;

	void SetBlendState(const RS_BLEND_STATE& BlendState) final;
	void SetBlendStateOption(BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable) final;

	void SetBlendTarget(int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget) final;
	void EnableBlendTarget(int32 BlendTargetIndex, BOOL Enable) final;

	void SetBlendTargetFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp) final;
	void SetBlendTargetFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor) final;
	void SetBlendTargetOp(int32 BlendTargetIndex, BLEND_OP BlendOp) final;

	void SetBlendTargetAlphaFactorOp(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp) final;
	void SetBlendTargetAlphaFactor(int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor) final;
	void SetBlendTargetOpAlpha(int32 BlendTargetIndex, BLEND_OP AlphaBlendOp) final;
	void SetBlendTargetFactorValue(int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A) final;
	void SetBlendTargetFactorValue(int32 BlendTargetIndex, const FLOAT4& Factor) final;

	void SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState) final;
	void EnableDepth(BOOL Enable) final;
	void EnableZWrite(BOOL Enable) final;
	void SetDepthState(BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc) final;
	void SetStencilState(BOOL Enable, uint8 ReadMask, uint8 WriteMask) final;

	void SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState) final;
	void SetFillMode(FILL_MODE Mode) final;
	void SetCullMode(CULL_MODE Mode) final;
	void SetWireframeMode(BOOL Enable) final;
	void SetFrontCounterClockwise(BOOL Enable) final;
	void SetScissorEnable(BOOL Enable) final;
	void SetMultisampleEnable(BOOL Enable) final;
	void SetAntialiasedLineEnable(BOOL Enable) final;
	void SetDepthBiasEnable(BOOL Enable) final;
	void SetDepthBiasParams(int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias) final;


	// 직접 PASS 접근
	void SetVertexShader(int32 Pass, IShaderProgram* pVS) final;
	void SetPixelShader(int32 Pass, IShaderProgram* pPS) final;

	IShaderProgram* GetVertexShader(int32 Pass) noexcept final;
	IShaderProgram* GetPixelShader(int32 Pass) noexcept final;

	void SetConstantBuffer(int32 Pass, int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) final;

	void SetSamplerState(int32 Pass, int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) final;
	void SetFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter) final;
	void SetMinFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter) final;
	void SetMagFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MagFilter) final;
	void SetMipFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MipFilter) final;

	void SetAddressUVW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW) final;
	void SetAddressUV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV) final;
	void SetAddressU(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU) final;
	void SetAddressV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV) final;
	void SetAddressW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW) final;
	void SetAnisotropy(int32 Pass, int32 SamplerIndex, uint8 MaxAnisotropy) final;
	void SetBorderColor(int32 Pass, int32 SamplerIndex, BORDER_COLOR BorderColor) final;
	void SetComparisonFunc(int32 Pass, int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc) final;
	void SetLODParams(int32 Pass, int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD) final;

	void SetTexture1D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture) final;
	void SetTexture2D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture) final;

	void SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState) final;
	void SetBlendStateOption(int32 Pass, BOOL AlphaToCoverageEnable, BOOL IndependentBlendEnable) final;

	void SetBlendTarget(int32 Pass, int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget) final;
	void EnableBlendTarget(int32 Pass, int32 BlendTargetIndex, BOOL Enable) final;

	void SetBlendTargetFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp) final;
	void SetBlendTargetFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor) final;
	void SetBlendTargetOp(int32 Pass, int32 BlendTargetIndex, BLEND_OP BlendOp) final;

	void SetBlendTargetAlphaFactorOp(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor, BLEND_OP AlphaBlendOp) final;
	void SetBlendTargetAlphaFactor(int32 Pass, int32 BlendTargetIndex, BLEND_FACTOR SrcAlphaFactor, BLEND_FACTOR DstAlphaFactor) final;
	void SetBlendTargetOpAlpha(int32 Pass, int32 BlendTargetIndex, BLEND_OP AlphaBlendOp) final;
	void SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, FLOAT R, FLOAT G, FLOAT B, FLOAT A) final;
	void SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, const FLOAT4& Factor) final;

	void SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) final;
	void EnableDepth(int32 Pass, BOOL Enable) final;
	void EnableZWrite(int32 Pass, BOOL Enable) final;
	void SetDepthState(int32 Pass, BOOL ZEnable, BOOL ZWriteEnable, DEPTH_FUNC DepthFunc) final;
	void SetStencilState(int32 Pass, BOOL Enable, uint8 ReadMask, uint8 WriteMask) final;

	void SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState) final;
	void SetFillMode(int32 Pass, FILL_MODE Mode) final;
	void SetCullMode(int32 Pass, CULL_MODE Mode) final;
	void SetWireframeMode(int32 Pass, BOOL Enable) final;
	void SetFrontCounterClockwise(int32 Pass, BOOL Enable) final;
	void SetScissorEnable(int32 Pass, BOOL Enable) final;
	void SetMultisampleEnable(int32 Pass, BOOL Enable) final;
	void SetAntialiasedLineEnable(int32 Pass, BOOL Enable) final;
	void SetDepthBiasEnable(int32 Pass, BOOL Enable) final;
	void SetDepthBiasParams(int32 Pass, int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias) final;

protected:
	D3D11SurfaceMaterial() = delete;
	~D3D11SurfaceMaterial() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	D3D11_SURFACE_MATERIAL_BLOCK* m_pMaterialBlock;

	int32 m_CurrentPass;
	int32 PADDING = 0;
};


#endif // __SURFACE_MATERIAL_H__