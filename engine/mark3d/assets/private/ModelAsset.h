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
			INT32 MaterialIndex; // Material배열 인덱스
			UINT32 PADDING_OR_RESERVED = 0; // 패딩 또는 예약 필드
			UINT32* pIndices; // 인덱스 데이터
			size_t NumIndices; // 인덱스 개수
		};

		SubMesh* pSubMeshes; // SubMesh 배열
		size_t NumSubMesh;
	};

	struct Material
	{
		INT32 ID;
		char Diffuse[MAX_FILE_LENGTH];
		char Normal[MAX_FILE_LENGTH];
		char Specular[MAX_FILE_LENGTH];
		char Emissive[MAX_FILE_LENGTH];
		FLOAT4 Color;
	};

public:
	ModelAsset(UINT64 ID, const char* szRelativePath);
	
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;
	virtual BOOL GetRelativePath(
		char* szBuffer,
		size_t BufferLen,
		BOOL IgnoreFileName
	) const noexcept final;

	// IModelAsset interface
	virtual UINT32 GetModelAttrib() const noexcept final;

	virtual size_t GetNumMesh() const noexcept final;

	virtual UINT32 GetVertexFormat(INT32 MeshIndex) const noexcept final;
	virtual UINT32 GetVertexFormat() const noexcept final;
	virtual UINT32 GetTotalVertexCount() const noexcept final;
	virtual UINT32 GetTotalIndexCount() const noexcept final;

	virtual const char* GetMeshName(INT32 MeshIndex) const noexcept final;

	virtual size_t GetNumSubMesh(INT32 MeshIndex) noexcept final;

	virtual size_t GetNumVertices(INT32 MeshIndex) const noexcept final;

	virtual FLOAT3* GetPositions(INT32 MeshIndex) noexcept final;
	virtual FLOAT3* GetNormals(INT32 MeshIndex) noexcept final;
	virtual FLOAT2* GetTexCoords(INT32 MeshIndex) noexcept final;
	virtual FLOAT4* GetColor(INT32 MeshIndex) noexcept final;
	virtual FLOAT3* GetTangent(INT32 MeshIndex) noexcept final;
	virtual FLOAT3* GetBinormal(INT32 MeshIndex) noexcept final;

	virtual UINT32* GetIndices(INT32 MeshIndex, INT32 SubMeshIndex) noexcept final;
	virtual size_t GetNumIndices(INT32 MeshIndex) const noexcept final;
	virtual size_t GetNumIndices(INT32 MeshIndex, INT32 SubMeshIndex) const noexcept final;

	virtual size_t GetNumMaterials() const noexcept final;
	
	virtual BOOL HasDiffuseTexture(INT32 MaterialID) const noexcept final;
	virtual BOOL HasNormalTexture(INT32 MaterialID) const noexcept final;
	virtual BOOL HasSpecularTexture(INT32 MaterialID) const noexcept final;
	virtual BOOL HasEmissiveTexture(INT32 MaterialID) const noexcept final;
	virtual BOOL HasColor(INT32 MaterialID) const noexcept final;

	virtual INT32 GetMaterialIndex(INT32 MeshIndex, INT32 SubMeshIndex) const noexcept final;
	virtual const char* GetMaterialDiffuse(INT32 MaterialID) const noexcept final;
	virtual const char* GetMaterialNormal(INT32 MaterialID) const noexcept final;
	virtual const char* GetMaterialSpecular(INT32 MaterialID) const noexcept final;
	virtual const char* GetMaterialEmissive(INT32 MaterialID) const noexcept final;
	virtual FLOAT4 GetMaterialColor(INT32 MaterialID) const noexcept final;

	__FORCEINLINE void INL_SetLoadStat(LOAD_STAT LoadStat) noexcept
	{
		interlock_store_l((long*)&m_LoadStat, (long)LoadStat, MEMORY_ORDER_RELAXED);
	}

	__FORCEINLINE UINT32 INL_GetModelAttrib() const noexcept { return m_ModelAttrib; }

	BOOL LoadFromFBX(const FBX_SCENE* fbx_scene) noexcept;

private:
	ModelAsset() = delete;
	virtual ~ModelAsset() noexcept;

	INT32 FindMaterialIndex(INT32 MatID) const noexcept;

private:
	volatile long m_RefCnt = 1;
	LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;
	UINT64 m_ID = 0;
	
	char m_szRelativePath[MAX_FILE_LENGTH] = { 0 };

	UINT32 m_ModelAttrib = 0;
	size_t m_NumMeshes = 0;
	size_t m_NumMaterials = 0;
	Mesh* m_pMeshes = nullptr;
	Material* m_pMaterials = nullptr;

};

#endif // !__MODEL_ASSET_H__