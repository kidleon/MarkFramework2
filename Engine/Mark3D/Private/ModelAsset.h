#pragma once


namespace mark
{
	class ModelAsset final : public IModelAsset
	{
	public:
		struct Material
		{
			int32_t MatID;
			char szDiffuse[MAX_TEXTURE_FILENAME];
			char szNormal[MAX_TEXTURE_FILENAME];
			char szSpecular[MAX_TEXTURE_FILENAME];
			char szEmissive[MAX_TEXTURE_FILENAME];
		};

		struct SubMesh
		{
			int32_t MatID;
			uint32_t StartOffset;
			uint32_t IndexCount;
		};

		struct Mesh
		{
			char szName[MESH_NAME_SIZE];
			uint32_t StartVertex;
			uint32_t StartIndex;
			uint32_t NumVertex;
			uint32_t NumIndex;
			int32_t MatID;
			uint32_t NumSubMesh;
			SubMesh SubMesh[MAX_SUBMESH_COUNT];
		};

		struct MeshData
		{
			uint32_t VertexFormats;
			uint32_t TotalVertexCount;
			uint32_t TotalIndexCount;
			INDEX_FORMAT IndexFormat;
			FLOAT3* pPosition = nullptr;
			FLOAT3* pNormal = nullptr;
			FLOAT2* pTexCoord0 = nullptr;
			FLOAT4* pColor = nullptr;
			FLOAT4* pTangent = nullptr;
			void* pIndices = nullptr;

			void clear()
			{
				CORE_SYS_FREE(pPosition);
				CORE_SYS_FREE(pNormal);
				CORE_SYS_FREE(pTexCoord0);
				CORE_SYS_FREE(pColor);
				CORE_SYS_FREE(pTangent);
				CORE_SYS_FREE(pIndices);
			}
		};

		ModelAsset(const char* szAssetPath);

		virtual void AddRef();
		virtual void Release();

		virtual ASSET_TYPE GetType() const noexcept { return ASSET_TYPE::MODEL; }
		virtual const char* GetAssetPath() const noexcept { return m_AssetPath.c_str(); }
		virtual bool IsLoaded() const noexcept { return m_IsLoaded.load(std::memory_order_acquire); }

		virtual uint32_t GetNumMeshes() const noexcept;
		virtual void GetMeshDesc(uint32_t MeshIndex, MESH_DESC& MeshDesc) const;

		void ComputeTangent();

		[[nodiscard]] inline const char* INL_GetAssetPath() const noexcept { return m_AssetPath.c_str(); }
		inline void INL_SetAssetPath(const char* szAssetPath) { m_AssetPath = szAssetPath; }

		[[nodiscard]] inline ASSET_TYPE INL_GetType() const noexcept { return ASSET_TYPE::MODEL; }
		[[nodiscard]] inline uint32_t INL_GetNumMeshes() const noexcept { return GetNumMeshes(); }
		[[nodiscard]] inline const Mesh* INL_GetMesh(uint32_t MeshIndex) const noexcept { return &m_lstMesh[MeshIndex]; }
		[[nodiscard]] inline const MeshData& INL_GetMeshData() const noexcept { return m_MeshData; }

		[[nodiscard]] inline uint32_t INL_GetTotalVertexCount() const noexcept { return m_MeshData.TotalVertexCount; }
		[[nodiscard]] inline uint32_t INL_GetTotalIndexCount() const noexcept { return m_MeshData.TotalIndexCount; }
		[[nodiscard]] inline uint32_t INL_GetVertexFormats() const noexcept { return m_MeshData.VertexFormats; }
		[[nodiscard]] inline BOOL INL_HasComputedTangent() const noexcept { return m_ComputedTangent; }

		[[nodiscard]] inline const FLOAT3* INL_GetPosition(int32_t MeshIndex) const noexcept { return m_MeshData.pPosition + m_lstMesh[MeshIndex].StartVertex; }
		[[nodiscard]] inline const FLOAT3* INL_GetNormal(int32_t MeshIndex) const noexcept { return m_MeshData.pNormal + m_lstMesh[MeshIndex].StartVertex; }
		[[nodiscard]] inline const FLOAT2* INL_GetTexCoord0(int32_t MeshIndex) const noexcept { return m_MeshData.pTexCoord0 + m_lstMesh[MeshIndex].StartVertex; }
		[[nodiscard]] inline const FLOAT4* INL_GetColor(int32_t MeshIndex) const noexcept { return m_MeshData.pColor + m_lstMesh[MeshIndex].StartVertex; }
		[[nodiscard]] inline const FLOAT4* INL_GetTangent(int32_t MeshIndex) const noexcept { return m_MeshData.pTangent + m_lstMesh[MeshIndex].StartVertex; }
		[[nodiscard]] inline const void* INL_GetIndices(int32_t MeshIndex) const noexcept
		{
			const size_t IndexSize = (m_MeshData.IndexFormat == INDEX_FORMAT::UINT16) ? sizeof(uint16_t) : sizeof(uint32_t);
			return static_cast<const uint8_t*>(m_MeshData.pIndices) + m_lstMesh[MeshIndex].StartIndex * IndexSize;
		}

		inline void INL_SetLoaded(BOOL isLoaded) noexcept { m_IsLoaded.store(isLoaded, std::memory_order_release); }

	private:
		ModelAsset() = delete;
		virtual ~ModelAsset() noexcept;
		void Reset();

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		std::atomic<BOOL> m_IsLoaded{ FALSE };
		BOOL m_ComputedTangent = FALSE;
		uint32_t PADDING = 0;

		sys_string m_AssetPath;

	public:
		sys_vector<Mesh> m_lstMesh;
		MeshData m_MeshData;
		sys_vector<Material*> m_lstMaterial;

	};

	bool LoadModelFromFBX(IAssetBlob* pBlob, ModelAsset* pModelAsset);
}
