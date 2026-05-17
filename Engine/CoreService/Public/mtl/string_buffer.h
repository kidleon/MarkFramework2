#pragma once
#include <algorithm>
#include <charconv>
#include <format>
#include <memory>    // std::allocator
#include <type_traits>
#include <vector>
#include <string>
#include <string_view>
#include <cassert>

namespace mtl
{
	template<size_t InitialCapacity = 1024, typename Alloc = std::allocator<char>>
	class string_buffer
	{
	public:
		// 할당자 타입 별칭
		using allocator_type = Alloc;
		using buffer_type = std::vector<char, Alloc>;

		// 기본 생성 — 기본 할당자 사용
		string_buffer()
		{
			m_buffer.reserve(InitialCapacity);
		}

		// 외부 할당자 인스턴스를 받아서 생성
		explicit string_buffer(const Alloc& alloc)
			: m_buffer(alloc)
			, m_buffer_to_c(alloc)
		{
			m_buffer.reserve(InitialCapacity);
		}

		template<typename StringAlloc = std::allocator<char>, typename... Args>
		[[nodiscard]] inline static std::basic_string<char, std::char_traits<char>, StringAlloc>
			make_string(
				std::format_string<Args...> fmt,
				Args&&... args)
		{
			std::vector<char> buf;
			std::format_to(std::back_inserter(buf), fmt, std::forward<Args>(args)...);
			return std::basic_string<char, std::char_traits<char>, StringAlloc>(
				buf.begin(), buf.end());
		}

		template<typename... Args>
		inline string_buffer& format(
			std::format_string<Args...> fmt,
			Args&&... args
		)
		{
			std::format_to(std::back_inserter(m_buffer), fmt, std::forward<Args>(args)...);
			return *this;
		}

		inline string_buffer& format(std::string_view fmt, std::format_args args)
		{
			std::vformat_to(std::back_inserter(m_buffer), fmt, args);
			return *this;
		}

		// 개행 추가
		inline string_buffer& append_endl()
		{
			return append('\n');
		}

		// raw 포인터 + 길이
		inline string_buffer& append(const char* str, size_t count)
		{
			m_buffer.insert(m_buffer.end(), str, str + count);
			return *this;
		}

		// std::string (어떤 할당자든 받을 수 있도록 템플릿)
		template<typename StrAlloc>
		inline string_buffer& append(const std::basic_string<char, std::char_traits<char>, StrAlloc>& str)
		{
			return append(str.data(), str.size());
		}

		// C 문자열
		inline string_buffer& append(const char* str)
		{
			return append(str, std::char_traits<char>::length(str));
		}

		// 단일 문자
		inline string_buffer& append(char ch)
		{
			m_buffer.push_back(ch);
			return *this;
		}

		// bool → "true" / "false"
		inline string_buffer& append(bool value)
		{
			return append(value ? "true" : "false");
		}

		// 정수/부동소수점 → std::to_chars로 고속 변환
		// char/bool은 위쪽 전용 오버로드가 가져감 (exact match가 템플릿보다 우선).
		template<typename T>
		inline std::enable_if_t<
			std::is_arithmetic_v<T>
			&& !std::is_same_v<std::remove_cv_t<T>, bool>
			&& !std::is_same_v<std::remove_cv_t<T>, char>,
			string_buffer&>
		append(T value)
		{
			return append_numeric(value);
		}

		// string_view
		inline string_buffer& append(std::string_view str)
		{
			return append(str.data(), str.size());
		}

		inline void reserve(size_t new_capacity)
		{
			m_buffer.reserve(new_capacity);
		}

		inline void clear()
		{
			m_buffer.clear();
		}

		[[nodiscard]] inline size_t size() const noexcept
		{
			return m_buffer.size();
		}

		// string_view로 변환 (복사 없음)
		[[nodiscard]] inline std::string_view to_string_view() const noexcept
		{
			return std::string_view(m_buffer.data(), m_buffer.size());
		}

		// 외부 char 버퍼에 null-terminated 복사
		inline void to_buffer(char* out_buffer, size_t buffer_size) const
		{
			assert(out_buffer && buffer_size > 0
				&& "Output buffer must be non-null and have positive size");

			size_t copy_size = std::min(buffer_size - 1, m_buffer.size());
			std::copy_n(m_buffer.data(), copy_size, out_buffer);
			out_buffer[copy_size] = '\0';
		}

		// 임의 할당자의 std::basic_string으로 출력
		template<typename OutAlloc = std::allocator<char>>
		inline void to_string(
			std::basic_string<char, std::char_traits<char>, OutAlloc>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		// C 스타일 문자열 반환 (내부 임시 버퍼 사용)
		inline const char* c_str()
		{
			m_buffer_to_c.assign(m_buffer.begin(), m_buffer.end());
			m_buffer_to_c.push_back('\0');
			return m_buffer_to_c.data();
		}

		// 할당자 접근
		[[nodiscard]] inline allocator_type get_allocator() const noexcept
		{
			return m_buffer.get_allocator();
		}

	private:
		// 숫자 타입 공통 변환 (코드 중복 제거)
		template<typename NumT>
		inline string_buffer& append_numeric(NumT value)
		{
			char buf[64];
			auto result = std::to_chars(buf, buf + sizeof(buf), value);
			assert(result.ec == std::errc());
			return append(buf, static_cast<size_t>(result.ptr - buf));
		}

		buffer_type m_buffer;        // 메인 버퍼
		buffer_type m_buffer_to_c;   // c_str() 전용 임시 버퍼
	};
	
}
