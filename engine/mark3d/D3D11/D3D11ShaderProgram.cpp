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