#ifndef __CRC64_H__
#define __CRC64_H__


/**
* @brief 데이터로부터 CRC64(64bit) 방식의 Hash값을 얻어옵니다.
* @param data 해쉬값을 알고자 하는 데이터
* @param length data의 길이
* @param initialize 해쉬 초기값
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARKENGINE_C_API uint64 crc64(
	const void* pData, 
	size_t length,
	uint64 initialize
);

#endif // __CRC64_H__ 