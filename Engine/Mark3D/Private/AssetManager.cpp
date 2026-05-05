#include "pch.h"
#include "AssetManager.h"


namespace mark
{
	AssetManager* AssetManager::s_pInstance = nullptr;

	AssetManager::AssetManager(IAssetProvider* pProvider)
		: m_pProvider(pProvider)
	{
		if (!s_pInstance)
			s_pInstance = this;
	}

	AssetManager::~AssetManager() noexcept
	{
		if (s_pInstance == this)
			s_pInstance = nullptr;

		CHECK_RELEASE(m_pProvider);
	}

	void AssetManager::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void AssetManager::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(AssetManager, this);
		}
	}

	IAsset* AssetManager::LoadAsset(ASSET_TYPE AssetType, const char* szAssetPath)
	{
		if (!m_pProvider) [[unlikely]]
		{
			SYS_LOG_ERR("Asset provider is not set. Cannot load asset.");
			return nullptr;
		}

		return m_pProvider->LoadAsset(AssetType, szAssetPath);
	}

}
