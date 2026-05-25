#pragma once
#include "TMemoryBlockPool.h"


namespace mark
{
	class SurfaceMaterial final : public ISurfaceMaterial
	{
		friend class TMemoryBlockPool<SurfaceMaterial>;

	public:
		SurfaceMaterial(TMemoryBlockPool<SurfaceMaterial>* pPool);

		virtual void AddRef();
		virtual void Release();

		virtual void SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader);
		virtual void SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader);

		virtual void RS_SetRasterizerState(int32_t Pass, const RS_RASTERIZER_STATE& RasterizerState);
		virtual void RS_SetBlendState(int32_t Pass, const RS_BLEND_STATE& BlendState);
		virtual void RS_SetDepthStencilState(int32_t Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState);
		virtual void RS_SetBlendFactor(int32_t Pass, const FLOAT4& BlendFactor);
		virtual void RS_SetSampleMask(int32_t Pass, uint32_t SampleMask);
		virtual void RS_SetStencilRef(int32_t Pass, uint32_t StencilRef);

		virtual void CB_SetConstant(int32_t Pass, uint32_t Slot, const void* pData, size_t DataSize);

		inline void SetMemoryBlockId(memory_block_id blockId) { m_BlockId = blockId; }
		inline memory_block_id GetMemoryBlockId() const { return m_BlockId; }

	private:
		SurfaceMaterial() = delete;
		~SurfaceMaterial();

	private:
		std::atomic<int32_t> m_RefCnt{ 1 };
		TMemoryBlockPool<SurfaceMaterial>* m_pPool = nullptr;
		
		memory_block_id m_BlockId{};

	};
}

