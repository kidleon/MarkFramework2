#ifndef __OS_CONSOLE_LOG_H__
#define __OS_CONSOLE_LOG_H__


extern void create_console();
extern void destroy_console();
extern void log_info(const char* format, va_list args);
extern void log_warning(const char* format, va_list args);
extern void log_error(const char* format, va_list args);
extern void log_debug(const char* format, va_list args);


#endif // __OS_CONSOLE_LOG_H__
