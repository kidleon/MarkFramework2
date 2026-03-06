#ifndef __MODEL_H__
#define __MODEL_H__

#include "GeomDef.h"
#include "TArray.h"
using namespace mark;


class Model : public IModel
{
	static constexpr size_t MAX_SUBMESH_PER_MESH = 8;

public:
	Model(UINT64 ID, uint32 VertexFormat, IPrimitiveBuffer* pPrimitiveBuffer);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IModel interface
	virtual size_t GetNumMesh() const noexcept final;
	virtual size_t GetNumSubMesh(int32 MeshIndex) const noexcept final;
	virtual size_t GetNumSubMesh(NameHash Name) const noexcept final;

	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 NumVertex, uint32 NumIndex) noexcept final;
	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 NumVertex, uint32 NumIndices, uint32* pNumIndices) noexcept final;

	virtual void SetMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept final;
	virtual void SetMaterial(NameHash Name, ISurfaceMaterial* pSurfaceMaterial) noexcept final;
	virtual void SetMaterial(int32 MeshIndex, int32 SubMeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept final;
	virtual void SetMaterial(NameHash Name, int32 SubMeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept final;

	virtual void SetPosition(int32 MeshIndex, FLOAT3* pPositions, UINT32 NumPosition) noexcept final;
	virtual void SetNormal(int32 MeshIndex, FLOAT3* pNormals, UINT32 NumNormal) noexcept final;
	virtual void SetTexCoord(int32 MeshIndex, FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept final;
	virtual void SetColor(int32 MeshIndex, FLOAT4* pColors, UINT32 NumColor) noexcept final;
	virtual void SetTangent(int32 MeshIndex, FLOAT3* pTangents, UINT32 NumTangent) noexcept final;
	virtual void SetBinormal(int32 MeshIndex, FLOAT3* pBinormals, UINT32 NumBinormal) noexcept final;

	virtual void SetPosition(NameHash Name, FLOAT3* pPositions, UINT32 NumPosition) noexcept final;
	virtual void SetNormal(NameHash Name, FLOAT3* pNormals, UINT32 NumNormal) noexcept final;
	virtual void SetTexCoord(NameHash Name, FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept final;
	virtual void SetColor(NameHash Name, FLOAT4* pColors, UINT32 NumColor) noexcept final;
	virtual void SetTangent(NameHash Name, FLOAT3* pTangents, UINT32 NumTangent) noexcept final;
	virtual void SetBinormal(NameHash Name, FLOAT3* pBinormals, UINT32 NumBinormal) noexcept final;

	virtual void SetIndex(int32 MeshIndex, const uint32* pIndices, UINT32 NumIndex) noexcept final;
	virtual void SetIndex(NameHash Name, const uint32* pIndices, UINT32 NumIndex) noexcept final;

	virtual void SetIndex(int32 MeshIndex, uint32 NumIndex, const uint32** ppIndices, uint32* pNumIndices) noexcept final;
	virtual void SetIndex(NameHash Name, uint32 NumIndex, const uint32** ppIndices, uint32* pNumIndices) noexcept final;

	BOOL LoadMesh(IModelAsset* pModelAsset) noexcept;
	BOOL LoadMaterial(IModelAsset* pModelAsset) noexcept;

private:
	Model() = delete;
	virtual ~Model() noexcept;
	int32 FindMeshIndex(NameHash Name) const noexcept;
	void OnApplyMaterials() noexcept;

private:
	long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::LOADED;

	struct MESH_DATA
	{
		NameHash Name;
		int PrimitiveIndex;
		uint32 MaxVertex;
		uint32 MaxIndex;
		uint32 NumVertex;

		struct SUB_MESH
		{
			uint32 NumIndex;
			int32 MaterialIndex;
		};

		uint32 NumSubMesh;
		SUB_MESH SubMeshes[MAX_SUBMESH_PER_MESH];
	};

	TArray<MESH_DATA, TA_POOL> m_lstMeshData;
	TArray<ISurfaceMaterial*, TA_POOL> m_lstMaterials;

	IPrimitiveBuffer* m_pPrimitiveBuffer = nullptr;
	uint32 m_VertexFormat = 0;

};


#endif // __MODEL_H__