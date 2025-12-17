#ifndef __TEXTURE1D_H__
#define __TEXTURE1D_H__

#include "ITexture1D.h"


class Texture1D : public ITexture1D
{
public:
	Texture1D(UINT32 ID);
	virtual ~Texture1D() noexcept;

	// Private inline methods
	__FORCEINLINE UINT32 INL_GetID() const noexcept
	{
		return m_ID;
	}

	__FORCEINLINE ASSET_TYPE INL_GetAssetType() const noexcept
	{
		return ASSET_TYPE::TEXT;
	}

	__FORCEINLINE LOAD_STAT INL_GetLoadStat() const noexcept
	{
		return m_LoadStat;
	}

};


#endif // __TEXTURE1D_H__