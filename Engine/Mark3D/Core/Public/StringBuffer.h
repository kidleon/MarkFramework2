#pragma once
#include <charconv>
#include <format>
#include "CoreGeneric.h"


namespace mark
{
	template<typename CharT, size_t InitialCapacity = 1024>
	class string_buffer
	{
	public:
		string_buffer()
		{
			m_buffer.reserve(InitialCapacity);

			if constexpr (!std::is_same_v<CharT, char> &&
				!std::is_same_v<CharT, wchar_t> &&
				!std::is_same_v<CharT, char16_t> &&
				!std::is_same_v<CharT, char32_t>)
			{
				static_assert(always_false<CharT>, "Unsupported character type");
			}
		}

		/**
		*  // ── 너비 지정 ──────────────────────────────────────────
		*	std::format("{:4}",   n);   // "  15"   (우측 정렬, 공백 채움)
		*	std::format("{:<4}",  n);   // "15  "   (좌측 정렬)
		*	std::format("{:>4}",  n);   // "  15"   (우측 정렬, 명시적)
		*	std::format("{:^4}",  n);   // " 15 "   (가운데 정렬)
		*	// ── 제로 패딩 ──────────────────────────────────────────
		*	std::format("{:04}",  n);   // "0015"   ← 원하시던 것
		*	std::format("{:08}",  n);   // "00000015"
		*	std::format("{:06}",  n);   // "000015"
		*	// ── 채움 문자 지정 ─────────────────────────────────────
		*	std::format("{:*<6}", n);   // "15****"  (왼쪽 정렬, * 채움)
		*	std::format("{:*>6}", n);   // "****15"  (오른쪽 정렬, * 채움)
		*	std::format("{:*^6}", n);   // "**15**"  (가운데, * 채움)
		*	std::format("{:0>6}", n);   // "000015"  (0 채움, 오른쪽 정렬)
		*	// ── 소수점 자릿수 ──────────────────────────────────────
		*	std::format("{:.0f}", v);   // "233"         소수점 없음
		*	std::format("{:.1f}", v);   // "233.2"
		*	std::format("{:.2f}", v);   // "233.23"      ← 원하시던 것
		*	std::format("{:.3f}", v);   // "233.232"
		*	std::format("{:.6f}", v);   // "233.232130"
		*	// ── 너비 + 소수점 조합 ─────────────────────────────────
		*	std::format("{:10.2f}",  v);  // "    233.23"  (총 10자리)
		*	std::format("{:010.2f}", v);  // "0000233.23"  (0 패딩 + 소수 2자리)
		*	std::format("{:<10.2f}", v);  // "233.23    "  (왼쪽 정렬)
		*	std::format("{:^10.2f}", v);  // "  233.23  "  (가운데 정렬)
		*	// ── 표기법 선택 ────────────────────────────────────────
		*	std::format("{:f}", v);     // "233.232130"   고정 소수점
		*	std::format("{:e}", v);     // "2.323213e+02" 과학적 표기법
		*	std::format("{:E}", v);     // "2.323213E+02" 대문자 E
		*	std::format("{:g}", v);     // "233.232"      더 짧은 쪽 자동 선택
		*	std::format("{:.2e}", v);   // "2.32e+02"     과학적 + 정밀도
		*	std::format("{:+}", pos);   // "+42"   양수에도 + 표시
		*	std::format("{:+}", neg);   // "-42"
		*	std::format("{:-}", pos);   // "42"    음수만 - (기본값)
		*	std::format("{: }", pos);   // " 42"   양수는 공백, 음수는 -
		*	std::format("{: }", neg);   // "-42"
		*	// 너비와 조합
		*	std::format("{:+08.2f}",  3.14);  // "+0003.14"
		*	std::format("{:+08.2f}", -3.14);  // "-0003.14"
		*	// ── 기본 ───────────────────────────────────────────────
		*	std::format("{:d}",  n);    // "255"        10진수 (기본)
		*	std::format("{:b}",  n);    // "11111111"   2진수
		*	std::format("{:o}",  n);    // "377"        8진수
		*	std::format("{:x}",  n);    // "ff"         16진수 소문자
		*	std::format("{:X}",  n);    // "FF"         16진수 대문자
		*	// ── 접두사 (#) ─────────────────────────────────────────
		*	std::format("{:#b}",  n);   // "0b11111111"
		*	std::format("{:#o}",  n);   // "0377"
		*	std::format("{:#x}",  n);   // "0xff"
		*	std::format("{:#X}",  n);   // "0XFF"
		*	// ── 접두사 + 제로 패딩 ─────────────────────────────────
		*	std::format("{:#010x}", n); // "0x000000ff"  (총 10자리)
		*	std::format("{:#010b}", n); // "0b11111111"  (총 10자리)
		*	// ── 채움 문자 ──────────────────────────────────────────
		*	std::format("{:*^10}", s);  // "**hello***"
		*	std::format("{:-^10}", s);  // "--hello---"
		*	// ── 최대 길이 (정밀도) ─────────────────────────────────
		*	std::format("{:.3}",   s);  // "hel"         3자리까지만
		*	std::format("{:10.3}", s);  // "hel       "  너비 10, 최대 3자
		*	// ── bool ───────────────────────────────────────────────
		*	std::format("{}",   true);   // "true"
		*	std::format("{}",   false);  // "false"
		*	std::format("{:d}", true);   // "1"     숫자로 출력
		*	std::format("{:d}", false);  // "0"
		*	// ── char ───────────────────────────────────────────────
		*	std::format("{}",   'A');    // "A"     문자로 출력
		*	std::format("{:d}", 'A');    // "65"    ASCII 코드로 출력
		*	std::format("{:c}", 65);     // "A"     숫자를 문자로 출력
		*	// ── 순서 변경 ──────────────────────────────────────────
		*	std::format("{0} {1}", "hello", "world");  // "hello world"
		*	std::format("{1} {0}", "hello", "world");  // "world hello"
		*	// ── 같은 인자 여러 번 사용 ─────────────────────────────
		*	std::format("{0}:{0}:{1}", 192, 168);      // "192:192:168"
		*	std::format("[{0}] [{0}] [{0}]", "반복");  // "[반복] [반복] [반복]"
		*	// ── 금액 표시 ──────────────────────────────────────────
		*	double price = 1234567.8;
		*	std::format("{:,.2f}", price);      // ※ 천단위 콤마는 {:L} 로케일 필요
		*	std::format("{:.2f}", price);       // "1234567.80"
		*	// ── 시간 표시 ──────────────────────────────────────────
		*	int h = 9, m = 5, s = 3;
		*	std::format("{:02}:{:02}:{:02}", h, m, s);  // "09:05:03"
		*	// ── 진행률 ─────────────────────────────────────────────
		*	double pct = 72.5;
		*	std::format("[{:>6.1f}%]", pct);    // "[  72.5%]"
		*	// ── 버전 번호 ──────────────────────────────────────────
		*	std::format("v{}.{}.{}", 1, 2, 3);  // "v1.2.3"
		*	// ── 테이블 정렬 ────────────────────────────────────────
		*	std::format("{:<12} {:>8} {:>8}", "Alice", 9900, 1);
		*	// "Alice           9900        1"
		*	// ── 16진수 메모리 주소 ─────────────────────────────────
		*	void* ptr = nullptr;
		*	std::format("0x{:016X}", reinterpret_cast<uintptr_t>(ptr));
		*	// "0x0000000000000000"
		*	// ── 소수점 자리 + 부호 + 패딩 조합 ────────────────────
		*	std::format("{:+010.2f}", 3.14);    // "+000003.14"
		*	std::format("{:+010.2f}", -3.14);   // "-000003.14"
		*/
		template<typename... Args>
		inline string_buffer& format(std::basic_format_string<CharT, std::type_identity_t<Args>...> fmt, Args&&... args)
		{
			auto end = std::format_to(std::back_inserter(m_buffer), fmt, std::forward<Args>(args)...);
			return *this;
		}

