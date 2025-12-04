#ifndef __THREAD_H__
#define __THREAD_H__


/**
* @brief 스레드 생성
* @param num_threads 스레드 풀에 생성할 스레드 수
* @return 생성된 스레드 풀 핸들, 실패시 NULL
*/
MARK_BASELIB_C_API HANDLE thread_create(
	void (*thread_func)(void*),
	void* arg
);

/**
* @brief 스레드 파괴
* @param handle 파괴할 스레드 풀 핸들
* @return 없음
*/
MARK_BASELIB_C_API void thread_destroy(
	HANDLE handle
);

/**
* @brief 스레드 실행 신호
* @param handle 실행할 스레드 핸들
* @return 없음
*/
MARK_BASELIB_C_API void thread_execute(
	HANDLE handle
);

/**
* @brief 스레드 대기
* @param handle 대기할 스레드 핸들
* @return 없음
*/
MARK_BASELIB_C_API void thread_wait(
	HANDLE handle
);


#endif // __THREAD_H__
