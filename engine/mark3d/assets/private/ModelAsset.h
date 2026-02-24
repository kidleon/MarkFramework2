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
		
		size_t NumVertices;

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

	virtual size_t GetNumVertices(int32 MeshIndex) const noexcept final;

	virtual FLOAT3* GetPositions(int32 MeshIndex) noexcept final;
	virtual FLOAT3* GetNormals(int32 MeshIndex) noexcept final;
	virtual FLOAT2* GetTexCoords(int32 MeshIndex) noexcept final;
	virtual FLOAT4* GetColor(int32 MeshIndex) noexcept final;
	virtual FLOAT3* GetTangent(int32 MeshIndex) noexcept final;
	virtual FLOAT3* GetBinormal(int32 MeshIndex) noexcept final;

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
	size_t m_NumMeshes = 0;
	size_t m_NumMaterials = 0;
	Mesh* m_pMeshes = nullptr;
	Material* m_pMaterials = nullptr;

};

#endif // !__MODEL_ASSET_H__