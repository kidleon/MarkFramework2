#ifndef __GEOMETRY_DEF_H__
#define __GEOMETRY_DEF_H__


enum class RENDER_TYPE : UINT32
{
	UNKNOWN = 0,
	MODEL = 1,
	MAX
};

constexpr static size_t MAX_MESH_PART = 8; // 모델당 최대 메시 수


struct ISceneObject : public IAsset
{
	virtual void SetEnabled(BOOL Enabled) noexcept = 0;
	virtual BOOL IsEnabled() const noexcept = 0;
};

/**
* @brief 기본 메시 인터페이스
*/
struct IMesh : public IUNKNOWN
{
	virtual void SetName(const char* szName) noexcept = 0;
	virtual const NameHash& GetName() const noexcept = 0;

	virtual void SetPosition(const FLOAT3* pPositions, UINT32 NumPosition) noexcept = 0;
	virtual void SetNormal(const FLOAT3* pNormals, UINT32 NumNormal) noexcept = 0;
	virtual void SetTexCoord(const FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept = 0;
	virtual void SetColor(const FLOAT4* pColors, UINT32 NumColor) noexcept = 0;
	virtual void SetTangent(const FLOAT3* pTangents, UINT32 NumTangent) noexcept = 0;
	virtual void SetBinormal(const FLOAT3* pBinormals, UINT NumBinormal) noexcept = 0;

	virtual void SetIndex(const UINT32* pIndices, UINT32 NumIndices) noexcept = 0;
	virtual void SetIndex(UINT32 NumMeshParts, const UINT32** ppIndices, UINT32* pNumIndices) noexcept = 0;
	virtual UINT32 GetNumMeshPart() const noexcept = 0;
	virtual UINT32 GetNumIndex(INT32 MeshPartIndex) const noexcept = 0;

	virtual void SetMaterial(INT32 MeshPartIndex, ISurfaceMaterial* pMaterial) noexcept = 0;
	virtual void GetMaterial(INT32 MeshPartIndex, ISurfaceMaterial** ppMaterial) noexcept = 0;
	virtual ISurfaceMaterial* GetMaterial(INT32 MeshPartIndex) noexcept = 0;

};

/**
* @brief 기본 모델 인터페이스
*/
struct IModel : public IAsset
{
	virtual UINT32 GetNumMesh() const noexcept = 0;
	virtual INT32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, UINT32 NumVertex, UINT32 NumIndex) noexcept = 0;

	virtual IMesh* GetMesh(INT32 MeshIndex) noexcept = 0;
	virtual IMesh* GetMesh(NameHash Name) noexcept = 0;

};


#endif // __GEOMETRY_DEF_H__