		inline string_buffer& append_endl()
		{
			if constexpr (std::is_same_v<CharT, char>)
			{
				return append('\n');
			}
			else if constexpr (std::is_same_v<CharT, wchar_t>)
			{
				return append(L'\n');
			}
			else if constexpr (std::is_same_v<CharT, char16_t>)
			{
				return append(u'\n');
			}
			else if constexpr (std::is_same_v<CharT, char32_t>)
			{
				return append(U'\n');
			}
			else
			{
				static_assert(always_false<CharT>, "Unsupported character type");
				return *this; // 컴파일 오류 방지용
			}
		}

		inline string_buffer& append(const CharT* str, size_t count)
		{
			m_buffer.insert(m_buffer.end(), str, str + count);
			return *this;
		}

		inline string_buffer& append(const std::basic_string<CharT>& str)
		{
			return append(str.data(), str.size());
		}

		inline string_buffer& append(const CharT* str)
		{
			return append(str, std::char_traits<CharT>::length(str));
		}

		inline string_buffer& append(CharT ch)
		{
			m_buffer.push_back(ch);
			return *this;
		}

		inline string_buffer& append(bool value)
		{
			if constexpr (std::is_same_v<CharT, char>)
			{
				return append(value ? "true" : "false");
			}
			else if constexpr (std::is_same_v<CharT, wchar_t>)
			{
				std::basic_string_view<wchar_t> str = value ? (std::basic_string_view<wchar_t>(L"true")) : (std::basic_string_view<wchar_t>(L"false"));
				return append(str.data(), str.size());
			}
			else if constexpr (std::is_same_v<CharT, char16_t>)
			{
				std::basic_string_view<char16_t> str = value ? (std::basic_string_view<char16_t>(u"true")) : (std::basic_string_view<char16_t>(u"false"));
				return append(str.data(), str.size());
			}
			else if constexpr (std::is_same_v<CharT, char32_t>)
			{
				std::basic_string_view<char32_t> str = value ? (std::basic_string_view<char32_t>(U"true")) : (std::basic_string_view<char32_t>(U"false"));
				return append(str.data(), str.size());
			}
			else
			{
				static_assert(always_false<CharT>, "Unsupported character type");
				return *this; // 컴파일 오류 방지용
			}
		}

