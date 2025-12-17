#include "pch.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderParams.h"
#include "D3D11RenderResources.h"
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

void D3D11ShaderProgram::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11ShaderProgram);
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