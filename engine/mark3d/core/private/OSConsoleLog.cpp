#include "pch.h"
#include "OSConsoleLog.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "sys_time.h"
#include "Log.h"


/* ANSI 색상 코드 */
constexpr char* ANSI_COLOR_RESET = "\x1b[0m";
constexpr char* ANSI_COLOR_RED = "\x1b[31m";
constexpr char* ANSI_COLOR_YELLOW = "\x1b[33m";
constexpr char* ANSI_COLOR_GREEN = "\x1b[32m";
constexpr char* ANSI_COLOR_CYAN = "\x1b[36m";

/* 로그 레벨 */

/* 콘솔 상태 */
static int console_initialized = 0;
static int console_allocated = 0; /* AllocConsole()로 생성되었는지 여부 */


// 공통 로그 함수
static void log_message(LOG_LEVEL level, const char* format, va_list args)
{
    char timestamp[64];
    const char* level_str;
    const char* color;

    if (!console_initialized) 
        return;

    get_timestamp_full(timestamp, sizeof(timestamp));

    // 로그 레벨에 따른 문자열과 색상 설정
    switch (level) 
    {
        case LOG_LEVEL_DEBUG:
            level_str = "DEBUG";
            color = ANSI_COLOR_CYAN;
            break;
        case LOG_LEVEL_INFO:
            level_str = "INFO";
            color = ANSI_COLOR_GREEN;
            break;
        case LOG_LEVEL_WARNING:
            level_str = "WARNING";
            color = ANSI_COLOR_YELLOW;
            break;
        case LOG_LEVEL_ERROR:
            level_str = "ERROR";
            color = ANSI_COLOR_RED;
            break;
        default:
            level_str = "UNKNOWN";
            color = ANSI_COLOR_RESET;
            break;
    }

    // 로그 출력: [타임스탬프] [레벨] 메시지 
    fprintf(stderr, "%s[%s] [%s]%s ", color, timestamp, level_str, ANSI_COLOR_RESET);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

void create_console() 
{
    if (console_initialized)
        return;

#ifdef _WIN32

    // 현재 콘솔창 실행인지 확인
    HWND consoleWindow = GetConsoleWindow();

    if (!consoleWindow) 
    {
        // 없을 경우 GUI앱으로 간주 후 새 콘솔 창 생성
        if (AllocConsole()) 
        {
            console_allocated = 1;
            
            // stdout 재지정 
            freopen("CONOUT$", "w", stdout);

            // stderr 재지정
            freopen("CONOUT$", "w", stderr);

            // stdin 재지정
            freopen("CONIN$", "r", stdin);
        }
    }
    
#endif

    console_initialized = 1;
    fprintf(stderr, "Console initialized\n");
    fflush(stderr);
}

void destroy_console() 
{
    if (!console_initialized)
        return;

    fprintf(stderr, "Console destroyed\n");
    fflush(stderr);

#ifdef _WIN32
    /* AllocConsole()로 생성한 경우에만 해제 */
    if (console_allocated) 
    {
        FreeConsole();
        console_allocated = 0;
    }
#endif

    console_initialized = 0;
}

void log_info(const char* format, va_list args) 
{
    log_message(LOG_LEVEL_INFO, format, args);
}

void log_warning(const char* format, va_list args) 
{
    log_message(LOG_LEVEL_WARNING, format, args);
}

void log_error(const char* format, va_list args) 
{
    log_message(LOG_LEVEL_ERROR, format, args);
}

void log_debug(const char* format, va_list args) 
{
    log_message(LOG_LEVEL_DEBUG, format, args);
}
