#pragma once

#include "D3D11SurfaceMaterialBlock.h"
#include "D3D11ShaderProgram.h"


class D3D11SurfaceMaterial final : public ISurfaceMaterial
{
public:
	explicit D3D11SurfaceMaterial(D3D11_SURFACE_MATERIAL_BLOCK* pMaterialBlock);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// ISurfaceMaterial interface
	virtual int32 AddPass(const char* szPassName) noexcept final;
	virtual int32 GetNumPass() const noexcept final;

	virtual void SetVertexShader(int32 Pass, IShaderProgram* pVS) final;
	virtual void SetVertexShader(IShaderProgram* pVS) final;

	virtual void SetPixelShader(int32 Pass, IShaderProgram* pPS) final;
	virtual void SetPixelShader(IShaderProgram* pPS) final;

	virtual IShaderProgram* GetVertexShader(int32 Pass) noexcept final;
	virtual IShaderProgram* GetVertexShader() noexcept final;

	virtual IShaderProgram* GetPixelShader(int32 Pass) noexcept final;
	virtual IShaderProgram* GetPixelShader() noexcept final;

	virtual void SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState) final;
	virtual void SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState) final;

	virtual void SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState) final;
	virtual void SetBlendState(const RS_BLEND_STATE& BlendState) final;

	virtual void SetBlendFactor(int32 Pass, const FLOAT4& BlendFactor) final;
	virtual void SetBlendFactor(const FLOAT4& BlendFactor) final;

	virtual void SetSampleMask(int32 Pass, UINT32 SampleMask) final;
	virtual void SetSampleMask(UINT32 SampleMask) final;

	virtual void SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) final;
	virtual void SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState) final;

	virtual void SetStencilRef(int32 Pass, UINT8 StencilRef) final;
	virtual void SetStencilRef(UINT8 StencilRef) final;

	virtual void SetColor(int32 Pass, const FLOAT4& Color) final;
	virtual void SetColor(const FLOAT4& Color) final;

	virtual const FLOAT4& GetColor(int32 Pass) const noexcept final;
	virtual const FLOAT4& GetColor() const noexcept final;

	__FORCEINLINE int32 INL_GetNumPass() const noexcept
	{
		return (int32)m_pMaterialBlock->NumPasses;
	}

	__FORCEINLINE D3D11ShaderProgram* INL_GetVertexShader(int32 Pass) noexcept
	{
		if (m_pMaterialBlock->RenderPasses[Pass].pVertexShader)
			m_pMaterialBlock->RenderPasses[Pass].pVertexShader->AddRef();

		return m_pMaterialBlock->RenderPasses[Pass].pVertexShader;
	}

	__FORCEINLINE D3D11ShaderProgram* INL_GetPixelShader(int32 Pass) noexcept
	{
		if (m_pMaterialBlock->RenderPasses[Pass].pPixelShader)
			m_pMaterialBlock->RenderPasses[Pass].pPixelShader->AddRef();

		return m_pMaterialBlock->RenderPasses[Pass].pPixelShader;
	}

	__FORCEINLINE ID3D11RasterizerState* INL_GetRasterizerState(int32 Pass) noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].pRasterizerState;
	}

	__FORCEINLINE ID3D11BlendState* INL_GetBlendState(int32 Pass) noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].pBlendState;
	}

	__FORCEINLINE ID3D11DepthStencilState* INL_GetDepthStencilState(int32 Pass) noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].pDepthStencilState;
	}

	__FORCEINLINE const FLOAT4& INL_GetBlendFactor(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].BlendFactor;
	}

	__FORCEINLINE UINT32 INL_GetSampleMask(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].SampleMask;
	}

	__FORCEINLINE UINT8 INL_GetStencilRef(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].StencilRef;
	}

	__FORCEINLINE const FLOAT4& INL_GetColor(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].Color;
	}

	__FORCEINLINE UINT64 INL_GetRasterizerStateHash(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].RasterizerStateHash;
	}

	__FORCEINLINE UINT64 INL_GetBlendStateHash(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].BlendStateHash;
	}

	__FORCEINLINE UINT64 INL_GetDepthStencilStateHash(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].DepthStencilStateHash;
	}

private:
	D3D11SurfaceMaterial() = delete;
	virtual ~D3D11SurfaceMaterial() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	D3D11_SURFACE_MATERIAL_BLOCK* m_pMaterialBlock = nullptr;

};
