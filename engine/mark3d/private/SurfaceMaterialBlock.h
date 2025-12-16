#ifndef __SURFACE_MATERIAL_BLOCK_H__
#define __SURFACE_MATERIAL_BLOCK_H__

#include "RenderPass.h"


struct SURFACE_MATERIAL_BLOCK
{
	RENDER_PASS RenderPasses[MAX_RENDER_PASS];
	size_t NumRenderPass;
	LINK_NODE LinkNode;

	void Reset();
};


#endif // __SURFACE_MATERIAL_BLOCK_H__
