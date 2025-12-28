#ifndef __D3D11_SURFACE_MATERIAL_BLOCK_H__
#define __D3D11_SURFACE_MATERIAL_BLOCK_H__

#include "D3D11RenderPass.h"


struct D3D11_SURFACE_MATERIAL_BLOCK
{
	D3D11_RENDER_PASS RenderPasses[MAX_RENDER_PASS];
	size_t NumRenderPass;
	LINK_NODE LinkNode;

	void Reset();
};


#endif // __D3D11_SURFACE_MATERIAL_BLOCK_H__
