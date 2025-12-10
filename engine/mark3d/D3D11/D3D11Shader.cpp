#include "pch.h"
#include "D3D11Shader.h"
#include "D3D11ShaderProp.h"
#include "D3D11InputLayout.h"


D3D11VertexShader::D3D11VertexShader(
	ID3D11VertexShader* pVS,
	D3D11ShaderProp* pShaderProp,
	D3D11InputLayout* pInputLayout
)
	: m_pVS(pVS)
	, m_pShaderProp(pShaderProp)
	, m_pInputLayout(pInputLayout)
{
}

D3D11VertexShader::~D3D11VertexShader()
{
	CHECK_RELEASE(m_pVS);
	MARK_POOL_DELETE(m_pShaderProp, D3D11ShaderProp);
	MARK_POOL_DELETE(m_pInputLayout, D3D11InputLayout);
}


//---------------------------------------------------------------------
D3D11PixelShader::D3D11PixelShader(
	ID3D11PixelShader* pPS,
	D3D11ShaderProp* pShaderProp
)
	: m_pPS(pPS)
	, m_pShaderProp(pShaderProp)
{
}

D3D11PixelShader::~D3D11PixelShader() noexcept
{
	CHECK_RELEASE(m_pPS);
	MARK_POOL_DELETE(m_pShaderProp, D3D11ShaderProp);
}


//---------------------------------------------------------------------
D3D11ComputeShader::D3D11ComputeShader(
	ID3D11ComputeShader* pCS,
	D3D11ShaderProp* pShaderProp
)
	: m_pCS(pCS)
	, m_pShaderProp(pShaderProp)
{
}

D3D11ComputeShader::~D3D11ComputeShader() noexcept
{
	CHECK_RELEASE(m_pCS);
	MARK_POOL_DELETE(m_pShaderProp, D3D11ShaderProp);
}