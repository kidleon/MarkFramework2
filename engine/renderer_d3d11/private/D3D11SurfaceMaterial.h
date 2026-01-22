#pragma once

#include "D3D11SurfaceMaterialBlock.h"
#include "D3D11ShaderProgram.h"


class D3D11SurfaceMaterial final : public ISurfaceMaterial
{
public:
	explicit D3D11SurfaceMaterial(UINT64 ID, D3D11_SURFACE_MATERIAL_BLOCK* pMaterialBlock);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

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

	__FORCEINLINE const FLOAT4& INL_GetColor(int32 Pass) const noexcept
	{
		return m_pMaterialBlock->RenderPasses[Pass].Color;
	}

private:
	D3D11SurfaceMaterial() = delete;
	virtual ~D3D11SurfaceMaterial() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	D3D11_SURFACE_MATERIAL_BLOCK* m_pMaterialBlock = nullptr;

};
