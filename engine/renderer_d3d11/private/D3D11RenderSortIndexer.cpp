#include "pch.h"
#include "D3D11RenderSortIndexer.h"


void RENDER_SORT_INDEXER::Reset()
{
	VertexShaderIndexer.clear();
	PixelShaderIndexer.clear();
	MaterialIndexer.clear();
	RasterizerStateIndexer.clear();
	BlendStateIndexer.clear();
	DepthStencilStateIndexer.clear();
}