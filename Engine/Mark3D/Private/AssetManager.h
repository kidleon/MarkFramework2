#pragma once


namespace mark
{
	class ModelAsset;

	class AssetManager final : public IAssetManager
	{
	public:
		AssetManager(IAssetProvider* pProvider);

		virtual void AddRef();
		virtual void Release();

		virtual IAsset* LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath);

		ModelAsset* LoadModelAsset(const char* szAssetPath);

	private:
		virtual ~AssetManager() noexcept;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		IAssetProvider* m_pProvider = nullptr;

	};
}
