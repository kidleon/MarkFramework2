#pragma once

namespace mark
{
	/**
	* @brief 문자열 인코딩 감지 및 변환을 위한 클래스입니다.
	*/
	class MARKENGINE_API unicode
	{
	public:
		enum class ENCODING_TYPE : uint8_t
		{
			ENCODING_UNKNOWN,
			ENCODING_UTF8,
			ENCODING_UTF8_BOM,
			ENCODING_UTF16LE,
			ENCODING_UTF16LE_BOM,
			ENCODING_UTF16BE,
			ENCODING_UTF16BE_BOM,
			ENCODING_UTF32LE,
			ENCODING_UTF32LE_BOM,
			ENCODING_UTF32BE,
			ENCODING_UTF32BE_BOM
		};

	public:
		static ENCODING_TYPE detect_encoding(const char* data, size_t size);

		/**
		* @brief 문자열 인코딩을 변환합니다.
		* @param src_encoding 원본 문자열의 인코딩 타입
		* @param dest_encoding 변환할 대상 인코딩 타입
		* @param src 변환할 원본 문자열의 포인터
		* @param src_size 원본 문자열의 크기 (바이트 단위)
		* @param dest 변환된 문자열을 저장할 버퍼의 포인터
		* @param dest_size 변환된 문자열을 저장할 버퍼의 크기 (바이트 단위)
		* @return 변환 성공 시 true, 실패 시 false를 반환.
		*/
		static bool conv(
			ENCODING_TYPE src_encoding,
			ENCODING_TYPE dest_encoding,
			const char* src,
			size_t src_size,
			char* dest,
			size_t dest_size
		);

		static bool conv_ansi_to_utf8(
			const char* src,
			size_t src_size,
			char* dest,
			size_t dest_size
		);

		static bool conv_ansi_to_utf16(
			const char* src,
			size_t src_size,
			char16_t* dest,
			size_t dest_size
		);

		static bool conv_ansi_to_wchar(
			const char* src,
			size_t src_size,
			wchar_t* dest,
			size_t dest_size
		);

		static bool conv_ansi_to_utf32(
			const char* src,
			size_t src_size,
			char32_t* dest,
			size_t dest_size
		);

		static bool conv_utf8_to_utf16(
			const char* src,
			size_t src_size,
			char16_t* dest,
			size_t dest_size
		);

		static bool conv_utf8_to_wchar(
			const char* src,
			size_t src_size,
			wchar_t* dest,
			size_t dest_size
		);

		static bool conv_utf8_to_utf32(
			const char* src,
			size_t src_size,
			char32_t* dest,
			size_t dest_size
		);

		static bool conv_utf32_to_utf8(
			const char32_t* src,
			size_t src_size,
			char* dest,
			size_t dest_size
		);

		static bool conv_utf16_to_utf8(
			const char16_t* src,
			size_t src_size,
			char* dest,
			size_t dest_size
		);

		static bool conv_wchar_to_utf8(
			const wchar_t* src,
			size_t src_size,
			char* dest,
			size_t dest_size
		);
	};
}
