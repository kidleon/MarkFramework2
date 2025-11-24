#ifndef __FNV_H__
#define __FNV_H__


enum CHAR_CASE
{
	C_ANY,		/**< 대소문자 모두 */
	C_LOWER,	/**< 소문자 */
	C_UPPER		/**< 대문자 */
};

/**
* @brief fnv1 Hash의 기본으로 제공되는 초기값
*/
static constexpr uint32 FNV1_INITIAL_VALUE = 2166136261U;

/**
* @brief fnv64 Hash의 기본으로 제공되는 초기값
*/
static constexpr uint64 FNV64_INITIAL_VALUE = UINT64_C(14695981039346656037);

/**
* fnv1
* @brief 주어진 데이터로부터 fnv1(32bit) 방식의 Hash값을 얻어옵니다.
* @param pData 해쉬값을 알고자 하는 데이터
* @param Bytes data의 길이
* @param Initial 해쉬 초기값
* @result 부호없는 32비트 정수형 해쉬값을 리턴합니다.
*/
MARKENGINE_C_API uint32 fnv1(
	const void* pData,
	size_t Bytes,
	uint32 Initial = FNV1_INITIAL_VALUE
);

/**
* fnv1_cstr
* @brief 8bit 문자열로부터 fnv1(32bit) 방식의 Hash값을 얻어옵니다.
* @param szStr 해쉬값을 알고자 하는 데이터
* @param Case string의 대소문자 처리를 강제 한다.
* @param Initial 해쉬 초기값
* @result 부호없는 32비트 정수형 해쉬값을 리턴합니다.
*/
MARKENGINE_C_API uint32 fnv1_cstr(
	const char* szStr,
	CHAR_CASE Case,
	uint32 Initial = FNV1_INITIAL_VALUE
);

/**
* fnv64
* @brief 주어진 데이터로부터 fnv64(64bit) 방식의 Hash값을 얻어옵니다.
* @param pData 해쉬값을 알고자 하는 데이터
* @param Bytes data의 길이
* @param Initial 해쉬 초기값
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARKENGINE_C_API uint64 fnv64(
	const void* pData,
	size_t Bytes,
	uint64 Initial = FNV64_INITIAL_VALUE
);

/**
* fnv64_cstr
* @brief 8bit 문자열로부터 fnv64(64bit) 방식의 Hash값을 얻어옵니다.
* @param szStr 해쉬값을 알고자 하는 데이터
* @param Case string의 대소문자 처리를 강제 한다.
* @param Initial 해쉬 초기값
* @result 부호없는 64비트 정수형 해쉬값을 리턴합니다.
*/
MARKENGINE_C_API uint64 fnv64_cstr(
	const char* szStr,
	CHAR_CASE Case,
	uint64 Initial = FNV64_INITIAL_VALUE
);

#endif // __FNV_H__