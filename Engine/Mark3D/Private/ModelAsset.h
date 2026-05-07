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
			uint32_t StartIndex;
			uint32_t IndexCount;
		};

		struct Mesh
		{
			char szName[MESH_NAME_SIZE];
			uint32_t NumVertex;
			uint32_t NumIndex;
			FLOAT3* pPosition;
			FLOAT3* pNormal;
			FLOAT2* pTexCoord0;
			FLOAT2* pTexCoord1;
			FLOAT4* pColor;
			FLOAT3* pTangent; // xyz : tangent
			UINT4* pBoneIndices;
			FLOAT4* pBoneWeight;
			void* pIndices;
			uint32_t IndexSize;
			uint32_t NumSubMesh;
			SubMesh* pSubMesh;

			void Cleanup() noexcept
			{
				CORE_SYS_FREE(pPosition);
				CORE_SYS_FREE(pNormal);
				CORE_SYS_FREE(pTexCoord0);
				CORE_SYS_FREE(pTexCoord1);
				CORE_SYS_FREE(pColor);
				CORE_SYS_FREE(pTangent);
				CORE_SYS_FREE(pBoneIndices);
				CORE_SYS_FREE(pBoneWeight);
				CORE_SYS_FREE(pIndices);
				CORE_SYS_FREE(pSubMesh);
			}
		};

		virtual void AddRef();
		virtual void Release();

		virtual ASSET_TYPE GetType() const noexcept { return ASSET_TYPE::MODEL; }
		virtual uint32_t GetNumMeshes() const noexcept;
		virtual void GetMeshDesc(uint32_t MeshIndex, MESH_DESC& MeshDesc) const;

		void ComputeTangent();

		[[nodiscard]] inline ASSET_TYPE INL_GetType() const noexcept { return ASSET_TYPE::MODEL; }
		[[nodiscard]] inline uint32_t INL_GetNumMeshes() const noexcept { return GetNumMeshes(); }
		[[nodiscard]] inline Mesh* INL_GetMesh(uint32_t MeshIndex) const noexcept { return m_lstMesh[MeshIndex]; }

		[[nodiscard]] inline uint32_t INL_GetTotalVertexCount() const noexcept { return m_TotalVertexCount; }
		[[nodiscard]] inline uint32_t INL_GetTotalIndexCount() const noexcept { return m_TotalIndexCount; }


	private:
		virtual ~ModelAsset() noexcept;
		void Reset();

	private:
		std::atomic<int32_t> m_RefCount{ 1 };

	public:
		uint32_t m_VertexFormats = 0;
		uint32_t m_TotalVertexCount = 0;
		uint32_t m_TotalIndexCount = 0;
		sys_vector<Mesh*> m_lstMesh;
		sys_vector<Material*> m_lstMaterial;

	};

	bool LoadModelFromFBX(IAssetBlob* pBlob, ModelAsset* pModelAsset);
}
