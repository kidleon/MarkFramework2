#pragma once


namespace mark
{
	class AssetBlob final : public IAssetBlob
	{
	public:
		AssetBlob(size_t DataSize, void* pData);
		AssetBlob(size_t DataSize, void* pData, BOOL OwnsData);
		AssetBlob(AssetBlob&& other) noexcept;
		AssetBlob(const AssetBlob&) = delete;

		AssetBlob& operator=(AssetBlob&& other) noexcept;
		AssetBlob& operator=(const AssetBlob&) = delete;

		virtual void AddRef();
		virtual void Release();

		virtual size_t GetDataSize() const noexcept;
		virtual const void* GetData() const noexcept;

		inline const void* INL_GetData() const noexcept { return m_pData; }
		inline size_t INL_GetDataSize() const noexcept { return m_DataSize; }

	private:
		virtual ~AssetBlob() noexcept;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		BOOL m_OwnsData = TRUE;
		size_t m_DataSize = 0;
		void* m_pData = nullptr;

	};
}
