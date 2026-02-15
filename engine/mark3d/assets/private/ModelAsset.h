#ifndef __MODEL_ASSET_H__
#define __MODEL_ASSET_H__

#include "TArray.h"
using namespace mark;
#include "AssetDef.h"


class ModelAsset final : public IModelAsset
{
public:
	struct Mesh
	{
		char szName[64]; // Mesh 이름 (최대 63자 + null terminator)

		FLOAT3* pPositions;
		FLOAT3* pNormals;
		FLOAT2* pTexCoords;
		FLOAT4* pColors;
		FLOAT3* pTangents;
		FLOAT3* pBinormals;
		
		size_t NumPositions;
		size_t NumNormals;
		size_t NumTexCoords;
		size_t NumColors;
		size_t NumTangents;
		size_t NumBinormals;

		// SubMesh별 인덱스 데이터
		struct SubMesh
		{
			uint32* pIndices; // 인덱스 데이터
			size_t NumIndices; // 인덱스 개수
		};
		SubMesh* pSubMeshes; // SubMesh 배열
		size_t NumSubMesh;

	};

public:
	ModelAsset(UINT64 ID);
	
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IModelAsset interface
	virtual size_t GetNumMesh() const noexcept final;
	virtual size_t GetNumSubMesh(int32 MeshIndex) noexcept final;

	virtual FLOAT3* GetPositions(int32 MeshIndex) noexcept final;
	virtual size_t GetNumPositions(int32 MeshIndex) const noexcept final;

	virtual FLOAT3* GetNormals(int32 MeshIndex) noexcept final;
	virtual size_t GetNumNormals(int32 MeshIndex) const noexcept final;

	virtual FLOAT2* GetTexCoords(int32 MeshIndex) noexcept final;
	virtual size_t GetNumTexCoords(int32 MeshIndex) const noexcept final;

	virtual FLOAT4* GetColor(int32 MeshIndex) noexcept final;
	virtual size_t GetNumColor(int32 MeshIndex) const noexcept final;

	virtual FLOAT3* GetTangent(int32 MeshIndex) noexcept final;
	virtual size_t GetNumTangent(int32 MeshIndex) const noexcept final;

	virtual FLOAT3* GetBinormal(int32 MeshIndex) noexcept final;
	virtual size_t GetNumBinormal(int32 MeshIndex) const noexcept final;
	/*
	virtual FLOAT4* GetBlendWeight(int32 MeshIndex) noexcept final;
	virtual size_t GetNumBlendWeight(int32 MeshIndex) const noexcept final;

	virtual UINT4* GetBlendIndices(int32 MeshIndex) noexcept final;
	virtual size_t GetNumBlendIndices(int32 MeshIndex) const noexcept final;
	*/

	virtual uint32* GetIndices(int32 MeshIndex, int32 SubMeshIndex) noexcept final;
	virtual size_t GetNumIndices(int32 MeshIndex, int32 SubMeshIndex) const noexcept final;

	void AddMesh(const char* szName, uint32 VertexFormat, size_t NumVertices, size_t NumSubMeshes) noexcept;

	__FORCEINLINE void INL_SetLoadStat(LOAD_STAT LoadStat) noexcept { m_LoadStat = LoadStat; }

private:
	ModelAsset() = delete;
	virtual ~ModelAsset() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)
	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

	TArray<Mesh*, TA_SYSCALL> m_Meshes;

};

#endif // !__MODEL_ASSET_H__