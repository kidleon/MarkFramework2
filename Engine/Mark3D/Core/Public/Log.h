#pragma once
#include <source_location>
#include <string>
#include "CoreGeneric.h"


namespace mark
{
	enum class log_category : uint32_t
	{
		system = 0,
		gameplay,
	};

	enum class log_level : uint32_t
	{
		info = 0,
		warning,
		error,
		critical
	};

	enum class log_target : uint32_t
	{
		console = 0x0001,
		file = 0x0002,
		all = console | file
	};

	template<typename... Args>
	struct FormatWithLocation
	{
		std::format_string<Args...> fmt;
		std::source_location loc;

		// consteval로 포맷 문자열은 컴파일 타임에 검증,
		// source_location::current()는 호출 지점(매크로 전개 위치)에서 캡처됨
		template<typename T>
		consteval FormatWithLocation(
			const T& s,
			std::source_location l = std::source_location::current()
		)
			: fmt{ s }, loc{ l }
		{
		}
	};

	template<size_t InitialCapacity = 2048>
	class std_string_buffer
	{
	public:
		std_string_buffer()
		{
			m_buffer.reserve(InitialCapacity);
		}

		template<typename... Args>
		inline std_string_buffer& format(
			std::format_string<Args...> fmt,
			Args&&... args)
		{
			std::format_to(std::back_inserter(m_buffer), fmt, std::forward<Args>(args)...);
			return *this;
		}
		
		inline std_string_buffer& append_endl()
		{
			return append('\n');
		}

		inline std_string_buffer& append(const char* str, size_t count)
		{
			m_buffer.insert(m_buffer.end(), str, str + count);
			return *this;
		}

		inline std_string_buffer& append(const std::basic_string<char>& str)
		{
			return append(str.data(), str.size());
		}

		inline std_string_buffer& append(const char* str)
		{
			return append(str, std::char_traits<char>::length(str));
		}

		inline std_string_buffer& append(char ch)
		{
			m_buffer.push_back(ch);
			return *this;
		}

		inline std_string_buffer& append(bool value)
		{
			return append(value ? "true" : "false");
		}

		inline std_string_buffer& append(int value)
		{
			char buffer[64]; // 충분히 큰 버퍼
			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());
			return append(buffer, result.ptr - buffer);
		}

		inline std_string_buffer& append(unsigned int value)
		{
			char buffer[64]; // 충분히 큰 버퍼
			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());
			return append(buffer, result.ptr - buffer);
		}

		inline std_string_buffer& append(float value)
		{
			char buffer[64]; // 충분히 큰 버퍼

			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());

			return append(buffer, result.ptr - buffer);
		}

		inline std_string_buffer& append(double value)
		{
			char buffer[64]; // 충분히 큰 버퍼
			std::to_chars_result result = std::to_chars(buffer, buffer + sizeof(buffer), value);
			assert(result.ec == std::errc());
			return append(buffer, result.ptr - buffer);
		}

		inline std_string_buffer& append(const sys_string& str)
		{
			return append(str.data(), str.size());
		}

		inline std_string_buffer& append(const spool_string& str)
		{
			return append(str.data(), str.size());
		}

		inline std_string_buffer& append(const upool_string& str)
		{
			return append(str.data(), str.size());
		}

		inline std_string_buffer& append(const temp_string& str)
		{
			return append(str.data(), str.size());
		}

		inline std_string_buffer& append(std::string_view str)
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

			size_t char_count = buffer_size;

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

	private:
		std::vector<char> m_buffer; // 내부 버퍼 (sync_pool_allocator 사용)

	};

	class MARKENGINE_API log
	{
	public:
		/**
		* @brief 로그 시스템 초기화. 로그 레벨과 타겟을 설정한다.
		*
		* @param log_level 로그 레벨 플래그 (bitwise OR 조합). 예: log_level::info | log_level::error
		* @param log_target 로그 타겟 플래그 (bitwise OR 조합). 예: log_target::console | log_target::file
		*
		* @note log_level 플래그는 현재 구현에서는 사용되지 않지만, 향후 레벨별 필터링 기능 추가 시 활용할 수 있다.
		*/
		static void initialize(uint32_t log_level, uint32_t log_target);

		/**
		* @brief 로그 시스템 종료. 모든 로그를 플러시하고 리소스를 해제한다.
		* @return 없음
		*/
		static void shutdown();

		/**
		* @brief 로그 메시지 기록. 포맷 문자열과 가변 인자를 받아 로그를 기록한다.
		* @tparam category 로그 카테고리 (system, gameplay 등)
		* @tparam level 로그 레벨 (info, warning, error, critical)
		* @tparam Args 포맷 문자열에 사용되는 인자들의 타입
		* @param fl 포맷 문자열과 호출 위치 정보를 담은 구조체
		* @param args 포맷 문자열에 전달할 가변 인자들
		* @return 없음
		*/
		template<log_category category, log_level level, typename... Args>
		static void log_msg_f(
			FormatWithLocation<std::type_identity_t<Args>...> fl,
			Args&&... args
		)
		{
			thread_local std_string_buffer<2048> str_buf;
			str_buf.format(fl.fmt, std::forward<Args>(args)...);
			str_buf.append(" (at ");
			str_buf.append(fl.loc.file_name());
			str_buf.append(":");
			str_buf.append(fl.loc.line());
			str_buf.append(")");

			log_f_impl(category, level, str_buf.to_string_view());
		}

		template<log_category category, log_level level>
		static void log_msg(const char* msg, std::source_location loc)
		{
			if (!msg) [[unlikely]]
				return;

			log_impl(category, level, msg, loc);
		}

		template<log_category category, log_level level>
		static void log_msg(const sys_string& str, std::source_location loc)
		{
			if (str.empty()) [[unlikely]]
				return;

			log_impl(category, level, str.c_str(), loc);
		}

		template<log_category category, log_level level>
		static void log_msg(const spool_string& str, std::source_location loc)
		{
			if (str.empty()) [[unlikely]]
				return;

			log_impl(category, level, str.c_str(), loc);
		}

		template<log_category category, log_level level>
		static void log_msg(const upool_string& str, std::source_location loc)
		{
			if (str.empty()) [[unlikely]]
				return;

			log_impl(category, level, str.c_str(), loc);
		}

		template<log_category category, log_level level>
		static void log_msg(const temp_string& str, std::source_location loc)
		{
			if (str.empty()) [[unlikely]]
				return;

			log_impl(category, level, str.c_str(), loc);
		}

		static void log_f_impl(
			log_category category,
			log_level    level,
			std::string_view msg
		);

		static void log_impl(
			log_category category,
			log_level    level,
			const char* msg,
			std::source_location loc
		);

	};
}


