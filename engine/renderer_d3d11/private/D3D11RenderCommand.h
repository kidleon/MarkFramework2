#pragma once

#include "D3D11RenderPipeline.h"
#include "D3D11ShaderDef.h"


class D3D11PrimitiveBuffer;

// 기본 메시 드로우 커맨더
constexpr size_t MAX_VERTEX_SHADER_INDEX = 4096;
constexpr size_t MAX_PIXEL_SHADER_INDEX = 4096;

struct D3D11_DRAW_COMMAND
{
	struct DRAW_SORT_KEY
	{
		union
		{
			uint64 Value;
			struct
			{
				uint64 Depth : SORT_DEPTH_BIT; // 512
				uint64 StateIndex : SORT_STATE_INDEX_BIT; // 4096
				uint64 TextureID : SORT_TEXTURE_ID_BIT; // 32768
				uint64 PixelShaderIndex : SORT_PIXEL_SHADER_BIT; // 4096
				uint64 VertexShaderIndex : SORT_VERTEX_SHADER_BIT; // 4096
				uint64 Pass : SORT_PASS_BIT; // 16
			};
		};
	};

	DRAW_SORT_KEY SortKey;
	uint32 DrawPrimitiveIndex;
	D3D11_OBJECT_CONSTANT ObjectConstant;
	D3D11_RENDER_PIPELINE RenderPipeline;
	D3D11_DYNAMIC_RENDER_PIPELINE DynamicRenderPipeline;
	D3D11PrimitiveBuffer* pPrimitiveBuffer;
	
	LINK_NODE LinkNode;
	UINT64 PADDING_OR_RESERVED = 0;

	void Reset();
	
};