#ifndef __BINARYASSET_H__
#define __BINARYASSET_H__

#include "IBinaryAsset.h"


class BinaryAsset : public IBinaryAsset
{
public:
	BinaryAsset(UINT32 ID);
	virtual ~BinaryAsset() noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT32 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IBinaryAsset interface
	virtual const char* GetData() const noexcept override;
	virtual size_t GetSize() const noexcept override;
	virtual uint32 ComputeCRC32() noexcept override;
	virtual uint64 ComputeCRC64() noexcept override;

	// Private inline methods
	__FORCEINLINE UINT32 INL_GetID() const noexcept
	{
		return m_ID;
	}

	__FORCEINLINE ASSET_TYPE INL_GetAssetType() const noexcept
	{
		return ASSET_TYPE::BINARY;
	}

	__FORCEINLINE LOAD_STAT INL_GetLoadStat() const noexcept
	{
		return m_LoadStat;
	}

	__FORCEINLINE const char* INL_GetData() const noexcept
	{
		return m_pData;
	}

	__FORCEINLINE size_t INL_GetSize() const noexcept
	{
		return m_Size;
	}

	__FORCEINLINE void INL_SetData(char* pData, size_t Size) noexcept
	{
		m_pData = pData;
		m_Size = Size;

		LOAD_STAT loadStat = (nullptr != pData) ? LOAD_STAT::LOADED : LOAD_STAT::FAILED;

		interlock_store_l((long*)&m_LoadStat, (long)loadStat, MEMORY_ORDER_RELAXED);
	}

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT32 m_ID;
	LOAD_STAT m_LoadStat;

	char* m_pData;
	size_t m_Size;
	uint64 m_CRC64Cache;
	uint32 m_CRC32Cache;

};


#endif // __BINARYASSET_H__
