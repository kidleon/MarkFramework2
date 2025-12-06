#ifndef __UNICODE_H__
#define __UNICODE_H__


typedef enum {
    ENCODING_UNKNOWN,
    ENCODING_UTF8,
    ENCODING_UTF16_LE,
    ENCODING_UTF16_BE,
    ENCODING_UTF32_LE,
    ENCODING_UTF32_BE
} ENCODING_TYPE;

/**
* @@brief UTF-8 유효성 검사
* @param data 검사할 데이터 포인터
* @param len 데이터 길이 (바이트 단위)
* @return 유효한 UTF-8 문자열이면 TRUE, 아니면 FALSE
*/
MARK_BASELIB_C_API BOOL is_valid_utf8(
    const unsigned char* data,
    size_t len
);

/**
* @brief BOM(Byte Order Mark) 감지 (문자열의 인코딩 유형 판별, 없으면 ENCODING_UNKNOWN 반환)
* @param data 검사할 데이터 포인터
* @param len 데이터 길이 (바이트 단위, 최소 2바이트 이상)
* @return 감지된 인코딩 유형
*/
MARK_BASELIB_C_API ENCODING_TYPE detect_bom(
    const unsigned char* data,
    size_t len
);

/**
* @brief ANSI 변환(WINDOWS일 경우에는 CP949, Unix계열일때는 UTF-8로 간주)
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_ansi_to_utf8(
    const char* src,
    size_t src_size,
    char* dest,
    size_t dest_size
);

/**
* @brief ANSI to UTF-16 변환(WINDOWS일 경우에는 CP949, Unix계열일때는 UTF-8로 간주)
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_ansi_to_utf16(
    const char* src,
    size_t src_size,
    char16_t* dest,
    size_t dest_size
);

/**
* @brief ANSI to wchar_t 변환(WINDOWS일 경우에는 CP949, Unix계열일때는 UTF-8로 간주, wchar_t 크기는 플랫폼에 따름)
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_ansi_to_wchar(
    const char* src,
    size_t src_size,
    wchar_t* dest,
    size_t dest_size
);

/**
* @brief ANSI to UTF-32 변환(WINDOWS일 경우에는 CP949, Unix계열일때는 UTF-8로 간주)
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_ansi_to_utf32(
    const char* src,
    size_t src_size,
    char32_t* dest,
    size_t dest_size
);

/**
* @brief UTF-8 to UTF-16 변환
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_utf8_to_utf16(
    const char* src,
    size_t src_size,
    char16_t* dest,
    size_t dest_size
);

/**
* @brief UTF-8 to wchar_t 변환
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_utf8_to_wchar(
    const char* src,
    size_t src_size,
    wchar_t* dest,
    size_t dest_size
);

/**
* @brief UTF-8 to UTF-32 변환
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_utf8_to_utf32(
    const char* src,
    size_t src_size,
    char32_t* dest,
    size_t dest_size
);

/**
* @brief UTF-32 to UTF-8 변환
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_utf32_to_utf8(
    const char32_t* src,
    size_t src_size,
    char* dest,
    size_t dest_size
);

/**
* @brief UTF-16 to UTF-8 변환
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_utf16_to_utf8(
    const char16_t* src,
    size_t src_size,
    char* dest,
    size_t dest_size
);

/**
* @brief wchar_t to UTF-8 변환
* @param src 변환할 소스 문자열
* @param src_size 소스 문자열 크기 (바이트 단위)
* @param dest 변환된 문자열을 저장할 버퍼
* @param dest_size dest 버퍼 크기 (바이트 단위)
* @return 변환 성공 시 TRUE, 실패 시 FALSE
*/
MARK_BASELIB_C_API BOOL conv_wchar_to_utf8(
    const wchar_t* src,
    size_t src_size,
    char* dest,
    size_t dest_size
);

#endif // __UNICODE_H__
