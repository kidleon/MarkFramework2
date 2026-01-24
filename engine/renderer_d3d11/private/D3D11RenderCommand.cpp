#include "pch.h"
#include "D3D11RenderCommand.h"
#include "D3D11ShaderProgram.h"
#include "D3D11PrimitiveBuffer.h"


void D3D11_DRAW_COMMAND::Reset()
{
	SortKey.Value = 0;
	CHECK_RELEASE(RenderPipeline.pVertexShader);
	CHECK_RELEASE(RenderPipeline.pPixelShader);
	DynamicRenderPipeline.BlendFactor = FLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	DynamicRenderPipeline.SampleMask = 0xFFFFFFFF;
	
	CHECK_RELEASE(pPrimitiveBuffer);
	DrawPrimitiveIndex = 0;
}