		inline string_buffer& append(int value)
		{
			if constexpr(std::is_same_v<CharT, char>)
			{
				char buffer[64]; // 충분히 큰 버퍼
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
				assert(result.ec == std::errc());
				return append(buffer, result.ptr - buffer);
			}
			else if constexpr (std::is_same_v<CharT, wchar_t>)
			{
				wchar_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, L"{}", value);
				return append(buffer, std::char_traits<wchar_t>::length(buffer));
			}
			else if constexpr (std::is_same_v<CharT, char16_t>)
			{
				char16_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, u"{}", value);
				return append(buffer, std::char_traits<char16_t>::length(buffer));
			}
			else if constexpr (std::is_same_v<CharT, char32_t>)
			{
				char32_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, U"{}", value);
				return append(buffer, std::char_traits<char32_t>::length(buffer));
			}
			else
			{
				static_assert(always_false<CharT>, "Unsupported character type");
				return *this; // 컴파일 오류 방지용
			}
		}

		inline string_buffer& append(float value)
		{
			if constexpr (std::is_same_v<CharT, char>)
			{
				char buffer[64]; // 충분히 큰 버퍼

				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
				assert(result.ec == std::errc());

				return append(buffer, result.ptr - buffer);
			}
			else if constexpr (std::is_same_v<CharT, wchar_t>)
			{
				wchar_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, L"{}", value);
				return append(buffer, std::char_traits<wchar_t>::length(buffer));
			}
			else if constexpr (std::is_same_v<CharT, char16_t>)
			{
				char16_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, u"{}", value);
				return append(buffer, std::char_traits<char16_t>::length(buffer));
			}
			else if constexpr (std::is_same_v<CharT, char32_t>)
			{
				char32_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, U"{}", value);
				return append(buffer, std::char_traits<char32_t>::length(buffer));
			}
			else
			{
				static_assert(always_false<CharT>, "Unsupported character type");
				return *this; // 컴파일 오류 방지용
			}
		}

		inline string_buffer& append(double value)
		{
			if constexpr (std::is_same_v<CharT, char>)
			{
				char buffer[64]; // 충분히 큰 버퍼
				std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
				assert(result.ec == std::errc());
				return append(buffer, result.ptr - buffer);
			}
			else if constexpr (std::is_same_v<CharT, wchar_t>)
			{
				wchar_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, L"{}", value);
				return append(buffer, std::char_traits<wchar_t>::length(buffer));
			}
			else if constexpr (std::is_same_v<CharT, char16_t>)
			{
				char16_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, u"{}", value);
				return append(buffer, std::char_traits<char16_t>::length(buffer));
			}
			else if constexpr (std::is_same_v<CharT, char32_t>)
			{
				char32_t buffer[64]; // 충분히 큰 버퍼
				std::format_to(buffer, U"{}", value);
				return append(buffer, std::char_traits<char32_t>::length(buffer));
			}
			else
			{
				static_assert(always_false<CharT>, "Unsupported character type");
				return *this; // 컴파일 오류 방지용
			}
		}

		inline void reserve(size_t new_capacity)
		{
			m_buffer.reserve(new_capacity);
		}

		inline void clear()
		{
			m_buffer.clear();
		}

		[[nodiscard]] inline std::basic_string_view<CharT> to_string_view() const noexcept
		{
			return std::basic_string_view<CharT>(m_buffer.data(), m_buffer.size());
		}

		inline void to_buffer(CharT* out_buffer, size_t buffer_size) const
		{
			assert(out_buffer && buffer_size > 0 && "Output buffer must be non-null and have positive size");

			// buffer_size는 바이트 단위이므로, CharT 크기에 맞게 문자 수로 변환
			size_t char_count = buffer_size / sizeof(CharT);

			// buffer_size가 작을 경우 작은 크기에 맞게 잘라서 복사
			size_t copy_size = std::min(char_count - 1, m_buffer.size()); // -1은 null-terminator 공간 확보를 위해
			std::copy_n(m_buffer.data(), copy_size, out_buffer);

			// 복사한 문자열 뒤에 null-terminator 추가
			out_buffer[copy_size] = CharT(0); // null-terminate
		}

		inline void to_string(std::basic_string<CharT>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline void to_sys_string(std::basic_string<CharT, std::char_traits<CharT>, ALLOC::SYS<CharT>>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline void to_spool_string(std::basic_string<CharT, std::char_traits<CharT>, ALLOC::SPOOL<CharT>>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline void to_upool_string(std::basic_string<CharT, std::char_traits<CharT>, ALLOC::UPOOL<CharT>>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline void to_temp_string(std::basic_string<CharT, std::char_traits<CharT>, ALLOC::TEMP<CharT>>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

	private:
		upool_vector<CharT> m_buffer; // 내부 버퍼 (sync_pool_allocator 사용)

	};
}
