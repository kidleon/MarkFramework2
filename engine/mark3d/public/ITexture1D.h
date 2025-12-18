#ifndef __ITEXTURE1D_H__
#define __ITEXTURE1D_H__

#include "IAssetImpl.h"


interface ITexture1D : public IASSET_IMPL<ASSET_TYPE::TEXTURE1D>
{
	/**
	* @brief 텍스처 너비 반환
	* @return 텍스처 너비
	*/
	virtual uint32 GetWidth() const noexcept = 0;

	/**
	* @brief MIP 레벨 수 반환
	* @return MIP 레벨 수
	*/
	virtual uint32 GetMipLevels() const noexcept = 0;

	/**
	* @brief 텍스처 컬러 포맷 반환
	* @return 텍스처 컬러 포맷
	*/
	virtual COLOR_FORMAT GetFormat() const noexcept = 0;

};

#endif // __ITEXTURE1D_H__
