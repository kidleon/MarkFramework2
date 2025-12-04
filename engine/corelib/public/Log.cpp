#include "pch.h"
#include "Log.h"


void log_init(
	uint32 level,
	uint32 output
)
{

}

void log_shutdown()
{

}

void log_info(
	const char* format,
	...
);

void log_warning(
	const char* format,
	...
);

void log_error(
	const char* format,
	...
);

void log_debug(
	const char* format,
	...
);

void sys_log_info(
	const char* format,
	...
);

void sys_log_warning(
	const char* format,
	...
);

void sys_log_error(
	const char* format,
	...
);

void sys_log_debug(
	const char* format,
	...
);
