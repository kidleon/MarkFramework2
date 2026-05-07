#pragma once

namespace mark
{
	enum class ASSET_TYPE : uint32_t
	{
		TEXT,
		BINARY,
		MODEL,
		EMAX
	};

	struct IAsset : public Unknown
	{
		[[nodiscard]] virtual ASSET_TYPE GetType() const noexcept = 0;
	};

	static constexpr size_t MAX_TEXTURE_FILENAME = 128;
	static constexpr size_t MESH_NAME_SIZE = 64;

	struct MESH_DESC
	{
		char szName[MESH_NAME_SIZE] = { 0 };
		
		FLOAT3* pPosition = nullptr;
		FLOAT3* pNormal = nullptr;
		FLOAT3* pTangent = nullptr;
		FLOAT2* pTexCoord0 = nullptr;
		FLOAT4* pColor = nullptr;
		uint32_t NumVertex = 0;

		INDEX_FORMAT IndexFormat = INDEX_FORMAT::UINT16;
		void* pIndices = { nullptr };
		uint32_t NumIndex = 0;
		uint32_t NumSubset = 0;
		uint32_t SubsetIndexCount[8] = { 0 };
		uint32_t SubsetIndexStart[8] = { 0 };
		
	};

	struct IAssetBlob : public Unknown
	{
		[[nodiscard]] virtual size_t GetDataSize() const noexcept = 0;
		[[nodiscard]] virtual const void* GetData() const noexcept = 0;
	};

	struct IModelAsset : public IAsset
	{
		virtual uint32_t GetNumMeshes() const noexcept = 0;
		virtual void GetMeshDesc(uint32_t MeshIndex, MESH_DESC& MeshDesc) const = 0;
	};

	struct IAssetProvider : public Unknown
	{
		[[nodiscard]] virtual IAssetBlob* LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath) = 0;
		[[nodiscard]] virtual IAssetBlob* LoadAsset(HANDLE temppool_handle, ASSET_TYPE AssetType, const char* szAssetPath) = 0;
	};

	struct IAssetManager: public Unknown
	{
		[[nodiscard]] virtual IAsset* LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath) = 0;
	};
}
