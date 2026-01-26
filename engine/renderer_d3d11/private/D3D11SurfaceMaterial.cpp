#include "pch.h"
#include "D3D11SurfaceMaterial.h"
#include "D3D11SurfaceMaterialBlockPool.h"
#include "D3D11RenderStateCache.h"
#include "D3D11RenderState.h"


D3D11SurfaceMaterial::D3D11SurfaceMaterial(UINT64 ID, D3D11_SURFACE_MATERIAL_BLOCK* pMaterialBlock)
	: m_ID(ID)
	, m_LoadStat(LOAD_STAT::LOADED)
	, m_pMaterialBlock(pMaterialBlock)
{
}

D3D11SurfaceMaterial::~D3D11SurfaceMaterial() noexcept
{
	if (m_pMaterialBlock)
	{
		D3D11SurfaceMaterialBlockPool::Get()->Release(m_pMaterialBlock);
		m_pMaterialBlock = nullptr;
	}
}

long D3D11SurfaceMaterial::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11SurfaceMaterial::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11SurfaceMaterial);
	}
	return NewRefCnt;
}

long D3D11SurfaceMaterial::RefCnt()
{
	return m_RefCnt;
}

UINT64 D3D11SurfaceMaterial::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11SurfaceMaterial::GetAssetType() const noexcept
{
	return ASSET_TYPE::SURFACE_MATERIAL;
}

LOAD_STAT D3D11SurfaceMaterial::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

int32 D3D11SurfaceMaterial::AddPass(const char* szPassName) noexcept
{
	int32 NewPassIndex = (int32)m_pMaterialBlock->NumPasses;

	if (NewPassIndex >= MAX_RENDER_PASS)
		return -1;

	fstrlcpy(
		m_pMaterialBlock->RenderPasses[NewPassIndex].PassName, 
		szPassName, 
		MAX_RENDER_PASS_NAME
	);

	m_pMaterialBlock->NumPasses++;

	return NewPassIndex;
}

int32 D3D11SurfaceMaterial::GetNumPass() const noexcept
{
	return (int32)m_pMaterialBlock->NumPasses;
}

void D3D11SurfaceMaterial::SetVertexShader(int32 Pass, IShaderProgram* pVS)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	if (pVS)
		pVS->AddRef();

	CHECK_RELEASE(m_pMaterialBlock->RenderPasses[Pass].pVertexShader);

	m_pMaterialBlock->RenderPasses[Pass].pVertexShader = (D3D11ShaderProgram*)pVS;
}

void D3D11SurfaceMaterial::SetVertexShader(IShaderProgram* pVS)
{
	if (!m_pMaterialBlock->NumPasses)
		return;

	if (pVS)
		pVS->AddRef();

	CHECK_RELEASE(m_pMaterialBlock->RenderPasses[0].pVertexShader);

	m_pMaterialBlock->RenderPasses[0].pVertexShader = (D3D11ShaderProgram*)pVS;
}

void D3D11SurfaceMaterial::SetPixelShader(int32 Pass, IShaderProgram* pPS)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	if (pPS)
		pPS->AddRef();

	CHECK_RELEASE(m_pMaterialBlock->RenderPasses[Pass].pPixelShader);

	m_pMaterialBlock->RenderPasses[Pass].pPixelShader = (D3D11ShaderProgram*)pPS;
}

void D3D11SurfaceMaterial::SetPixelShader(IShaderProgram* pPS)
{
	if (!m_pMaterialBlock->NumPasses)
		return;

	if (pPS)
		pPS->AddRef();

	CHECK_RELEASE(m_pMaterialBlock->RenderPasses[0].pPixelShader);

	m_pMaterialBlock->RenderPasses[0].pPixelShader = (D3D11ShaderProgram*)pPS;
}

IShaderProgram* D3D11SurfaceMaterial::GetVertexShader(int32 Pass) noexcept
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return nullptr;

	return m_pMaterialBlock->RenderPasses[Pass].pVertexShader;
}

IShaderProgram* D3D11SurfaceMaterial::GetVertexShader() noexcept
{
	if (!m_pMaterialBlock->NumPasses)
		return nullptr;

	return m_pMaterialBlock->RenderPasses[0].pVertexShader;
}

IShaderProgram* D3D11SurfaceMaterial::GetPixelShader(int32 Pass) noexcept
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return nullptr;

	return m_pMaterialBlock->RenderPasses[Pass].pPixelShader;
}

IShaderProgram* D3D11SurfaceMaterial::GetPixelShader() noexcept
{
	if (!m_pMaterialBlock->NumPasses)
		return nullptr;

	return m_pMaterialBlock->RenderPasses[0].pPixelShader;
}

