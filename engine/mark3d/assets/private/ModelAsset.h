#ifndef __MODEL_ASSET_H__
#define __MODEL_ASSET_H__


struct FBX_SCENE;

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
			int32 MaterialID; // SubMesh에 적용된 Material ID
			uint32 PADDING_OR_RESERVED = 0; // 패딩 또는 예약 필드
			uint32* pIndices; // 인덱스 데이터
			size_t NumIndices; // 인덱스 개수
		};

		SubMesh* pSubMeshes; // SubMesh 배열
		size_t NumSubMesh;

		~Mesh() noexcept
		{
			if (pPositions)
				CORE_SYS_FREE(pPositions);
			if (pNormals)
				CORE_SYS_FREE(pNormals);
			if (pTexCoords)
				CORE_SYS_FREE(pTexCoords);
			if (pColors)
				CORE_SYS_FREE(pColors);
			if (pTangents)
				CORE_SYS_FREE(pTangents);
			if (pBinormals)
				CORE_SYS_FREE(pBinormals);
			if (pSubMeshes)
			{
				for (size_t i = 0; i < NumSubMesh; i++)
				{
					if (pSubMeshes[i].pIndices)
						CORE_SYS_FREE(pSubMeshes[i].pIndices);
				}
				CORE_SYS_FREE(pSubMeshes);
			}
		}

	};

	struct Material
	{
		int32 ID;
		char Diffuse[MAX_FILE_LENGTH];
		char Normal[MAX_FILE_LENGTH];
		char Specular[MAX_FILE_LENGTH];
		char Emissive[MAX_FILE_LENGTH];
		FLOAT4 Color;
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
	virtual UINT32 GetModelAttrib() const noexcept final;

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

	virtual uint32* GetIndices(int32 MeshIndex, int32 SubMeshIndex) noexcept final;
	virtual size_t GetNumIndices(int32 MeshIndex) const noexcept final;
	virtual size_t GetNumIndices(int32 MeshIndex, int32 SubMeshIndex) const noexcept final;

	__FORCEINLINE void INL_SetLoadStat(LOAD_STAT LoadStat) noexcept { m_LoadStat = LoadStat; }

	BOOL LoadFromFBX(const FBX_SCENE* fbx_scene) noexcept;

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

	UINT32 m_ModelAttrib = 0;
	Mesh* m_pMeshes;
	Material* m_pMaterials;

};

#endif // !__MODEL_ASSET_H__