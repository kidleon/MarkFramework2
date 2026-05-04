#pragma once


namespace mark
{
	struct SurfaceMaterialBlock
	{
		IShaderProgram* pVertexShader;
		IShaderProgram* pPixelShader;

		RS_BLEND_STATE BlendState;
		RS_RASTERIZER_STATE RasterizerState;
		RS_DEPTH_STENCIL_STATE DepthStencilState;

		FLOAT4 BlendFactor;
		UINT32 SampleMask;
		UINT32 StencilRef;
		FLOAT4 Color;
	};
}
