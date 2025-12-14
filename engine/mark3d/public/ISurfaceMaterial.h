#ifndef __I_SURFAE_MATERIAL_H__
#define __I_SURFAE_MATERIAL_H__

#include "IAssetImpl.h"


interface IShaderProgram;
interface IConstantBuffer;

class ISurfaceMaterial : public IASSET_IMPL<ASSET_TYPE::SURFACE_MATERIAL>
{
public:
	virtual int32 AddPass(const char* szPassName) noexcept = 0;
	virtual int32 GetNumPass() const noexcept = 0;

	virtual void BeginPass(int32 Pass) noexcept = 0;
	virtual void EndPass() noexcept = 0;

	virtual void SetVertexShader(IShaderProgram* pVS) = 0;
	virtual void SetPixelShader(IShaderProgram* pPS) = 0;

	virtual IShaderProgram* GetVertexShader() noexcept = 0;
	virtual IShaderProgram* GetPixelShader() noexcept = 0;

	virtual void SetGlobalConstantBuffer(const NameHash& Name, IConstantBuffer* pCBuffer) = 0;
	virtual void SetConstantBuffer(const NameHash& Name, IConstantBuffer* pCBuffer) = 0;

	virtual void SetSamplerState(int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) = 0;
	virtual void SetFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter) = 0;
	virtual void SetMinFilter(int32 SamplerIndex, SAMPLER_FILTER MinFilter) = 0;
	virtual void SetMagFilter(int32 SamplerIndex, SAMPLER_FILTER MagFilter) = 0;
	virtual void SetMipFilter(int32 SamplerIndex, SAMPLER_FILTER MipFilter) = 0;

	virtual void SetAddressUVW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW) = 0;
	virtual void SetAddressUV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV) = 0;
	virtual void SetAddressU(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU) = 0;
	virtual void SetAddressV(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV) = 0;
	virtual void SetAddressW(int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW) = 0;
	virtual void SetAnisotropy(int32 SamplerIndex, uint8 MaxAnisotropy) = 0;
	virtual void SetBorderColor(int32 SamplerIndex, BORDER_COLOR BorderColor) = 0;
	virtual void SetComparisonFunc(int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc) = 0;
	virtual void SetLODParams(int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD) = 0;

	virtual void SetTexture1D(int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture) = 0;
	virtual void SetTexture2D(int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture) = 0;
	
	virtual void SetBlendState(const RS_BLEND_STATE& BlendState) = 0;
	virtual void SetBlendState(BOOL Enable, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp) = 0;
	virtual void SetBlendFactor(FLOAT R, FLOAT G, FLOAT B, FLOAT A) = 0;
	virtual void SetBlendFactor(const FLOAT4 Factor) = 0;

	virtual void SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;
	virtual void EnableDepth(BOOL Enable) = 0;
	virtual void EnableZWrite(BOOL Enable) = 0;
	virtual void SetDepthState(BOOL Enable, DEPTH_WRITE_MASK WriteMask, COMPARISON_FUNC Func) = 0;
	virtual void SetDepthBias(int32 DepthBias) = 0;
	virtual void SetStencilState(BOOL Enable, uint8 ReadMask, uint8 WriteMask, COMPARISON_FUNC Func) = 0;

	virtual void SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState) = 0;
	virtual void SetCullMode(CULL_MODE Mode) = 0;
	virtual void SetWireframeMode(BOOL Enable) = 0;
	virtual void SetFrontCounterClockwise(BOOL Enable) = 0;
	virtual void SetScissorEnable(BOOL Enable) = 0;
	virtual void SetMultisampleEnable(BOOL Enable) = 0;
	virtual void SetAntialiasedLineEnable(BOOL Enable) = 0;
	virtual void SetDepthBiasEnable(BOOL Enable) = 0;
	virtual void SetDepthBiasParams(int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias) = 0;

	// 직접 PASS 접근
	virtual void SetVertexShader(int32 Pass, IShaderProgram* pVS) = 0;
	virtual void SetPixelShader(int32 Pass, IShaderProgram* pPS) = 0;

	virtual IShaderProgram* GetVertexShader(int32 Pass) noexcept = 0;
	virtual IShaderProgram* GetPixelShader(int32 Pass) noexcept = 0;

	virtual void SetGlobalConstantBuffer(int32 Pass, const NameHash& Name, IConstantBuffer* pCBuffer) = 0;
	virtual void SetConstantBuffer(int32 Pass, const NameHash& Name, IConstantBuffer* pCBuffer) = 0;

	virtual void SetSamplerState(int32 Pass, int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) = 0;
	virtual void SetFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter, SAMPLER_FILTER MagFilter, SAMPLER_FILTER MipFilter) = 0;
	virtual void SetMinFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MinFilter) = 0;
	virtual void SetMagFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MagFilter) = 0;
	virtual void SetMipFilter(int32 Pass, int32 SamplerIndex, SAMPLER_FILTER MipFilter) = 0;

	virtual void SetAddressUVW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV, TEXTURE_ADDRESS_MODE AddressW) = 0;
	virtual void SetAddressUV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU, TEXTURE_ADDRESS_MODE AddressV) = 0;
	virtual void SetAddressU(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressU) = 0;
	virtual void SetAddressV(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressV) = 0;
	virtual void SetAddressW(int32 Pass, int32 SamplerIndex, TEXTURE_ADDRESS_MODE AddressW) = 0;
	virtual void SetAnisotropy(int32 Pass, int32 SamplerIndex, uint8 MaxAnisotropy) = 0;
	virtual void SetBorderColor(int32 Pass, int32 SamplerIndex, BORDER_COLOR BorderColor) = 0;
	virtual void SetComparisonFunc(int32 Pass, int32 SamplerIndex, COMPARISON_FUNC ComparisonFunc) = 0;
	virtual void SetLODParams(int32 Pass, int32 SamplerIndex, float MipLODBias, float MinLOD, float MaxLOD) = 0;

	virtual void SetTexture1D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture1D* pTexture) = 0;
	virtual void SetTexture2D(int32 Pass, int32 TextureSlot, const NameHash& Name, ITexture2D* pTexture) = 0;

	virtual void SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState) = 0;
	virtual void SetBlendState(int32 Pass, BOOL Enable, BLEND_FACTOR SrcFactor, BLEND_FACTOR DstFactor, BLEND_OP BlendOp) = 0;
	virtual void SetBlendFactor(int32 Pass, FLOAT R, FLOAT G, FLOAT B, FLOAT A) = 0;
	virtual void SetBlendFactor(int32 Pass, const FLOAT4 Factor) = 0;

	virtual void SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;
	virtual void EnableDepth(int32 Pass, BOOL Enable) = 0;
	virtual void EnableZWrite(int32 Pass, BOOL Enable) = 0;
	virtual void SetDepthState(int32 Pass, BOOL Enable, DEPTH_WRITE_MASK WriteMask, COMPARISON_FUNC Func) = 0;
	virtual void SetDepthBias(int32 Pass, int32 DepthBias) = 0;
	virtual void SetStencilState(int32 Pass, BOOL Enable, uint8 ReadMask, uint8 WriteMask, COMPARISON_FUNC Func) = 0;

	virtual void SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState) = 0;
	virtual void SetCullMode(int32 Pass, CULL_MODE Mode) = 0;
	virtual void SetWireframeMode(int32 Pass, BOOL Enable) = 0;
	virtual void SetFrontCounterClockwise(int32 Pass, BOOL Enable) = 0;
	virtual void SetScissorEnable(int32 Pass, BOOL Enable) = 0;
	virtual void SetMultisampleEnable(int32 Pass, BOOL Enable) = 0;
	virtual void SetAntialiasedLineEnable(int32 Pass, BOOL Enable) = 0;
	virtual void SetDepthBiasEnable(int32 Pass, BOOL Enable) = 0;
	virtual void SetDepthBiasParams(int32 Pass, int32 DepthBias, float DepthBiasClamp, float SlopeScaledDepthBias) = 0;

};


#endif // __I_SURFAE_MATERIAL_H__
