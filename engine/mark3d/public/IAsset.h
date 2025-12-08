#ifndef __IASSET_H__
#define __IASSET_H__


interface IAsset : public IUNKNOWN
{
	/**
	* @brief 자산 고유 ID 반환
	* @return 자산 고유 ID
	*/
	virtual uint32 GetID() const noexcept = 0;

	/**
	* @brief 자산 유형 반환
	* @return 자산 유형
	*/
	virtual ASSET_TYPE GetAssetType() const noexcept = 0;

	/**
	* @brief 로드 상태 반환
	* @return 로드 상태
	*/
	virtual LOAD_STAT GetLoadStat() const noexcept = 0;
};

#endif // __IASSET_H__