#if defined(__LOG_ENABLED__)
#define SYS_LOG_F(...)				mark::log::log_msg_f<mark::log_category::system, mark::log_level::info>(__VA_ARGS__)
#define SYS_LOG_WRN_F(...)			mark::log::log_msg_f<mark::log_category::system, mark::log_level::warning>(__VA_ARGS__)
#define SYS_LOG_ERR_F(...)			mark::log::log_msg_f<mark::log_category::system, mark::log_level::error>(__VA_ARGS__)
#define SYS_LOG_CRIT_F(...)			mark::log::log_msg_f<mark::log_category::system, mark::log_level::critical>(__VA_ARGS__)

#define LOG_F(...)					mark::log::log_msg_f<mark::log_category::gameplay, mark::log_level::info>(__VA_ARGS__)
#define LOG_WRN_F(...)				mark::log::log_msg_f<mark::log_category::gameplay, mark::log_level::warning>(__VA_ARGS__)
#define LOG_ERR_F(...)				mark::log::log_msg_f<mark::log_category::gameplay, mark::log_level::error>(__VA_ARGS__)
#define LOG_CRIT_F(...)				mark::log::log_msg_f<mark::log_category::gameplay, mark::log_level::critical>(__VA_ARGS__)

#define SYS_LOG(msg)				mark::log::log_msg<mark::log_category::system, mark::log_level::info>(msg, std::source_location::current())
#define SYS_LOG_WRN(msg)			mark::log::log_msg<mark::log_category::system, mark::log_level::warning>(msg, std::source_location::current())
#define SYS_LOG_ERR(msg)			mark::log::log_msg<mark::log_category::system, mark::log_level::error>(msg, std::source_location::current())
#define SYS_LOG_CRIT(msg)			mark::log::log_msg<mark::log_category::system, mark::log_level::critical>(msg, std::source_location::current())

#define LOG(msg)					mark::log::log_msg<mark::log_category::gameplay, mark::log_level::info>(msg, std::source_location::current())
#define LOG_WRN(msg)				mark::log::log_msg<mark::log_category::gameplay, mark::log_level::warning>(msg, std::source_location::current())
#define LOG_ERR(msg)				mark::log::log_msg<mark::log_category::gameplay, mark::log_level::error>(msg, std::source_location::current())
#define LOG_CRIT(msg)				mark::log::log_msg<mark::log_category::gameplay, mark::log_level::critical>(msg, std::source_location::current())

#else
#define SYS_LOG_F(...)				0
#define SYS_LOG_WRN_F(...)			0
#define SYS_LOG_ERR_F(...)			0
#define SYS_LOG_CRIT_F(...)			0

#define LOG_F(...)					0
#define LOG_WRN_F(...)				0
#define LOG_ERR_F(...)				0
#define LOG_CRIT_F(...)				0

#define SYS_LOG(...)				0
#define SYS_LOG_WRN(...)			0
#define SYS_LOG_ERR(...)			0
#define SYS_LOG_CRIT(...)			0

#define LOG(...)					0
#define LOG_WRN(...)				0
#define LOG_ERR(...)				0
#define LOG_CRIT(...)				0
#endif // __LOG_ENABLED__
