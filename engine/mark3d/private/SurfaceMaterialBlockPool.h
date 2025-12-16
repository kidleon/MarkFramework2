#ifndef __SURFACE_MATERIAL_BLOCK_POOL_H__
#define __SURFACE_MATERIAL_BLOCK_POOL_H__

#include "linked_list.h"


struct SURFACE_MATERIAL_BLOCK;

class SurfaceMaterialBlockPool
{
public:
	static void Init();
	static void Shutdown();

	static SURFACE_MATERIAL_BLOCK* Alloc();
	static void Release(SURFACE_MATERIAL_BLOCK* pBlock);

private:
	static void AllocPool();

private:
	static LINKED_LIST m_BlockPool;

};


#endif // __SURFACE_MATERIAL_BLOCK_POOL_H__
