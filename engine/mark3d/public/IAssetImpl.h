#ifndef __I_ASSET_IMPL_H__
#define __I_ASSET_IMPL_H__

#include "IAsset.h"
#include "AssetDef.h"
#include "idgen.h"


template<ASSET_TYPE AssetType>
struct IASSET_IMPL : public IAsset
{
public:
	IASSET_IMPL() = default;
	virtual long AddRef() final
	{
		interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
		return m_RefCnt;
	}

	virtual long Release() final
	{
		long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
		if (NewRefCnt == 0)
		{
			OnDestroy();
		}
		return NewRefCnt;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

	virtual uint32 GetID() const noexcept final
	{
		return m_ID;
	}

	virtual ASSET_TYPE GetAssetType() const noexcept final
	{
		return AssetType;
	}

	virtual LOAD_STAT GetLoadStat() const noexcept final
	{
		return m_LoadStat;
	}

protected:
	virtual ~IASSET_IMPL() noexcept
	{
		idgen_release(GLOBAL_VARS::ID_GEN_HANDLE, m_ID);
	}

	IASSET_IMPL(const IASSET_IMPL&) = delete;
	IASSET_IMPL(IASSET_IMPL&&) = delete;
	IASSET_IMPL& operator=(const IASSET_IMPL&) = delete;
	IASSET_IMPL& operator=(IASSET_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

	__INLINE void INL_SetID(uint32 ID) noexcept
	{
		m_ID = ID;
	}

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

protected:
	uint32 m_ID = 0;
	//ASSET_TYPE AssetType = ASSET_TYPE::UNKNOWN;
	volatile LOAD_STAT m_LoadStat = LOAD_STAT::NOT_LOADED;

};


#endif // __I_ASSET_IMPL_H__
