#ifndef __D3D11_VERTEX_SHADER_H__
#define __D3D11_VERTEX_SHADER_H__

#include "IVertexShader.h"
//#include "TUnknown.h"

class D3D11ShaderParams;
class D3D11InputLayout;

/*

class D3D11VertexShader : public IVertexShader
{
	DECLARATION_IUNKNOWN_INTERFACE(D3D11VertexShader);

public:
	D3D11VertexShader(UINT32 ID);

	// IAsset interface
	virtual UINT32 GetID() const noexcept override;
	virtual ASSET_TYPE GetAssetType() const noexcept override;
	virtual LOAD_STAT GetLoadStat() const noexcept override;

	// IVertexShader interface
	virtual int32 GetBindIndexByName(const NameHash& Name) const override;
	virtual void SetConstant(const NameHash& Name, const void* pData, uint32 DataSize) override;
	virtual void SetConstant(int32 BindIndex, const void* pData, uint32 DataSize) override;

	// Private inline methods
	__FORCEINLINE void INL_SetData(
		ID3D11VertexShader* pVS,
		D3D11ShaderParams* pShaderParams,
		D3D11InputLayout* pInputLayout
	)
	{
		m_pVS = pVS;
		m_pShaderParams = pShaderParams;
		m_pInputLayout = pInputLayout;
		m_LoadStat = LOAD_STAT::LOADED;
	}

	__FORCEINLINE UINT32 INL_GetID() const noexcept
	{
		return m_ID;
	}

	__FORCEINLINE ASSET_TYPE INL_GetAssetType() const noexcept
	{
		return ASSET_TYPE::VERTEX_SHADER;
	}

	__FORCEINLINE LOAD_STAT INL_GetLoadStat() const noexcept
	{
		return m_LoadStat;
	}

	__FORCEINLINE ID3D11VertexShader* INL_GetVS() const noexcept
	{
		return m_pVS;
	}

	__FORCEINLINE D3D11ShaderParams* INL_GetShaderProp() const noexcept
	{
		return m_pShaderParams;
	}

	__FORCEINLINE D3D11InputLayout* INL_GetInputLayout() const noexcept
	{
		return m_pInputLayout;
	}

private:
	ID3D11VertexShader* m_pVS;
	D3D11ShaderParams* m_pShaderParams;
	D3D11InputLayout* m_pInputLayout;
	uint32 m_ID;
	LOAD_STAT m_LoadStat;

};
*/

#endif // __D3D11_VERTEX_SHADER_H__
