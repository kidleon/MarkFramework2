#pragma once
#include <source_location>
#include <string>


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
		static void log_msg(
			FormatWithLocation<std::type_identity_t<Args>...> fl,
			Args&&... args
		)
		{
			// 로그 메시지 포맷팅
			std::string body = std::format(fl.fmt, std::forward<Args>(args)...);

			body += " (at " + std::string(fl.loc.file_name()) + ":" + std::to_string(fl.loc.line()) + ")";
			log_impl(category, level, body);
		}

	private:
		static void log_impl(log_category category, log_level level, const std::string& msg);

	};
}

#if defined(__LOG_ENABLED__)
#define SYS_LOG(...)				mark::log::log_msg<mark::log_category::system, mark::log_level::info>(__VA_ARGS__)
#define SYS_LOG_WRN(...)			mark::log::log_msg<mark::log_category::system, mark::log_level::warning>(__VA_ARGS__)
#define SYS_LOG_ERR(...)			mark::log::log_msg<mark::log_category::system, mark::log_level::error>(__VA_ARGS__)
#define SYS_LOG_CRIT(...)			mark::log::log_msg<mark::log_category::system, mark::log_level::critical>(__VA_ARGS__)

#define LOG(...)					mark::log::log_msg<mark::log_category::gameplay, mark::log_level::info>(__VA_ARGS__)
#define LOG_WRN(...)				mark::log::log_msg<mark::log_category::gameplay, mark::log_level::warning>(__VA_ARGS__)
#define LOG_ERR(...)				mark::log::log_msg<mark::log_category::gameplay, mark::log_level::error>(__VA_ARGS__)
#define LOG_CRIT(...)				mark::log::log_msg<mark::log_category::gameplay, mark::log_level::critical>(__VA_ARGS__)
#else
#define SYS_LOG(...)				0
#define SYS_LOG_WRN(...)			0
#define SYS_LOG_ERR(...)			0
#define SYS_LOG_CRIT(...)			0

#define LOG(...)					0
#define LOG_WRN(...)				0
#define LOG_ERR(...)				0
#define LOG_CRIT(...)				0
#endif // __LOG_ENABLED__
