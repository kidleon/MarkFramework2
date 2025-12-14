#ifndef __SURFACE_MATERIAL_POOL_H__
#define __SURFACE_MATERIAL_POOL_H__


class SurfaceMaterial;

class SurfaceMaterialPool
{
public:
	SurfaceMaterialPool() = default;
	~SurfaceMaterialPool();

	void Init(size_t InitialCapacity = 32);

	SurfaceMaterial* Alloc();
	void Release(SurfaceMaterial* pMaterial);

private:
	void AllocBlock();

private:
	LINKED_LIST m_FreeList = {};
	size_t m_Capacity = 0;

};


#endif // __SURFACE_MATERIAL_POOL_H__