void D3D11SurfaceMaterial::SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	uint64 Hash = fnv64_c(&RasterizerState, sizeof(RS_RASTERIZER_STATE));
	
	if (m_pMaterialBlock->RenderPasses[Pass].RasterizerStateHash != Hash)
	{
		m_pMaterialBlock->RenderPasses[Pass].RasterizerStateHash = Hash;
		m_pMaterialBlock->RenderPasses[Pass].pRasterizerState = D3D11RenderStateCache::Get()->Find_RS(Hash);

		if (!m_pMaterialBlock->RenderPasses[Pass].pRasterizerState)
		{
			m_pMaterialBlock->RenderPasses[Pass].pRasterizerState = D3D11RenderStateCache::Get()->Register(RasterizerState);
		}
	}
}

void D3D11SurfaceMaterial::SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState)
{
	if (!m_pMaterialBlock->NumPasses)
		return;
	SetRasterizerState(0, RasterizerState);
}

void D3D11SurfaceMaterial::SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	uint64 Hash = fnv64_c(&BlendState, sizeof(RS_BLEND_STATE));
	if (m_pMaterialBlock->RenderPasses[Pass].BlendStateHash != Hash)
	{
		m_pMaterialBlock->RenderPasses[Pass].BlendStateHash = Hash;
		m_pMaterialBlock->RenderPasses[Pass].pBlendState = D3D11RenderStateCache::Get()->Find_BS(Hash);

		if (!m_pMaterialBlock->RenderPasses[Pass].pBlendState)
		{
			m_pMaterialBlock->RenderPasses[Pass].pBlendState = D3D11RenderStateCache::Get()->Register(BlendState);
		}
	}
}
void D3D11SurfaceMaterial::SetBlendState(const RS_BLEND_STATE& BlendState)
{
	if (!m_pMaterialBlock->NumPasses)
		return;

	SetBlendState(0, BlendState);
}

void D3D11SurfaceMaterial::SetBlendFactor(int32 Pass, const FLOAT4& BlendFactor)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	m_pMaterialBlock->RenderPasses[Pass].BlendFactor = BlendFactor;
}

void D3D11SurfaceMaterial::SetBlendFactor(const FLOAT4& BlendFactor)
{
	if (!m_pMaterialBlock->NumPasses)
		return;

	m_pMaterialBlock->RenderPasses[0].BlendFactor = BlendFactor;
}

void D3D11SurfaceMaterial::SetSampleMask(int32 Pass, UINT32 SampleMask)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;
	m_pMaterialBlock->RenderPasses[Pass].SampleMask = SampleMask;
}

void D3D11SurfaceMaterial::SetSampleMask(UINT32 SampleMask)
{
	if (!m_pMaterialBlock->NumPasses)
		return;
	m_pMaterialBlock->RenderPasses[0].SampleMask = SampleMask;
}

void D3D11SurfaceMaterial::SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	uint64 Hash = fnv64_c(&DepthStencilState, sizeof(RS_DEPTH_STENCIL_STATE));
	if (m_pMaterialBlock->RenderPasses[Pass].DepthStencilStateHash != Hash)
	{
		m_pMaterialBlock->RenderPasses[Pass].DepthStencilStateHash = Hash;
		m_pMaterialBlock->RenderPasses[Pass].pDepthStencilState = D3D11RenderStateCache::Get()->Find_DSS(Hash);

		if (!m_pMaterialBlock->RenderPasses[Pass].pDepthStencilState)
		{
			m_pMaterialBlock->RenderPasses[Pass].pDepthStencilState = D3D11RenderStateCache::Get()->Register(DepthStencilState);
		}
	}
}

void D3D11SurfaceMaterial::SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState)
{
	if (!m_pMaterialBlock->NumPasses)
		return;

	SetDepthStencilState(0, DepthStencilState);
}

void D3D11SurfaceMaterial::SetStencilRef(int32 Pass, UINT8 StencilRef)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;
	m_pMaterialBlock->RenderPasses[Pass].StencilRef = StencilRef;
}

void D3D11SurfaceMaterial::SetStencilRef(UINT8 StencilRef)
{
	if (!m_pMaterialBlock->NumPasses)
		return;
	m_pMaterialBlock->RenderPasses[0].StencilRef = StencilRef;
}

void D3D11SurfaceMaterial::SetColor(int32 Pass, const FLOAT4& Color)
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
		return;

	m_pMaterialBlock->RenderPasses[Pass].Color = Color;
}

void D3D11SurfaceMaterial::SetColor(const FLOAT4& Color)
{
	if (!m_pMaterialBlock->NumPasses)
		return;

	m_pMaterialBlock->RenderPasses[0].Color = Color;
}

const FLOAT4& D3D11SurfaceMaterial::GetColor(int32 Pass) const noexcept
{
	if (!m_pMaterialBlock->NumPasses || Pass < 0 || Pass >= (int32)m_pMaterialBlock->NumPasses)
	{
		static FLOAT4 DefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		return DefaultColor;
	}

	return m_pMaterialBlock->RenderPasses[Pass].Color;
}

const FLOAT4& D3D11SurfaceMaterial::GetColor() const noexcept
{
	if (!m_pMaterialBlock->NumPasses)
	{
		static FLOAT4 DefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		return DefaultColor;
	}

	return m_pMaterialBlock->RenderPasses[0].Color;
}