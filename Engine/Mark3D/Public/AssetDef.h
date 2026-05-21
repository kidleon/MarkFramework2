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
		[[nodiscard]] virtual const char* GetAssetPath() const noexcept = 0;
		[[nodiscard]] virtual bool IsLoaded() const noexcept = 0;
	};

	static constexpr size_t MAX_TEXTURE_FILENAME = 128;
	static constexpr size_t MESH_NAME_SIZE = 64;
	constexpr static size_t MAX_SUBMESH_COUNT = 8;

	struct MESH_DESC
	{
		char szName[MESH_NAME_SIZE] = { 0 };
		
		FLOAT3* pPosition = nullptr;
		FLOAT3* pNormal = nullptr;
		FLOAT4* pTangent = nullptr;
		FLOAT2* pTexCoord0 = nullptr;
		FLOAT4* pColor = nullptr;
		void* pIndices = nullptr;
		INDEX_FORMAT IndexFormat = INDEX_FORMAT::UINT16;
		uint32_t NumVertex = 0;
		uint32_t NumIndex = 0;
		uint32_t NumSubset = 0;
		uint32_t SubsetIndexCount[MAX_SUBMESH_COUNT] = { 0 };
		uint32_t SubsetIndexStart[MAX_SUBMESH_COUNT] = { 0 };
		
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
		/**
		 * @brief 에셋 데이터를 로드합니다.
		 * @return [Owning] 반환된 포인터는 AddRef된 상태입니다. 호출자가 Release()를 책임지거나
		 *         unknown_ptr<IAssetBlob>::attach()로 takeover 하세요. 실패 시 nullptr.
		 */
		[[nodiscard]] virtual IAssetBlob* LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath) = 0;

		/**
		 * @brief 호출자 제공 temppool 위에 에셋 데이터를 로드합니다 (zero-copy 경로).
		 * @return [Owning] 위와 동일. 데이터 버퍼 자체의 소유권은 temppool에 있으므로
		 *         AssetBlob 해제 시에도 buffer는 풀에 머무릅니다.
		 */
		[[nodiscard]] virtual IAssetBlob* LoadAsset(HANDLE temppool_handle, ASSET_TYPE AssetType, const char* szAssetPath) = 0;
	};

	struct IAssetManager: public Unknown
	{
		/**
		 * @brief 지정 타입의 에셋을 로드합니다.
		 * @return [Owning] 반환된 포인터는 AddRef된 상태입니다. 호출자가 Release()를 책임지거나
		 *         unknown_ptr<IAsset>::attach()로 takeover 하세요. 실패 시 nullptr.
		 */
		[[nodiscard]] virtual IAsset* LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath) = 0;
	};
}
