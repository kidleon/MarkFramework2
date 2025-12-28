#include "pch.h"
#include "Log.h"
#include "OSConsoleLog.h"
#include "crt_memory.h"


struct log_t
{
	uint32 min_level;
	uint32 log_type;
	uint32 output;

};


static log_t* log_impl = nullptr;

void log_init(
	uint32 min_level,
	uint32 log_type,
	uint32 output
)
{
	// 이미 초기화된 경우 무시
	if (log_impl)
		return;

	log_impl = (log_t*)crt_malloc(sizeof(log_t));

	if (output & LOG_OUTPUT_CONSOLE)
		create_console();

	log_impl->min_level = min_level;
	log_impl->output = output;
	log_impl->log_type = log_type;
}

void log_shutdown()
{
	if (!log_impl) return;

	if (log_impl->output & LOG_OUTPUT_CONSOLE)
		destroy_console();
}

void log_info(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_INFO) return;

	if (log_impl->log_type & LOG_TYPE_APPLICATION)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_info(format, args);

		va_end(args);
	}
}

void log_warning(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_WARNING) return;

	if (log_impl->log_type & LOG_TYPE_APPLICATION)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_warning(format, args);

		va_end(args);
	}
}

void log_error(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_ERROR) return;

	if (log_impl->log_type & LOG_TYPE_APPLICATION)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_error(format, args);

		va_end(args);
	}
}

void log_debug(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_DEBUG) return;

	if (log_impl->log_type & LOG_TYPE_APPLICATION)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_debug(format, args);

		va_end(args);
	}

	
}

void sys_log_info(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_INFO) return;

	if (log_impl->log_type & LOG_TYPE_SYSTEM)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_info(format, args);

		va_end(args);
	}
}

void sys_log_warning(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_WARNING) return;

	if (log_impl->log_type & LOG_TYPE_SYSTEM)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_warning(format, args);

		va_end(args);
	}
}

void sys_log_error(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_ERROR) return;
	if (log_impl->log_type & LOG_TYPE_SYSTEM)
	{
		va_list args;
		va_start(args, format);
		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_error(format, args);
		va_end(args);
	}
}

void sys_log_debug(
	const char* format,
	...
)
{
	if (!log_impl || log_impl->min_level > LOG_LEVEL_DEBUG) return;

	if (log_impl->log_type & LOG_TYPE_SYSTEM)
	{
		va_list args;
		va_start(args, format);

		if (log_impl->output & LOG_OUTPUT_CONSOLE)
			log_debug(format, args);

		va_end(args);
	}
}
