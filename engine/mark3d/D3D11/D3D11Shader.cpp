#include "pch.h"
#include "D3D11Shader.h"


D3D11Shader::D3D11Shader(
	const NameHash& ShaderName,
	D3D11ShaderParamTable* pParamTable,
	ID3D11VertexShader* pVS
)
	: m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::VERTEX)
	, m_pParamTable(pParamTable)
	, m_pVS(pVS)
{
	if (m_pVS)
		m_pVS->AddRef();
}

D3D11Shader::D3D11Shader(
	const NameHash& ShaderName,
	D3D11ShaderParamTable* pParamTable,
	ID3D11PixelShader* pPS
)
	: m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::PIXEL)
	, m_pParamTable(pParamTable)
	, m_pPS(pPS)
{
	if (m_pPS)
		m_pPS->AddRef();
}

D3D11Shader::D3D11Shader(
	const NameHash& ShaderName,
	D3D11ShaderParamTable* pParamTable,
	ID3D11ComputeShader* pCS
)
	: m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::COMPUTE)
	, m_pParamTable(pParamTable)
	, m_pCS(pCS)
{
	if (m_pCS)
		m_pCS->AddRef();
}

D3D11Shader::~D3D11Shader() noexcept
{
	switch (m_ShaderType)
	{
		case SHADER_TYPE::VERTEX:
			if (m_pVS)
			{
				m_pVS->Release();
				m_pVS = nullptr;
			}
			break;
		case SHADER_TYPE::PIXEL:
			if (m_pPS)
			{
				m_pPS->Release();
				m_pPS = nullptr;
			}
			break;
		case SHADER_TYPE::COMPUTE:
			if (m_pCS)
			{
				m_pCS->Release();
				m_pCS = nullptr;
			}
			break;
		default:
			break;
	}
}


