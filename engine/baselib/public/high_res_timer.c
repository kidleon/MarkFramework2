#include "pch.h"
#include "high_res_timer.h"
#include "crt_memory.h"


struct high_res_timer_t
{
	double created_time_ms;
	double elapsed_time_ms;
	double stopped_time_ms;

#if defined(__TARGET_OS_WINDOWS)
	LARGE_INTEGER frequency; // Ticks per second
	LARGE_INTEGER created_count;
#else 
	timeval CreateCount;
#endif // __TARGET_OS_WINDOWS

	BOOL stopped;
	int PADDING;
};


HANDLE highrestimer_create()
{
	struct high_res_timer_t* timer = (struct high_res_timer_t*)crt_malloc(sizeof(struct high_res_timer_t));

	memset(timer, 0, sizeof(struct high_res_timer_t));

#if defined(__TARGET_OS_WINDOWS)
	QueryPerformanceFrequency(&timer->frequency);
	QueryPerformanceCounter(&timer->created_count);
	timer->created_time_ms = (DOUBLE)(timer->created_count.QuadPart) / (DOUBLE)(timer->frequency.QuadPart);
	timer->elapsed_time_ms = timer->created_time_ms;
#else
	timeval curr_count;
	gettimeofday(&curr_count, NULL);
	timer->created_time_ms = (curr_count.tv_sec * 1000000.0) + curr_count.tv_usec;
	timer->elapsed_time_ms = timer->created_time_ms;
#endif // __TARGET_OS_WINDOWS

	return (HANDLE)timer;
}

void highrestimer_destroy(HANDLE handle)
{
	if (!handle) return;
	crt_free(handle);
}

void highrestimer_stop(HANDLE handle)
{
	if (!handle) return;

	struct high_res_timer_t* timer = (struct high_res_timer_t*)handle;

	if (!timer->stopped)
	{
		timer->stopped_time_ms = highrestimer_gettime(timer);
		timer->stopped = TRUE;
	}
}

void highrestimer_resume(HANDLE handle)
{
	if (!handle) return;

	struct high_res_timer_t* timer = (struct high_res_timer_t*)handle;

	timer->stopped = FALSE;
}

void highrestimer_reset(HANDLE handle)
{
	if (!handle) return;

	struct high_res_timer_t* timer = (struct high_res_timer_t*)handle;

#if defined(__TARGET_OS_WINDOWS)
	QueryPerformanceFrequency(&timer->frequency);
	QueryPerformanceCounter(&timer->created_count);
	timer->created_time_ms = (DOUBLE)(timer->created_count.QuadPart) / (DOUBLE)(timer->frequency.QuadPart);
	timer->elapsed_time_ms = timer->created_time_ms;
#else
	timeval CurrentCount;
	gettimeofday(&CurrentCount, NULL);
	timer->created_time_ms = (CurrentCount.tv_sec * 1000000.0) + CurrentCount.tv_usec;
	timer->elapsed_time_ms = timer->created_time_ms;
#endif // __TARGET_OS_WINDOWS 
}

DOUBLE highrestimer_gettime(HANDLE handle)
{
	if (!handle) return 0;

	struct high_res_timer_t* timer = (struct high_res_timer_t*)handle;

	if (timer->stopped)
		return timer->stopped_time_ms;

#if defined(__TARGET_OS_WINDOWS)
	LARGE_INTEGER CurrentCount;
	QueryPerformanceCounter(&CurrentCount);
	DOUBLE currentTimeInMicroSecond = (DOUBLE)CurrentCount.QuadPart / (DOUBLE)(timer->frequency.QuadPart);
	return currentTimeInMicroSecond - timer->created_time_ms;
#else
	timeval CurrentCount;
	gettimeofday(&CurrentCount, NULL);
	double currentTimeInMicroSecond = (CurrentCount.tv_sec * 1000000.0) + CurrentCount.tv_usec;
	return (currentTimeInMicroSecond - timer->created_time_ms);
#endif // __TARGET_OS_WINDOWS
}

DOUBLE highrestimer_getelapsedtime(HANDLE handle)
{
	if (!handle) return 0;

	struct high_res_timer_t* timer = (struct high_res_timer_t*)handle;

	if (timer->stopped)
		return 0.0;

#if defined(__TARGET_OS_WINDOWS)
	LARGE_INTEGER CurrentCount;
	QueryPerformanceCounter(&CurrentCount);
	DOUBLE CurrentTimeInMicroSecond = (DOUBLE)CurrentCount.QuadPart / (DOUBLE)(timer->frequency.QuadPart);
	DOUBLE ElapsedTime = (CurrentTimeInMicroSecond - timer->elapsed_time_ms);
	timer->elapsed_time_ms = CurrentTimeInMicroSecond;
	return ElapsedTime;
#else
	timeval CurrentCount;
	gettimeofday(&CurrentCount, NULL);
	DOUBLE CurrentTimeInMicroSecond = (CurrentCount.tv_sec * 1000000.0) + CurrentCount.tv_usec;
	DOUBLE ElapsedTime = (CurrentTimeInMicroSecond - timer->elapsed_time_ms) * 0.001;
	timer->elapsed_time_ms = CurrentTimeInMicroSecond;
#endif // __TARGET_OS_WINDOWS
}