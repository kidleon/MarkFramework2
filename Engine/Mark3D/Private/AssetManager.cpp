#include "pch.h"
#include "AssetManager.h"
#include "AssetBlob.h"
#include "ModelAsset.h"


namespace mark
{
	AssetManager::AssetManager(IAssetProvider* pProvider)
		: m_pProvider(pProvider)
	{
	}

	AssetManager::~AssetManager() noexcept
	{
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

		AssetBlob* pBlob = static_cast<AssetBlob*>(m_pProvider->LoadAsset(AssetType, szAssetPath));
		if (!pBlob)
			return nullptr;

		ModelAsset* pModelAsset = CORE_NEW(ModelAsset)(szAssetPath);

		if (!LoadModelFromFBX(pBlob, pModelAsset))
		{
			SYS_LOG_ERR_F("Failed to load model asset: {}", szAssetPath);
			pModelAsset->Release();
			pBlob->Release();
			return nullptr;
		}

		pBlob->Release();

		return pModelAsset;
	}

}
