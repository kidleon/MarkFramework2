#include "pch.h"
#include "BinaryAsset.h"
#include "crc32.h"
#include "crc64.h"
#include "idgen.h"


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

BinaryAsset::BinaryAsset(UINT32 ID)
	: m_pData(nullptr)
	, m_Size(0)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
	, m_CRC64Cache(0)
	, m_CRC32Cache(0)
	, m_ID(ID)
{
}

BinaryAsset::~BinaryAsset() noexcept
{
	idgen_release(GLOBAL_VARS::ID_GEN_HANDLE, m_ID);
	m_ID = 0;

	if (m_pData)
	{
		MARK_SYS_FREE(m_pData);
		m_pData = nullptr;
	}
}

void BinaryAsset::OnDestroy()
{
	MARK_DELETE(this, BinaryAsset);
}

UINT32 BinaryAsset::GetID() const noexcept
{
	return INL_GetID();
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

uint32 BinaryAsset::ComputeCRC32() noexcept
{
	if (!m_pData || !m_Size)
		return 0;

	if (!m_CRC32Cache)
	{
		m_CRC32Cache = crc32(m_pData, m_Size, 0xFFFFFFFFu);
	}

	return m_CRC32Cache;
}

uint64 BinaryAsset::ComputeCRC64() noexcept
{
	if (!m_pData || !m_Size)
		return 0;

	if (!m_CRC64Cache)
	{
		m_CRC64Cache = crc64(m_pData, m_Size, 0xFFFFFFFFFFFFFFFFull);
	}

	return m_CRC64Cache;
}