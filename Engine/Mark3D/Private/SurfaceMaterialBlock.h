#pragma once


namespace mark
{
	struct SurfaceMaterialBlock
	{
		unknown_ptr<IShaderProgram> pVertexShader;
		unknown_ptr<IShaderProgram> pPixelShader;

		RS_BLEND_STATE BlendState;
		RS_RASTERIZER_STATE RasterizerState;
		RS_DEPTH_STENCIL_STATE DepthStencilState;

		FLOAT4 BlendFactor;
		UINT32 SampleMask;
		UINT32 StencilRef;
		FLOAT4 Color;

		~SurfaceMaterialBlock() noexcept
		{
			pVertexShader.reset();
			pPixelShader.reset();
			memset(&BlendState, 0, sizeof(BlendState));
			memset(&RasterizerState, 0, sizeof(RasterizerState));
			memset(&DepthStencilState, 0, sizeof(DepthStencilState));
			BlendFactor = { 0, 0, 0, 0 };
			SampleMask = 0;
			StencilRef = 0;
			Color = { 0, 0, 0, 0 };
		}
	};
}
