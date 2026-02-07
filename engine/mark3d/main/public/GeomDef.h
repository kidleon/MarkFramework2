#ifndef __GEOMETRY_DEF_H__
#define __GEOMETRY_DEF_H__


enum class RENDER_TYPE : uint32
{
	UNKNOWN = 0,
	MODEL = 1,
	MAX
};


struct ISceneObject : public IAsset
{
	virtual void SetEnabled(BOOL Enabled) noexcept = 0;
	virtual BOOL IsEnabled() const noexcept = 0;
};

/**
* @brief 기본 모델 인터페이스
*/
struct IModel : public ISceneObject
{
	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 VertexFormat, size_t IndexStride, size_t NumVertex, size_t NumIndex) noexcept = 0;
	virtual void ClearMesh() noexcept = 0;

	virtual void UpdateVertex(int32 MeshIndex, void* pVertexData, size_t VertexSize) noexcept = 0;
	virtual void UpdateVertex(NameHash Name, void* pVertexData, size_t VertexSize) noexcept = 0;

	virtual void UpdateIndex(int32 MeshIndex, void* pIndexData, size_t IndexSize) noexcept = 0;
	virtual void UpdateIndex(NameHash Name, void* pIndexData, size_t IndexSize) noexcept = 0;

	virtual void SetMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;
	virtual void SetMaterial(NameHash Name, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;

	virtual int32 FindMeshIndexByName(NameHash Name) const noexcept = 0;
	virtual ISurfaceMaterial* GetMaterial(int32 MeshIndex) noexcept = 0;
	virtual ISurfaceMaterial* GetMaterial(NameHash Name) noexcept = 0;

	virtual void SetActiveMesh(int32 MeshIndex, BOOL Active) noexcept = 0;
	virtual void SetActiveMesh(NameHash Name, BOOL Active) noexcept = 0;

	virtual BOOL IsActiveMesh(int32 MeshIndex) const noexcept = 0;
	virtual BOOL IsActiveMesh(NameHash Name) const noexcept = 0;

	virtual size_t GetNumMesh() const noexcept = 0;
};


#endif // __GEOMETRY_DEF_H__
