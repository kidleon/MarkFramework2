#include "pch.h"
#include "sys_time.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>


void get_timestamp_full(char* buffer, size_t size)
{
    time_t now;
    struct tm* timeinfo;

    time(&now);
    timeinfo = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}