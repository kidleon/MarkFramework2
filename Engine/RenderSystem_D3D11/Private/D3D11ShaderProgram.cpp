#include "pch.h"
#include "D3D11ShaderProgram.h"


namespace mark
{
	D3D11ShaderProgram::D3D11ShaderProgram(const char* szShaderName, ID3D11VertexShader* pVertexShader)
		: m_ShaderType(SHADER_TYPE::VERTEX_SHADER)
		, m_pVertexShader(pVertexShader)
		, m_HashName(szShaderName)
	{
#if defined(_DEBUG)
		safe_strcpy(m_szShaderName, MAX_SHADER_NAME, szShaderName);
#endif // #if defined(_DEBUG)
	}

	D3D11ShaderProgram::D3D11ShaderProgram(const char* szShaderName, ID3D11PixelShader* pPixelShader)
		: m_ShaderType(SHADER_TYPE::PIXEL_SHADER)
		, m_pPixelShader(pPixelShader)
		, m_HashName(szShaderName)
	{
#if defined(_DEBUG)
		safe_strcpy(m_szShaderName, MAX_SHADER_NAME, szShaderName);
#endif // #if defined(_DEBUG)
	}

	D3D11ShaderProgram::D3D11ShaderProgram(const char* szShaderName, ID3D11ComputeShader* pComputeShader)
		: m_ShaderType(SHADER_TYPE::COMPUTE_SHADER)
		, m_pComputeShader(pComputeShader)
		, m_HashName(szShaderName)
	{
#if defined(_DEBUG)
		safe_strcpy(m_szShaderName, MAX_SHADER_NAME, szShaderName);
#endif // #if defined(_DEBUG)
	}

	D3D11ShaderProgram::~D3D11ShaderProgram() noexcept
	{
		CHECK_RELEASE(m_pBaseShader);
	}

	void D3D11ShaderProgram::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void D3D11ShaderProgram::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(D3D11ShaderProgram, this);
		}
	}

	SHADER_TYPE D3D11ShaderProgram::GetShaderType() const noexcept
	{
		return m_ShaderType;
	}
}
