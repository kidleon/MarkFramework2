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
	virtual void CreateFromModelAsset(IModelAsset* pModelAsset) noexcept = 0;

	virtual size_t GetNumMesh() const noexcept = 0;

	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 VertexFormat, size_t IndexStride, size_t NumVertex, size_t NumIndex) noexcept = 0;
	virtual int32 AddSubmesh(int32 MeshIndex, size_t NumVertex, size_t NumIndex) noexcept = 0;
	virtual int32 AddSurfaceMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept = 0;

	virtual void SetPosition(int32 MeshIndex, FLOAT3* pPositions, size_t NumPosition) noexcept = 0;
	virtual void SetNormal(int32 MeshIndex, FLOAT3* pNormals, size_t NumNormal) noexcept = 0;
	virtual void SetTexCoord(int32 MeshIndex, FLOAT2* pTexCoords, size_t NumTexCoord) noexcept = 0;
	virtual void SetColor(int32 MeshIndex, FLOAT4* pColors, size_t NumColor) noexcept = 0;
	virtual void SetTangent(int32 MeshIndex, FLOAT3* pTangents, size_t NumTangent) noexcept = 0;
	virtual void SetBinormal(int32 MeshIndex, FLOAT3* pBinormals, size_t NumBinormal) noexcept = 0;

	virtual void SetPosition(NameHash Name, FLOAT3* pPositions, size_t NumPosition) noexcept = 0;
	virtual void SetNormal(NameHash Name, FLOAT3* pNormals, size_t NumNormal) noexcept = 0;
	virtual void SetTexCoord(NameHash Name, FLOAT2* pTexCoords, size_t NumTexCoord) noexcept = 0;
	virtual void SetColor(NameHash Name, FLOAT4* pColors, size_t NumColor) noexcept = 0;
	virtual void SetTangent(NameHash Name, FLOAT3* pTangents, size_t NumTangent) noexcept = 0;
	virtual void SetBinormal(NameHash Name, FLOAT3* pBinormals, size_t NumBinormal) noexcept = 0;

	virtual void SetIndex(int32 MeshIndex, int32 SubMeshIndex, uint32* pIndices, size_t NumIndex) noexcept = 0;
	virtual void SetIndex(NameHash Name, int32 SubMeshIndex, uint32* pIndices, size_t NumIndex) noexcept = 0;

};


#endif // __GEOMETRY_DEF_H__
