#ifndef __IBINARYASSET_H__
#define __IBINARYASSET_H__

#include "IAsset.h"


interface IBinaryAsset : public IAsset
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
	* @brief CRC32 해시 값 반환. 처음 계산 후 캐싱됨
	* @return CRC32 해시 값
	*/
	virtual uint32 ComputeCRC32() noexcept = 0;

	/**
	* @brief CRC64 해시 값 반환. 처음 계산 후 캐싱됨
	* @return CRC64 해시 값
	*/
	virtual uint64 ComputeCRC64() noexcept = 0;
};


#endif // __IBINARYASSET_H__
