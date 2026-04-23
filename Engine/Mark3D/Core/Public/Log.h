#pragma once


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
		* @brief 로그 메시지를 출력한다.
		* @param category 로그 카테고리
		* @param format 출력할 메시지 형식 문자열
		* @param ... 가변 인자
		* @return 없음
		*/
		static void log_msg(log_category category, const char* format, ...);

		/**
		* @brief 경고 로그 메시지를 출력한다.
		* @param category 로그 카테고리
		* @param format 출력할 메시지 형식 문자열
		* @param ... 가변 인자
		* @return 없음
		*/
		static void log_wrn(log_category category, const char* format, ...);

		/**
		* @brief 오류 로그 메시지를 출력한다.
		* @param category 로그 카테고리
		* @param format 출력할 메시지 형식 문자열
		* @param ... 가변 인자
		* @return 없음
		*/
		static void log_err(log_category category, const char* format, ...);

		/**
		* @brief 치명적 오류 로그 메시지를 출력한다. 이 레벨의 로그는 즉시 플러시되어 유실을 방지한다.
		* @param category 로그 카테고리
		* @param format 출력할 메시지 형식 문자열
		* @param ... 가변 인자
		* @return 없음
		*/
		static void log_crit(log_category category, const char* format, ...);

	};
}

#if defined(__LOG_ENABLED__)
#define SYS_LOG(...)				mark::log::log_msg(mark::log_category::system, __VA_ARGS__)
#define SYS_LOG_WRN(...)			mark::log::log_wrn(mark::log_category::system, __VA_ARGS__)
#define SYS_LOG_ERR(...)			mark::log::log_err(mark::log_category::system, __VA_ARGS__)
#define SYS_LOG_CRIT(...)			mark::log::log_crit(mark::log_category::system, __VA_ARGS__)

#define LOG(...)					mark::log::log_msg(mark::log_category::gameplay, __VA_ARGS__)
#define LOG_WRN(...)				mark::log::log_wrn(mark::log_category::gameplay, __VA_ARGS__)
#define LOG_ERR(...)				mark::log::log_err(mark::log_category::gameplay, __VA_ARGS__)
#define LOG_CRIT(...)				mark::log::log_crit(mark::log_category::gameplay, __VA_ARGS__)
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
