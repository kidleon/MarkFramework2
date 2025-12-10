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

	virtual uint32 ComputeCRC32() noexcept = 0;
	virtual uint64 ComputeCRC64() noexcept = 0;
};


#endif // __IBINARYASSET_H__
