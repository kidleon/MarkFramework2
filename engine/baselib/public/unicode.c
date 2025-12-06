#include "pch.h"
#include "unicode.h"
#include <iconv.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <wchar.h>
#include <limits.h>


BOOL is_valid_utf8(
    const unsigned char* data, 
    size_t len
) 
{
    size_t i = 0;

    while (i < len) 
    {
        unsigned char byte = data[i];
        int num_bytes = 0;
        
        if ((byte & 0x80) == 0) // ASCII (0xxxxxxx)
        {
            i++;
            continue;
        }
        else if ((byte & 0xE0) == 0xC0) // 2바이트 (110xxxxx)
        {
            num_bytes = 1;
        }
        
        else if ((byte & 0xF0) == 0xE0) // 3바이트 (1110xxxx)
        {
            num_bytes = 2;
        }
        
        else if ((byte & 0xF8) == 0xF0) // 4바이트 (11110xxx)
        {
            num_bytes = 3;
        }
        else // 잘못된 시작 바이트
        {
            return FALSE;
        }

        if (i + num_bytes >= len) // 충분한 바이트가 있는지 확인
        {
            return FALSE;
        }

        // 연속 바이트 확인 (10xxxxxx)
        for (int j = 1; j <= num_bytes; j++) 
        {
            if ((data[i + j] & 0xC0) != 0x80) 
            {
                return FALSE;
            }
        }

        i += num_bytes + 1;
    }

    return TRUE;
}

ENCODING_TYPE detect_bom(
    const unsigned char* data,
    size_t len
) 
{
    if (len < 2) return ENCODING_UNKNOWN;

    // UTF-8 BOM: EF BB BF
    if (len >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) 
        return ENCODING_UTF8;

    // UTF-16 LE BOM: FF FE
    if (data[0] == 0xFF && data[1] == 0xFE) 
    {
        // UTF-32 LE: FF FE 00 00
        if (len >= 4 && data[2] == 0x00 && data[3] == 0x00) 
        {
            return ENCODING_UTF32_LE;
        }
        return ENCODING_UTF16_LE;
    }

    // UTF-16 BE BOM: FE FF
    if (data[0] == 0xFE && data[1] == 0xFF)
        return ENCODING_UTF16_BE;

    // UTF-32 BE BOM: 00 00 FE FF
    if (len >= 4 && 
        data[0] == 0x00 &&
        data[1] == 0x00 &&
        data[2] == 0xFE &&
        data[3] == 0xFF
    )
    {
        return ENCODING_UTF32_BE;
    }

    return ENCODING_UNKNOWN;
}

 // 공통 변환 헬퍼 함수
static BOOL convert_encoding(
    const char* from_encoding,
    const char* to_encoding,
    const char* src,
    size_t src_bytes,
    char* dest,
    size_t dest_bytes
) 
{
    if (!src || !dest || src_bytes == 0 || dest_bytes == 0) 
        return FALSE;

    // iconv 디스크립터 열기
    iconv_t cd = iconv_open(to_encoding, from_encoding);
    if (cd == (iconv_t)-1)
        return FALSE;

    // iconv는 포인터를 수정하므로 복사본 사용
    char* inbuf = (char*)src;
    char* outbuf = dest;
    size_t inbytesleft = src_bytes;
    size_t outbytesleft = dest_bytes;

    // 변환 수행
    size_t result = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);

    iconv_close(cd);

    // 에러 체크
    if (result == (size_t)-1) 
    {
        if (errno == E2BIG) 
        {
            // 출력 버퍼 부족
            return FALSE;
        }
        else if (errno == EILSEQ) 
        {
            // 잘못된 입력 시퀀스
            return FALSE;
        }
        else if (errno == EINVAL) 
        {
            // 불완전한 입력 시퀀스
            return FALSE;
        }

        return FALSE;
    }

    // NULL 종료 (가능한 경우)
    if (outbytesleft > 0) 
    {
        *outbuf = '\0';
    }

    return TRUE;
}

BOOL conv_ansi_to_utf8(
    const char* src,
    size_t src_size,
    char* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_ANSI, 
        MARK_ENCODING_UTF8, 
        src, 
        src_size, 
        dest,
        dest_size
    );
}

BOOL conv_ansi_to_utf16(
    const char* src,
    size_t src_size,
    char16_t* dest,
    size_t dest_size
)
{
    // dest_size은 바이트 수로 가정
    return convert_encoding(
        MARK_ENCODING_ANSI, 
        MARK_ENCODING_UTF16,
        src, 
        src_size, 
        (char*)dest,
        dest_size
    );
}

BOOL conv_ansi_to_wchar(
    const char* src,
    size_t src_size,
    wchar_t* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_ANSI, 
        MARK_ENCODING_WCHAR, 
        src, 
        src_size, 
        (char*)dest, 
        dest_size
    );
}

BOOL conv_ansi_to_utf32(
    const char* src,
    size_t src_size,
    char32_t* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_ANSI, 
        MARK_ENCODING_UTF32, 
        src, 
        src_size,
        (char*)dest, 
        dest_size
    );
}

BOOL conv_utf8_to_utf16(
    const char* src,
    size_t src_size,
    char16_t* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_UTF8, 
        MARK_ENCODING_UTF16, 
        src, 
        src_size, 
        (char*)dest, 
        dest_size
    );
}

BOOL conv_utf8_to_wchar(
    const char* src,
    size_t src_size,
    wchar_t* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_UTF8, 
        MARK_ENCODING_WCHAR,
        src, 
        src_size, 
        (char*)dest,
        dest_size
    );
}

BOOL conv_utf8_to_utf32(
    const char* src,
    size_t src_size,
    char32_t* dest,
    size_t dest_size)
{
    return convert_encoding(
        MARK_ENCODING_UTF8,
        MARK_ENCODING_UTF32,
        src,
        src_size, 
        (char*)dest,
        dest_size
    );
}

BOOL conv_utf32_to_utf8(
    const char32_t* src,
    size_t src_size,
    char* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_UTF32,
        MARK_ENCODING_UTF8, 
        (char*)src, 
        src_size, 
        (char*)dest,
        dest_size
    );
}

BOOL conv_utf16_to_utf8(
    const char16_t* src,
    size_t src_size,
    char* dest,
    size_t dest_size
)
{
    return convert_encoding(
        MARK_ENCODING_UTF16, 
        MARK_ENCODING_UTF8,
        (char*)src,
        src_size, 
        (char*)dest, 
        dest_size
    );
}

BOOL conv_wchar_to_utf8(
    const wchar_t* src,
    size_t src_size,
    char* dest,
    size_t dest_size)
{
    return convert_encoding(
        MARK_ENCODING_WCHAR, 
        MARK_ENCODING_UTF8, 
        (char*)src,
        src_size, 
        (char*)dest,
        dest_size
    );
}
