#ifndef __FNV_H__
#define __FNV_H__


typedef enum tagCHAR_CASE
{
	C_ANY,		/**< 대소문자 모두 */
	C_LOWER,	/**< 소문자 */
	C_UPPER		/**< 대문자 */
} CHAR_CASE;

/**
* @brief fnv1 Hash의 기본으로 제공되는 초기값
*/
#define FNV1_INITIAL_VALUE 2166136261U

/**
* @brief fnv64 Hash의 기본으로 제공되는 초기값
*/
#define FNV64_INITIAL_VALUE UINT64_C(14695981039346656037)

/**
* fnv1
* @brief 주어진 데이터로부터 fnv1(32bit) 방식의 Hash값을 얻어옵니다.
* @param data 해쉬값을 알고자 하는 데이터
* @param bytes data의 길이
* @param initial 해쉬 초기값
* @result 부호없는 32비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint32 fnv1(
	const void* data,
	size_t bytes,
	uint32 initial
);

/**
* fnv1_c
* @brief 주어진 데이터로부터 fnv1(32bit) 방식의 Hash값을 얻어옵니다. 기본값을 이용해 초기화 합니다.
* @param data 해쉬값을 알고자 하는 데이터
* @param bytes data의 길이
* @result 부호없는 32비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint32 fnv1_c(
	const void* data,
	size_t bytes
);

/**
* fnv1_cstr
* @brief 8bit 문자열로부터 fnv1(32bit) 방식의 Hash값을 얻어옵니다.
* @param str 해쉬값을 알고자 하는 데이터
* @param char_case string의 대소문자 처리를 강제 한다.
* @param initial 해쉬 초기값
* @result 부호없는 32비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint32 fnv1_cstr(
	const char* str,
	CHAR_CASE char_case,
	uint32 initial
);

/**
* fnv1_cstr_c
* @brief 8bit 문자열로부터 fnv1(32bit) 방식의 Hash값을 얻어옵니다. 기본값을 이용해 초기화 합니다.
* @param str 해쉬값을 알고자 하는 데이터
* @param char_case string의 대소문자 처리를 강제 한다.
* @result 부호없는 32비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint32 fnv1_cstr_c(
	const char* str,
	CHAR_CASE char_case
);

/**
* fnv64
* @brief 주어진 데이터로부터 fnv64(64bit) 방식의 Hash값을 얻어옵니다.
* @param data 해쉬값을 알고자 하는 데이터
* @param bytes data의 길이
* @param initial 해쉬 초기값
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint64 fnv64(
	const void* data,
	size_t bytes,
	uint64 initial
);

/**
* fnv64_c
* @brief 주어진 데이터로부터 fnv64(64bit) 방식의 Hash값을 얻어옵니다. 기본값을 이용해 초기화 합니다.
* @param data 해쉬값을 알고자 하는 데이터
* @param bytes data의 길이
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint64 fnv64_c(
	const void* data,
	size_t bytes
);

/**
* fnv64_cstr
* @brief 8bit 문자열로부터 fnv64(64bit) 방식의 Hash값을 얻어옵니다.
* @param szStr 해쉬값을 알고자 하는 데이터
* @param Case string의 대소문자 처리를 강제 한다.
* @param Initial 해쉬 초기값
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint64 fnv64_cstr(
	const char* str,
	CHAR_CASE char_case,
	uint64 initial
);

/**
* fnv64_cstr_c
* @brief 8bit 문자열로부터 fnv64(64bit) 방식의 Hash값을 얻어옵니다. 기본값을 이용해 초기화 합니다.
* @param szStr 해쉬값을 알고자 하는 데이터
* @param Case string의 대소문자 처리를 강제 한다.
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARK_BASELIB_C_API uint64 fnv64_cstr_c(
	const char* str,
	CHAR_CASE char_case
);

#endif // __FNV_H__