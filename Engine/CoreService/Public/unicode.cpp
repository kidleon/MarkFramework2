#include "pch.h"
#include "unicode.h"
#if defined(__linux__) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#	include <iconv.h>
#else
#	include <iconv/iconv.h>
#endif // __linux__ || __unix__ || (__APPLE__ && __MACH__)


namespace mark
{
	static bool convert_encoding(
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

		char* inptr = inbuf;
		char* outptr = outbuf;

		size_t result = iconv(
			cd,
#if defined(__APPLE__)
			(char**)&inptr,
#else
			(const char**)&inptr,
#endif
			&inbytesleft,
			&outptr,
			&outbytesleft);

		// Update outbuf to current position for null-termination below
		outbuf = outptr;

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

	unicode::ENCODING_TYPE unicode::detect_encoding(const char* data, size_t size)
	{
		// 데이터가 없으면 판별 불가
		if (data == nullptr || size == 0)
			return ENCODING_TYPE::ENCODING_UNKNOWN;

		const uint8_t* buf = reinterpret_cast<const uint8_t*>(data);

		if (size >= 4)
		{
			// UTF-32 LE BOM: FF FE 00 00
			if (buf[0] == 0xFF && buf[1] == 0xFE &&
				buf[2] == 0x00 && buf[3] == 0x00)
				return ENCODING_TYPE::ENCODING_UTF32LE_BOM;

			// UTF-32 BE BOM: 00 00 FE FF
			if (buf[0] == 0x00 && buf[1] == 0x00 &&
				buf[2] == 0xFE && buf[3] == 0xFF)
				return ENCODING_TYPE::ENCODING_UTF32BE_BOM;
		}

		if (size >= 3)
		{
			// UTF-8 BOM: EF BB BF
			if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
				return ENCODING_TYPE::ENCODING_UTF8_BOM;
		}

		if (size >= 2)
		{
			// UTF-16 LE BOM: FF FE
			if (buf[0] == 0xFF && buf[1] == 0xFE)
				return ENCODING_TYPE::ENCODING_UTF16LE_BOM;

			// UTF-16 BE BOM: FE FF
			if (buf[0] == 0xFE && buf[1] == 0xFF)
				return ENCODING_TYPE::ENCODING_UTF16BE_BOM;
		}

		// 휴리스틱 판별.. 정확하지 않을수 있음..
		const size_t sample_size = size < 256 ? size : 256;

		// ── UTF-32 패턴 검사 (4바이트 단위) ──
		if (sample_size >= 4)
		{
			int score_utf32_le = 0;  // [XX 00 00 00] 패턴 카운트
			int score_utf32_be = 0;  // [00 00 00 XX] 패턴 카운트
			int groups = 0;  // 검사한 4바이트 그룹 수

			for (size_t i = 0; i + 3 < sample_size; i += 4, ++groups)
			{
				// UTF-32 LE: 첫 바이트만 값이 있고 나머지 3바이트가 0x00
				if (buf[i] != 0x00 &&
					buf[i + 1] == 0x00 &&
					buf[i + 2] == 0x00 &&
					buf[i + 3] == 0x00)
					++score_utf32_le;

				// UTF-32 BE: 앞 3바이트가 0x00이고 마지막 바이트만 값이 있음
				if (buf[i] == 0x00 &&
					buf[i + 1] == 0x00 &&
					buf[i + 2] == 0x00 &&
					buf[i + 3] != 0x00)
					++score_utf32_be;
			}

			// 전체 그룹의 절반 이상이 패턴에 부합하면 UTF-32로 판정
			if (groups > 0)
			{
				if (score_utf32_le > groups / 2)
					return ENCODING_TYPE::ENCODING_UTF32LE;

				if (score_utf32_be > groups / 2)
					return ENCODING_TYPE::ENCODING_UTF32BE;
			}
		}

		// ── UTF-16 패턴 검사 (2바이트 단위) ──
		if (sample_size >= 2)
		{
			int score_utf16_le = 0;  // [XX 00] 패턴 카운트
			int score_utf16_be = 0;  // [00 XX] 패턴 카운트
			int pairs = 0;  // 검사한 2바이트 쌍 수

			for (size_t i = 0; i + 1 < sample_size; i += 2, ++pairs)
			{
				// UTF-16 LE: 첫 바이트에 값, 두 번째 바이트가 0x00
				if (buf[i] != 0x00 && buf[i + 1] == 0x00)
					++score_utf16_le;

				// UTF-16 BE: 첫 바이트가 0x00, 두 번째 바이트에 값
				if (buf[i] == 0x00 && buf[i + 1] != 0x00)
					++score_utf16_be;
			}

			// 전체 쌍의 절반 이상이 패턴에 부합하면 UTF-16으로 판정
			if (pairs > 0)
			{
				if (score_utf16_le > pairs / 2)
					return ENCODING_TYPE::ENCODING_UTF16LE;

				if (score_utf16_be > pairs / 2)
					return ENCODING_TYPE::ENCODING_UTF16BE;
			}
		}

		// ── 위 패턴 모두 해당 없음 → UTF-8 (no BOM) 으로 가정 ──
		return ENCODING_TYPE::ENCODING_UTF8;
	}

	bool unicode::conv(
		ENCODING_TYPE src_encoding,
		ENCODING_TYPE dest_encoding,
		const char* src,
		size_t src_size,
		char* dest,
		size_t dest_size
	)
	{
		if (!src || !dest || src_size == 0 || dest_size == 0)
			return false;

		const char* from_enc = nullptr;
		const char* to_enc = nullptr;

		switch (src_encoding)
		{
		case ENCODING_TYPE::ENCODING_UTF8:
			from_enc = "UTF-8";
			break;

		case ENCODING_TYPE::ENCODING_UTF16LE:
			from_enc = "UTF-16LE";
			break;

		case ENCODING_TYPE::ENCODING_UTF16BE:
			from_enc = "UTF-16BE";
			break;

		case ENCODING_TYPE::ENCODING_UTF32LE:
			from_enc = "UTF-32LE";
			break;

		case ENCODING_TYPE::ENCODING_UTF32BE:
			from_enc = "UTF-32BE";
			break;

		default:
			return FALSE; // 지원하지 않는 인코딩
		}


		switch (dest_encoding)
		{
		case ENCODING_TYPE::ENCODING_UTF8:
			to_enc = "UTF-8";
			break;

		case ENCODING_TYPE::ENCODING_UTF16LE:
			to_enc = "UTF-16LE";
			break;

		case ENCODING_TYPE::ENCODING_UTF16BE:
			to_enc = "UTF-16BE";
			break;

		case ENCODING_TYPE::ENCODING_UTF32LE:
			to_enc = "UTF-32LE";
			break;

		case ENCODING_TYPE::ENCODING_UTF32BE:
			to_enc = "UTF-32BE";
			break;

		default:
			return false; // 지원하지 않는 인코딩
		}

		return convert_encoding(from_enc, to_enc, src, src_size, dest, dest_size);
	}

	bool unicode::conv_ansi_to_utf8(
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

	bool unicode::conv_ansi_to_utf16(
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

	bool unicode::conv_ansi_to_wchar(
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

	bool unicode::conv_ansi_to_utf32(
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

	bool unicode::conv_utf8_to_utf16(
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

	bool unicode::conv_utf8_to_wchar(
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

	bool unicode::conv_utf8_to_utf32(
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

	bool unicode::conv_utf32_to_utf8(
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

	bool unicode::conv_utf16_to_utf8(
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

	bool unicode::conv_wchar_to_utf8(
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
}

