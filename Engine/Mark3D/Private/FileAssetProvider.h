#pragma once


namespace mark
{
	class FileAssetProvider final : public IAssetProvider
	{
	public:
		FileAssetProvider(const char* szRootPath);

		virtual void AddRef();
		virtual void Release();

		virtual IAsset* LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath);

	private:
		virtual ~FileAssetProvider() noexcept;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		char m_szRootPath[_MAX_PATH] = { 0 };
		HANDLE m_hTempPool = nullptr; // 임시 버퍼 풀 핸들 (파일 읽기 등에서 사용)

	};
}
