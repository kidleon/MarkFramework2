#include "pch.h"
#include "D3D11ShaderProgram.h"
#include "D3D11RenderResources.h"


D3D11ShaderProgram::D3D11ShaderProgram(
	D3D11VertexShader* pVS,
	D3D11PixelShader* pPS
)
	: m_pVS(pVS)
	, m_pPS(pPS)
{
	m_pVS->AddRef();
	m_pPS->AddRef();
}


D3D11ShaderProgram::~D3D11ShaderProgram() noexcept
{
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
}

void D3D11ShaderProgram::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11ShaderProgram);
}

int32 D3D11ShaderProgram::VS_GetBindIndexByName(const NameHash& Name) const
{
	return 0;
}

int32 D3D11ShaderProgram::PS_GetBindIndexByName(const NameHash& Name) const
{
	return 0;
}

void D3D11ShaderProgram::VS_SetConstant(const NameHash& Name, const void* pData, uint32 DataSize)
{

}

void D3D11ShaderProgram::VS_SetConstant(int32 BindIndex, const void* pData, uint32 DataSize)
{

}

void D3D11ShaderProgram::PS_SetConstant(const NameHash& Name, const void* pData, uint32 DataSize)
{

}

void D3D11ShaderProgram::PS_SetConstant(int32 BindIndex, const void* pData, uint32 DataSize)
{

}

void D3D11ShaderProgram::PS_SetTexture1D(const NameHash& Name, ITexture1D* pTexture)
{

}

void D3D11ShaderProgram::PS_SetTexture1D(int32 BindIndex, ITexture1D* pTexture)
{

}

void D3D11ShaderProgram::PS_SetTexture2D(const NameHash& Name, ITexture2D* pTexture)
{

}

void D3D11ShaderProgram::PS_SetTexture2D(int32 BindIndex, ITexture2D* pTexture)
{

}
