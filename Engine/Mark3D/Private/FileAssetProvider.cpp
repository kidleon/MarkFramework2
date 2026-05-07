#include "pch.h"
#include "FileAssetProvider.h"
#include "file_system.h"
#include "AssetBlob.h"


namespace mark
{
	FileAssetProvider::FileAssetProvider(const char* szRootPath)
	{
		safe_strcpy(m_szRootPath, _MAX_PATH, szRootPath);
		file_system::initialize(m_szRootPath);
		m_hTempPool = temppool_create(1024 * 1024 * 18);
	}

	FileAssetProvider::~FileAssetProvider() noexcept
	{
		if (m_hTempPool)
		{
			temppool_destroy(m_hTempPool);
			m_hTempPool = nullptr;
		}

		file_system::destroy();
	}

	void FileAssetProvider::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void FileAssetProvider::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(FileAssetProvider, this);
		}
	}

	IAssetBlob* FileAssetProvider::LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath)
	{
		char szFullPath[_MAX_PATH];
		file_system::combine_path(m_szRootPath, szAssetPath, szFullPath, _MAX_PATH);

		HANDLE file_handle = file_system::open_file(szFullPath, file_system::ACESS_MODE::READ);
		if (!file_handle)
		{
			SYS_LOG_ERR_F("Failed to open asset file: {}", szFullPath);
			return nullptr;
		}

		size_t ReadBytes = 0;
		void* pBuffer = file_system::read_all(m_hTempPool, file_handle, ReadBytes);
		if (!pBuffer)
		{
			file_system::close_file(file_handle);
			SYS_LOG_ERR_F("Failed to read asset file: {}", szFullPath);
			return nullptr;
		}

		file_system::close_file(file_handle);

		return CORE_NEW(AssetBlob)(ReadBytes, pBuffer);
	}

	IAssetBlob* FileAssetProvider::LoadAsset(HANDLE temppool_handle, ASSET_TYPE AssetType, const char* szAssetPath)
	{
		char szFullPath[_MAX_PATH];
		file_system::combine_path(m_szRootPath, szAssetPath, szFullPath, _MAX_PATH);

		HANDLE file_handle = file_system::open_file(szFullPath, file_system::ACESS_MODE::READ);
		if (!file_handle)
		{
			SYS_LOG_ERR_F("Failed to open asset file: {}", szFullPath);
			return nullptr;
		}

		temppool_reset(temppool_handle);

		size_t ReadBytes = 0;
		void* pBuffer = file_system::read_all(temppool_handle, file_handle, ReadBytes);
		if (!pBuffer)
		{
			file_system::close_file(file_handle);
			SYS_LOG_ERR_F("Failed to read asset file: {}", szFullPath);
			return nullptr;
		}

		file_system::close_file(file_handle);

		return CORE_NEW(AssetBlob)(ReadBytes, pBuffer, FALSE);
	}
}
