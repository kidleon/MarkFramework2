#include "pch.h"
#include "BinaryAsset.h"
#include "crc32.h"
#include "crc64.h"
#include "idgen.h"
#include "Assets.h"


BinaryAsset::BinaryAsset(UINT32 ID)
	: m_pData(nullptr)
	, m_Size(0)
	, m_CRC64Cache(0)
	, m_CRC32Cache(0)
	, m_ID(ID)
	, m_LoadStat(LOAD_STAT::NOT_LOADED)
{
}

BinaryAsset::~BinaryAsset() noexcept
{
	idgen_release(Assets::ID_GEN_HANDLE, m_ID);
	m_ID = 0;

	if (m_pData)
	{
		MARK_SYS_FREE(m_pData);
		m_pData = nullptr;
	}
}

long BinaryAsset::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long BinaryAsset::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		MARK_DELETE(this, BinaryAsset);
	}
	return NewRefCnt;
}

long BinaryAsset::RefCnt()
{
	return m_RefCnt;
}

UINT32 BinaryAsset::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE BinaryAsset::GetAssetType() const noexcept
{
	return ASSET_TYPE::BINARY;
}

LOAD_STAT BinaryAsset::GetLoadStat() const noexcept
{
	return m_LoadStat;
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