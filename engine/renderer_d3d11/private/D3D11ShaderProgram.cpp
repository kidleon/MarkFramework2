#include "pch.h"
#include "D3D11ShaderProgram.h"


D3D11ShaderProgram::D3D11ShaderProgram(
	UINT64 ID,
	NameHash ShaderName,
	ID3D11VertexShader* pVertexShader
)
	: m_ID(ID)
	, m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::VERTEX)
	, m_pVertexShader(pVertexShader)
{
	m_LoadStat = LOAD_STAT::LOADED;
	m_HashNode.data = this;
}

D3D11ShaderProgram::D3D11ShaderProgram(
	UINT64 ID,
	NameHash ShaderName,
	ID3D11PixelShader* pPixelShader
)
	: m_ID(ID)
	, m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::PIXEL)
	, m_pPixelShader(pPixelShader)
{
	m_LoadStat = LOAD_STAT::LOADED;
	m_HashNode.data = this;
}

D3D11ShaderProgram::D3D11ShaderProgram(
	UINT64 ID,
	NameHash ShaderName,
	ID3D11GeometryShader* pGeometryShader
)
	: m_ID(ID)
	, m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::UNKNOWN)
	, m_pGeometryShader(pGeometryShader)
{
	m_LoadStat = LOAD_STAT::LOADED;
	m_HashNode.data = this;
}

D3D11ShaderProgram::D3D11ShaderProgram(
	UINT64 ID,
	NameHash ShaderName,
	ID3D11ComputeShader* pComputeShader
)
	: m_ID(ID)
	, m_ShaderName(ShaderName)
	, m_ShaderType(SHADER_TYPE::COMPUTE)
	, m_pComputeShader(pComputeShader)
{
	m_LoadStat = LOAD_STAT::LOADED;
	m_HashNode.data = this;
}

D3D11ShaderProgram::~D3D11ShaderProgram() noexcept
{
	switch (m_ShaderType)
	{
	case SHADER_TYPE::VERTEX:
		CHECK_RELEASE(m_pVertexShader);
		break;
	case SHADER_TYPE::PIXEL:
		CHECK_RELEASE(m_pPixelShader);
		break;
	case SHADER_TYPE::UNKNOWN:
		CHECK_RELEASE(m_pGeometryShader);
		break;
	case SHADER_TYPE::COMPUTE:
		CHECK_RELEASE(m_pComputeShader);
		break;
	default:
		break;
	}
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
		D3D11_POOL_DELETE(this, D3D11ShaderProgram);
	}
	return NewRefCnt;
}

long D3D11ShaderProgram::RefCnt()
{
	return m_RefCnt;
}

UINT64 D3D11ShaderProgram::GetID() const noexcept
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

SHADER_TYPE D3D11ShaderProgram::GetShaderType() const noexcept
{
	return m_ShaderType;
}
