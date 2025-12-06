#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__


/**
* @brief 현재 시간을 "YYYY-MM-DD HH:MM:SS" 형식의 문자열로 가져옵니다.
* @param buffer 시간을 저장할 버퍼
* @param size 버퍼의 크기
* @return 없음
*/
MARK_BASELIB_C_API void get_timestamp_full(
	char* buffer, 
	size_t size
);


#endif // __SYS_TIME_H__