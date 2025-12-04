#ifndef __LOG_H__
#define __LOG_H__


enum LOG_LEVEL
{
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_DEBUG
};

enum LOG_OUTPUT
{
	LOG_OUTPUT_CONSOLE = 0x1,
	LOG_OUTPUT_FILE = 0x2,
	LOG_OUTPUT_OUTPUT_DEBUG_STRING = 0x4
};


MARKENGINE_C_API void log_init(
	uint32 level,
	uint32 output
);

MARKENGINE_C_API void log_shutdown();

MARKENGINE_C_API void log_info(
	const char* format,
	...
);

MARKENGINE_C_API void log_warning(
	const char* format,
	...
);

MARKENGINE_C_API void log_error(
	const char* format,
	...
);

MARKENGINE_C_API void log_debug(
	const char* format,
	...
);

MARKENGINE_C_API void sys_log_info(
	const char* format,
	...
);

MARKENGINE_C_API void sys_log_warning(
	const char* format,
	...
);

MARKENGINE_C_API void sys_log_error(
	const char* format,
	...
);

MARKENGINE_C_API void sys_log_debug(
	const char* format,
	...
);


#define SYS_LOG_I(...) sys_log_info(__VA_ARGS__)
#define SYS_LOG_W(...) sys_log_warning(__VA_ARGS__)
#define SYS_LOG_E(...) sys_log_error(__VA_ARGS__)
#define SYS_LOG_D(...) sys_log_debug(__VA_ARGS__)

#define LOG_I(...) log_info(__VA_ARGS__)
#define LOG_W(...) log_warning(__VA_ARGS__)
#define LOG_E(...) log_error(__VA_ARGS__)
#define LOG_D(...) log_debug(__VA_ARGS__)

#endif // __LOG_H__