#include "pch.h"
#include "D3D11ShaderProgram.h"
#include "D3D11RenderDevice.h"


D3D11ShaderProgram::~D3D11ShaderProgram() noexcept
{
	/*
	if (m_pVS)
	{
		m_pVS->Release();
		m_pVS = nullptr;
	}

	if (m_pPS)
	{
		m_pPS->Release();
		m_pPS = nullptr;
	}
	*/
}

long D3D11ShaderProgram::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11ShaderProgram::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		MARK_POOL_DELETE(this, D3D11ShaderProgram);
	}
	return NewRefCnt;
}

long D3D11ShaderProgram::RefCnt()
{
	return m_RefCnt;
}

UINT32 D3D11ShaderProgram::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11ShaderProgram::GetAssetType() const noexcept
{
	return ASSET_TYPE::SHADER_PROGRAM;
}

LOAD_STAT D3D11ShaderProgram::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

int32 D3D11ShaderProgram::GetBindIndexByName(const NameHash& Name) const
{
	/*
	if (m_pVS)
	{
		const D3D11_SHADER_PARAMS* pParams = m_pVS->pShaderParams->INL_GetShaderParamByName(Name);
		if (!pParams) return -1;
		return pParams->BindPoint;
	}

	if (m_pPS)
	{
		const D3D11_SHADER_PARAMS* pParams = m_pPS->pShaderParams->INL_GetShaderParamByName(Name);
		if (!pParams) return -1;
		return pParams->BindPoint;
	}
	*/

	return -1;
}

/*
void D3D11ShaderProgram::SetConstant(const NameHash& Name, const void* pData, uint32 DataSize)
{
	ID3D11Device* pD3D11Device = D3D11GlobalVars::RENDER_DEVICE->INL_GetD3D11Device();
}

void D3D11ShaderProgram::SetConstant(int32 BindIndex, const void* pData, uint32 DataSize)
{
	ID3D11Device* pD3D11Device = D3D11GlobalVars::RENDER_DEVICE->INL_GetD3D11Device();
}


void D3D11ShaderProgram::SetTexture1D(const NameHash& Name, ITexture1D* pTexture)
{
	ID3D11Device* pD3D11Device = D3D11GlobalVars::RENDER_DEVICE->INL_GetD3D11Device();
}

void D3D11ShaderProgram::SetTexture1D(int32 BindIndex, ITexture1D* pTexture)
{
	ID3D11Device* pD3D11Device = D3D11GlobalVars::RENDER_DEVICE->INL_GetD3D11Device();
}

void D3D11ShaderProgram::SetTexture2D(const NameHash& Name, ITexture2D* pTexture)
{
	ID3D11Device* pD3D11Device = D3D11GlobalVars::RENDER_DEVICE->INL_GetD3D11Device();
}

void D3D11ShaderProgram::SetTexture2D(int32 BindIndex, ITexture2D* pTexture)
{
	ID3D11Device* pD3D11Device = D3D11GlobalVars::RENDER_DEVICE->INL_GetD3D11Device();
}

*/