#pragma once


namespace mark
{
	enum class GPU_BUFFER_LAYOUT : uint32_t
	{
		MERGED,
		SEPARATE,
	};

	struct IMaterial : public Unknown
	{
		virtual int32_t AddPass(const char* szPassName) = 0;
		virtual uint32_t GetNumPasses() const noexcept = 0;

		virtual void SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader) = 0;
		virtual void SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader) = 0;

		virtual void RS_GetRasterizerState(int32_t Pass, RS_RASTERIZER_STATE& RasterizerState) = 0;
		virtual void RS_GetBlendState(int32_t Pass, RS_BLEND_STATE& BlendState) = 0;
		virtual void RS_GetDepthStencilState(int32_t Pass, RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;
		virtual FLOAT4 RS_GetBlendFactor(int32_t Pass) = 0;
		virtual uint32_t RS_GetSampleMask(int32_t Pass) = 0;
		virtual uint32_t RS_GetStencilRef(int32_t Pass) = 0;
	};

	struct ISurfaceMaterial : public Unknown
	{
		virtual void SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader) = 0;
		virtual void SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader) = 0;

		virtual void RS_SetRasterizerState(int32_t Pass, const RS_RASTERIZER_STATE& RasterizerState) = 0;
		virtual void RS_SetBlendState(int32_t Pass, const RS_BLEND_STATE& BlendState) = 0;
		virtual void RS_SetDepthStencilState(int32_t Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;
		virtual void RS_SetBlendFactor(int32_t Pass, const FLOAT4& BlendFactor) = 0;
		virtual void RS_SetSampleMask(int32_t Pass, uint32_t SampleMask) = 0;
		virtual void RS_SetStencilRef(int32_t Pass, uint32_t StencilRef) = 0;

		virtual void CB_SetConstant(int32_t Pass, uint32_t Slot, const void* pData, size_t DataSize) = 0;

	};

	struct IModelAsset;

	struct IGPUGeometry : public Unknown
	{
		virtual bool Create(IModelAsset* pModelAsset, GPU_BUFFER_LAYOUT BufferLayout, BOOL HasModelAsset) = 0;
	};

	struct IModelInstance : public Unknown
	{
	};

}
