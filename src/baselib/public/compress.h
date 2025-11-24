#ifndef __COMPRESS_H__
#define __COMPRESS_H__


/**
* @brief lz4 압축을 수행합니다.
* @param src_data 압축할 데이터의 포인터
* @param src_size 압축할 데이터의 사이즈
* @param dest_buffer 압축된 데이터를 저장할 버퍼의 포인터
* @param dest_size 압축된 데이터를 저장할 버퍼의 사이즈
* @return 압축된 데이터의 사이즈를 반환합니다. 실패시 0을 반환합니다.
*/
MARKENGINE_C_API size_t compress(
	char* src_data, 
	size_t src_size, 
	char* dest_buffer,
	size_t dest_size
);

/**
* @brief lz4 압축 해제를 수행합니다.
* @param src_data 압축 해제할 데이터의 포인터
* @param src_size 압축 해제할 데이터의 사이즈
* @param dest_buffer 압축 해제된 데이터를 저장할 버퍼의 포인터
* @param dest_size 압축 해제된 데이터를 저장할 버퍼의 사이즈
* @return 압축 해제된 데이터의 사이즈를 반환합니다. 실패시 0을 반환합니다.
*/
MARKENGINE_C_API size_t decompress(
	char* src_data,
	size_t src_size,
	char* dest_buffer,
	size_t dest_size
);

/**
* @brief lz4로 압축된 데이터의 최대 사이즈를 반환합니다.
* @param src_size 압축할 데이터의 사이즈
* @return 압축된 데이터의 최대 사이즈를 반환합니다.
*/
MARKENGINE_C_API size_t compress_size(
	size_t src_size
);


#endif // __COMPRESS_H__
