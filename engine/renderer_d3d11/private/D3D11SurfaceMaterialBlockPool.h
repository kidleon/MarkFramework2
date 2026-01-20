#pragma once


struct D3D11_SURFACE_MATERIAL_BLOCK;

class D3D11SurfaceMaterialBlockPool
{
	static D3D11SurfaceMaterialBlockPool* s_pInstance;

public:
	D3D11SurfaceMaterialBlockPool();
	~D3D11SurfaceMaterialBlockPool() noexcept;

	BOOL Init(size_t InitialCapacity);
	void Cleanup();

	D3D11_SURFACE_MATERIAL_BLOCK* Acquire();
	void Release(D3D11_SURFACE_MATERIAL_BLOCK* pCommand);

	static inline D3D11SurfaceMaterialBlockPool* Get() noexcept { return s_pInstance; }

private:
	void ExpandPool(size_t Count);

private:
	LINKED_LIST m_FreeList = {};
	LINKED_LIST m_UsedList = {};

};