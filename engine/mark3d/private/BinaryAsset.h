#ifndef __BINARYASSET_H__
#define __BINARYASSET_H__

#include "IBinaryAsset.h"


class BinaryAsset : public IBinaryAsset
{
	DECLARATION_IUNKNOWN_INTERFACE(BinaryAsset);

public:
	BinaryAsset(UINT32 ID);

	// IAsset interface
	virtual UINT32 GetID() const noexcept override;
	virtual ASSET_TYPE GetAssetType() const noexcept override;
	virtual LOAD_STAT GetLoadStat() const noexcept override;

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
	char* m_pData;
	size_t m_Size;
	volatile LOAD_STAT m_LoadStat;
	uint32 m_CRC32Cache;
	uint64 m_CRC64Cache;
	uint32 m_ID;

};


#endif // __BINARYASSET_H__
