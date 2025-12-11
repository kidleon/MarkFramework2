#include "pch.h"
#include "D3D11VertexShader.h"
#include "D3D11ShaderParams.h"
#include "D3D11InputLayout.h"
#include "idgen.h"
#include "Heap.h"

/*

IMPLEMENTATION_IUNKNOWN_INTERFACE(D3D11VertexShader);

D3D11VertexShader::D3D11VertexShader(UINT32 ID)
	: m_ID(ID)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
	, m_pVS(nullptr)
	, m_pShaderParams(nullptr)
	, m_pInputLayout(nullptr)
{
}

D3D11VertexShader::~D3D11VertexShader() noexcept
{
	idgen_release(GLOBAL_VARS::ID_GEN_HANDLE, m_ID);

	CHECK_RELEASE(m_pVS);
	MARK_POOL_DELETE(m_pShaderParams, D3D11ShaderParams);
	MARK_POOL_DELETE(m_pInputLayout, D3D11InputLayout);
}

void D3D11VertexShader::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11VertexShader);
}

UINT32 D3D11VertexShader::GetID() const noexcept
{
	return INL_GetID();
}

ASSET_TYPE D3D11VertexShader::GetAssetType() const noexcept
{
	return INL_GetAssetType();
}

LOAD_STAT D3D11VertexShader::GetLoadStat() const noexcept
{
	return INL_GetLoadStat();
}

int32 D3D11VertexShader::GetBindIndexByName(const NameHash& Name) const
{
	return m_pShaderParams->INL_GetShaderParamIndexByName(Name);
}

void D3D11VertexShader::SetConstant(const NameHash& Name, const void* pData, uint32 DataSize)
{
}

void D3D11VertexShader::SetConstant(int32 BindIndex, const void* pData, uint32 DataSize)
{

}

*/