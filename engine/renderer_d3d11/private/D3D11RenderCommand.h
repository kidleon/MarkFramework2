#pragma once

#include "D3D11RenderPipeline.h"


// 기본 메시 드로우 커맨더
constexpr size_t MAX_VERTEX_SHADER_INDEX = 4096;
constexpr size_t MAX_PIXEL_SHADER_INDEX = 4096;


struct D3D11_DRAW_COMMAND
{
	struct DRAW_SORT_KEY
	{
		union
		{
			uint64 Hash;
			struct
			{
				uint64 Pass : 4; // 16
				uint64 VertexShaderIndex : 12; // 4096
				uint64 PixelShaderIndex : 12; // 4096
				uint64 RenderStateHash : 14; // 16384
				uint64 Depth : 14; // 16384
				uint64 Reserved : 8;
			};
		};
	};

	DRAW_SORT_KEY SortKey;
	D3D11_RENDER_PIPELINE RenderPipeline;
	D3D11_DYNAMIC_RENDER_PIPELINE DynamicRenderPipeline;
	IPrimitiveBuffer* pPrimitiveBuffer;
	uint32 DrawPrimitiveIndex;
	uint32 PADDING;
	
	LINK_NODE LinkNode;

	void Reset();
	
};