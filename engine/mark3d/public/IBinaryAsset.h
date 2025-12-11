#ifndef __IBINARYASSET_H__
#define __IBINARYASSET_H__

#include "IAssetImpl.h"


interface IBinaryAsset : public IASSET_IMPL<ASSET_TYPE::BINARY>
{
	/**
	* @brief 바이너리 데이터 포인터 반환
	* @return 바이너리 데이터 포인터
	*/
	virtual const char* GetData() const noexcept = 0;

	/**
	* @brief 바이너리 데이터 크기 반환
	* @return 바이너리 데이터 크기 (바이트 단위)
	*/
	virtual size_t GetSize() const noexcept = 0;

	/**
	* @brief CRC32 계산
	* @return CRC32 값
	*/
	virtual uint32 ComputeCRC32() noexcept = 0;

	/**
	* @brief CRC64 계산
	* @return CRC64 값
	*/
	virtual uint64 ComputeCRC64() noexcept = 0;
};


#endif // __IBINARYASSET_H__
