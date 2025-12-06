#ifndef __LOG_H__
#define __LOG_H__

// 로그 레벨
enum LOG_LEVEL
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
};

// 로그 출력 터입
enum LOG_OUTPUT
{
	LOG_OUTPUT_CONSOLE = 0x1,
	LOG_OUTPUT_FILE = 0x2,
	LOG_OUTPUT_OUTPUT_DEBUG_STRING = 0x4,
	LOG_OUTPUT_WINDOW_EVENT = 0x8
};

// 로그 타입
enum LOG_TYPE
{
	LOG_TYPE_SYSTEM = 0x01,
	LOG_TYPE_APPLICATION = 0x02
};;

/**
* @brief 로그 시스템 초기화 함수
* @param min_level 최소 로그 레벨
* @param log_type 로그 타입 (시스템, 애플리케이션 등)
* @param output 로그 출력 방식 (콘솔, 파일 등)
* @return 없음
*/
MARKENGINE_C_API void log_init(
	uint32 min_level,
	uint32 log_type,
	uint32 output
);

/**
* @brief 로그 시스템 종료 함수
* @return 없음
*/
MARKENGINE_C_API void log_shutdown();

/**
* @brief 정보성 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void log_info(
	const char* format,
	...
);

/**
* @brief 경고성 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void log_warning(
	const char* format,
	...
);

/**
* @brief 오류성 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void log_error(
	const char* format,
	...
);

/**
* @brief 디버그용 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void log_debug(
	const char* format,
	...
);

/**
* @brief 시스템 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void sys_log_info(
	const char* format,
	...
);

/**
* @brief 시스템 경고성 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void sys_log_warning(
	const char* format,
	...
);

/**
* @brief 시스템 오류성 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void sys_log_error(
	const char* format,
	...
);

/**
* @brief 시스템 디버그용 로그 출력 함수
* @param format 출력할 형식 문자열
* @return 없음
*/
MARKENGINE_C_API void sys_log_debug(
	const char* format,
	...
);


#if defined (USE_SYS_LOG)
#	define SYS_LOG_I(...) sys_log_info(__VA_ARGS__)
#	define SYS_LOG_W(...) sys_log_warning(__VA_ARGS__)
#	define SYS_LOG_E(...) sys_log_error(__VA_ARGS__)
#	define SYS_LOG_D(...) sys_log_debug(__VA_ARGS__)
#else 
#	define SYS_LOG_I(...) 0
#	define SYS_LOG_W(...) 0
#	define SYS_LOG_E(...) 0
#	define SYS_LOG_D(...) 0
#endif // defined (USE_SYS_LOG)

#if defined (USE_LOG)
#	define LOG_I(...) log_info(__VA_ARGS__)
#	define LOG_W(...) log_warning(__VA_ARGS__)
#	define LOG_E(...) log_error(__VA_ARGS__)
#	define LOG_D(...) log_debug(__VA_ARGS__)
#else
#	define LOG_I(...) 0
#	define LOG_W(...) 0
#	define LOG_E(...) 0
#	define LOG_D(...) 0
#endif // defined (USE_LOG)

#endif // __LOG_H__