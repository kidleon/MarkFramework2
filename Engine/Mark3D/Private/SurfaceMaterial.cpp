#include "pch.h"
#include "SurfaceMaterial.h"
#include "SurfaceMaterialPool.h"


namespace mark
{
	SurfaceMaterial::SurfaceMaterial(SurfaceMaterialPool* pPool, memory_block_id BlockId)
		: m_pPool(pPool)
		, m_BlockId(BlockId)
	{
	}

	SurfaceMaterial::~SurfaceMaterial()
	{
	}

	void SurfaceMaterial::AddRef()
	{
		m_RefCnt.fetch_add(1, std::memory_order_relaxed);
	}

	void SurfaceMaterial::Release()
	{
		if (m_RefCnt.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			m_pPool->Release(this);
		}
	}

	void SurfaceMaterial::SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader)
	{

	}

	void  SurfaceMaterial::SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader)
	{

	}

	void SurfaceMaterial::RS_SetRasterizerState(int32_t Pass, const RS_RASTERIZER_STATE& RasterizerState)
	{

	}

	void SurfaceMaterial::RS_SetBlendState(int32_t Pass, const RS_BLEND_STATE& BlendState)
	{

	}

	void SurfaceMaterial::RS_SetDepthStencilState(int32_t Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState)
	{

	}

	void SurfaceMaterial::RS_SetBlendFactor(int32_t Pass, const FLOAT4& BlendFactor)
	{

	}

	void SurfaceMaterial::RS_SetSampleMask(int32_t Pass, uint32_t SampleMask)
	{

	}

	void SurfaceMaterial::RS_SetStencilRef(int32_t Pass, uint32_t StencilRef)
	{

	}

	void SurfaceMaterial::CB_SetConstant(int32_t Pass, uint32_t Slot, const void* pData, size_t DataSize)
	{


	}
}
