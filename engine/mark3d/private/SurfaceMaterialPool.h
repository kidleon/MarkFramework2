#ifndef __SURFACE_MATERIAL_POOL_H__
#define __SURFACE_MATERIAL_POOL_H__


class SurfaceMaterial;

class SurfaceMaterialPool
{
public:
	static void Init(size_t InitialCapacity = 32);
	static void Shutdown();

	static SurfaceMaterial* Alloc();
	static void Release(SurfaceMaterial* pMaterial);

private:
	static void AllocBlock();

private:
	static LINKED_LIST m_FreeList;
	static size_t m_Capacity;

};


#endif // __SURFACE_MATERIAL_POOL_H__
