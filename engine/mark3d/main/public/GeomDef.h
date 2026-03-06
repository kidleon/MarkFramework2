#ifndef __GEOMETRY_DEF_H__
#define __GEOMETRY_DEF_H__


enum class RENDER_TYPE : UINT32
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
	virtual size_t GetNumMesh() const noexcept = 0;
	virtual size_t GetNumSubMesh(int32 MeshIndex) const noexcept = 0;
	virtual size_t GetNumSubMesh(NameHash Name) const noexcept = 0;

	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, UINT32 NumVertex, UINT32 NumIndex) noexcept = 0;
	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, UINT32 NumVertex, UINT32 NumIndices, UINT32* pNumIndices) noexcept = 0;

	virtual void SetMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;
	virtual void SetMaterial(NameHash Name, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;
	virtual void SetMaterial(int32 MeshIndex, int32 SubMeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;
	virtual void SetMaterial(NameHash Name, int32 SubMeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;

	virtual void SetPosition(int32 MeshIndex, FLOAT3* pPositions, UINT32 NumPosition) noexcept = 0;
	virtual void SetNormal(int32 MeshIndex, FLOAT3* pNormals, UINT32 NumNormal) noexcept = 0;
	virtual void SetTexCoord(int32 MeshIndex, FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept = 0;
	virtual void SetColor(int32 MeshIndex, FLOAT4* pColors, UINT32 NumColor) noexcept = 0;
	virtual void SetTangent(int32 MeshIndex, FLOAT3* pTangents, UINT32 NumTangent) noexcept = 0;
	virtual void SetBinormal(int32 MeshIndex, FLOAT3* pBinormals, UINT32 NumBinormal) noexcept = 0;

	virtual void SetPosition(NameHash Name, FLOAT3* pPositions, UINT32 NumPosition) noexcept = 0;
	virtual void SetNormal(NameHash Name, FLOAT3* pNormals, UINT32 NumNormal) noexcept = 0;
	virtual void SetTexCoord(NameHash Name, FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept = 0;
	virtual void SetColor(NameHash Name, FLOAT4* pColors, UINT32 NumColor) noexcept = 0;
	virtual void SetTangent(NameHash Name, FLOAT3* pTangents, UINT32 NumTangent) noexcept = 0;
	virtual void SetBinormal(NameHash Name, FLOAT3* pBinormals, UINT32 NumBinormal) noexcept = 0;

	virtual void SetIndex(int32 MeshIndex, const UINT32* pIndices, UINT32 NumIndex) noexcept = 0;
	virtual void SetIndex(NameHash Name, const UINT32* pIndices, UINT32 NumIndex) noexcept = 0;

	virtual void SetIndex(int32 MeshIndex, UINT32 NumIndex, const UINT32** ppIndices, UINT32* pNumIndices) noexcept = 0;
	virtual void SetIndex(NameHash Name, UINT32 NumIndex, const UINT32** ppIndices, UINT32* pNumIndices) noexcept = 0;
};


#endif // __GEOMETRY_DEF_H__
