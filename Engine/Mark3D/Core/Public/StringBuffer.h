#pragma once
#include <charconv>
#include <format>
#include "CoreGeneric.h"


namespace mark
{
	template<size_t InitialCapacity = 1024>
	class string_buffer
	{
	public:
		string_buffer()
		{
			m_buffer.reserve(InitialCapacity);
		}

		template<typename... Args>
		[[nodiscard]] inline static sys_string sys_format(
			std::format_string<Args...> fmt,
			Args&&... args)
		{
			sys_vector<char> buffer;
			std::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
			return sys_string(buffer.begin(), buffer.end());
		}
	
		template<typename... Args>
		inline string_buffer& format(std::basic_format_string<char, std::type_identity_t<Args>...> fmt, Args&&... args)
		{
			auto end = std::format_to(std::back_inserter(m_buffer), fmt, std::forward<Args>(args)...);
			return *this;
		}

		inline string_buffer& format(std::string_view fmt, std::format_args args)
		{
			auto end = std::vformat_to(std::back_inserter(m_buffer), fmt, args);
			return *this;
		}

		inline string_buffer& append_endl()
		{
			return append('\n');
		}

		inline string_buffer& append(const char* str, size_t count)
		{
			m_buffer.insert(m_buffer.end(), str, str + count);
			return *this;
		}

		inline string_buffer& append(const std::basic_string<char>& str)
		{
			return append(str.data(), str.size());
		}

		inline string_buffer& append(const char* str)
		{
			return append(str, std::char_traits<char>::length(str));
		}

		inline string_buffer& append(char ch)
		{
			m_buffer.push_back(ch);
			return *this;
		}

		inline string_buffer& append(bool value)
		{
			return append(value ? "true" : "false");
		}

		inline string_buffer& append(int value)
		{
			char buffer[64]; // 충분히 큰 버퍼
			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());
			return append(buffer, result.ptr - buffer);
		}

		inline string_buffer& append(unsigned int value)
		{
			char buffer[64]; // 충분히 큰 버퍼
			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());
			return append(buffer, result.ptr - buffer);
		}

		inline string_buffer& append(float value)
		{
			char buffer[64]; // 충분히 큰 버퍼

			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());

			return append(buffer, result.ptr - buffer);
		}

		inline string_buffer& append(double value)
		{
			char buffer[64]; // 충분히 큰 버퍼
			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());
			return append(buffer, result.ptr - buffer);
		}

		inline string_buffer& append(const sys_string& str)
		{
			return append(str.data(), str.size());
		}

		inline string_buffer& append(const temp_string& str)
		{
			return append(str.data(), str.size());
		}

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

		[[nodiscard]] inline std::basic_string_view<char> to_string_view() const noexcept
		{
			return std::basic_string_view<char>(m_buffer.data(), m_buffer.size());
		}

		inline void to_buffer(char* out_buffer, size_t buffer_size) const
		{
			assert(out_buffer && buffer_size > 0 && "Output buffer must be non-null and have positive size");

			// buffer_size는 바이트 단위이므로, char 크기에 맞게 문자 수로 변환
			size_t char_count = buffer_size / sizeof(char);

			// buffer_size가 작을 경우 작은 크기에 맞게 잘라서 복사
			size_t copy_size = std::min(char_count - 1, m_buffer.size()); // -1은 null-terminator 공간 확보를 위해
			std::copy_n(m_buffer.data(), copy_size, out_buffer);

			// 복사한 문자열 뒤에 null-terminator 추가
			out_buffer[copy_size] = char(0); // null-terminate
		}

		inline void to_string(std::basic_string<char>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline void to_sys_string(std::basic_string<char, std::char_traits<char>, ALLOC::SYS<char>>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline void to_temp_string(std::basic_string<char, std::char_traits<char>, ALLOC::TEMP<char>>& out_str) const
		{
			out_str.assign(m_buffer.data(), m_buffer.size());
		}

		inline const char* c_str()
		{
			// to_buffer()를 사용하여 C 스타일 문자열로 변환
			m_buffer_to_c.reserve(m_buffer.size() + 1); // null-terminator 공간 확보
			m_buffer_to_c.assign(m_buffer.begin(), m_buffer.end());
			m_buffer_to_c.push_back(char(0)); // null-terminate
			return m_buffer_to_c.data();
		}

		[[nodiscard]] inline size_t size() const noexcept
		{
			return m_buffer.size();
		}

	private:
		sys_vector<char> m_buffer; // 내부 버퍼 (sync_pool_allocator 사용)
		sys_vector<char> m_buffer_to_c; // to_buffer()에서 C 스타일 문자열로 변환할 때 사용할 임시 버퍼

	};

	
}
