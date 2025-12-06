#include "pch.h"
#include "BinaryAsset.h"


IMPLEMENTATION_IUNKNOWN_INTERFACE(BinaryAsset);

/*
BinaryAsset::BinaryAsset()
	: m_pData(nullptr)
	, m_Size(0)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
	, m_HeapType(HEAP_TYPE::SYSCALL)
{
}
*/

BinaryAsset::BinaryAsset(HEAP_TYPE HeapType)
	: m_pData(nullptr)
	, m_Size(0)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
	, m_HeapType(HeapType)
{
}

BinaryAsset::~BinaryAsset() noexcept
{
	if (m_pData)
	{
		HEAP_FREE_AUTO(m_pData, m_HeapType);
		m_pData = nullptr;
	}
}

void BinaryAsset::OnDestroy()
{
	HEAP_FREE_AUTO(this, m_HeapType);
}

ASSET_TYPE BinaryAsset::GetAssetType() const noexcept
{
	return INL_GetAssetType();
}

LOAD_STAT BinaryAsset::GetLoadStat() const noexcept
{
	return INL_GetLoadStat();
}

const char* BinaryAsset::GetData() const noexcept
{
	return m_pData;
}

size_t BinaryAsset::GetSize() const noexcept
{
	return m_Size;
}
