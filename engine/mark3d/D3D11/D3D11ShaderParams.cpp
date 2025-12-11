#include "pch.h"
#include "D3D11ShaderParams.h"


D3D11ShaderParams::D3D11ShaderParams(
	D3D11_SHADER_PARAMS* pShaderParams,
	UINT32 NumShaderParams
)
	: m_pShaderParams(pShaderParams)
	, m_NumShaderParams(NumShaderParams)
{
}

D3D11ShaderParams::~D3D11ShaderParams()
{
	if (m_pShaderParams)
	{
		MARK_POOL_FREE(m_pShaderParams);
		m_pShaderParams = nullptr;
	}
}
