#ifndef __D3D11_SURFACE_MATERIAL_BLOCK_POOL_H__
#define __D3D11_SURFACE_MATERIAL_BLOCK_POOL_H__

#include "linked_list.h"


struct D3D11_SURFACE_MATERIAL_BLOCK;

class D3D11SurfaceMaterialBlockPool
{
public:
	static void Init();
	static void Shutdown();

	static D3D11_SURFACE_MATERIAL_BLOCK* Alloc();
	static void Release(D3D11_SURFACE_MATERIAL_BLOCK* pBlock);

private:
	static void AllocPool();

private:
	static LINKED_LIST m_BlockPool;

};


#endif // __D3D11_SURFACE_MATERIAL_BLOCK_POOL_H__
