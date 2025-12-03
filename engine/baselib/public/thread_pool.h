#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__



/**
* @brief 스레드 풀 생성
* @param num_threads 스레드 풀에 생성할 스레드 수
* @return 생성된 스레드 풀 핸들, 실패시 NULL
*/
MARK_BASELIB_C_API HANDLE threadpool_create(
	size_t num_threads
);

/**
* @brief 스레드 풀 파괴
* @param pool 파괴할 스레드 풀 핸들
* @return 없음
*/
MARK_BASELIB_C_API void threadpool_destroy(
	HANDLE pool
);

/**
* @brief 스레드 풀에 대기 중인 모든 작업을 취소
* @param pool 작업을 취소할 스레드 풀 핸들
* @return 성공시 0, 실패시 음수 오류 코드
*/
MARK_BASELIB_C_API int threadpool_cancel_all_tasks(
	HANDLE pool
);

/**
* @brief 스레드 풀에 작업 추가 (인자 포함)
* @param pool 작업을 추가할 스레드 풀 핸들
* @param task_func 실행할 작업 함수 포인터
* @param arg 작업 함수에 전달할 인자 포인터
* @return 성공시 0, 실패시 음수 오류 코드
*/
MARK_BASELIB_C_API int threadpool_add_task_arg(
	HANDLE pool,
	void (*task_func)(void*),
	void* arg
);

/**
* @brief 스레드 풀에 작업 추가 (인자 없음)
* @param pool 작업을 추가할 스레드 풀 핸들
* @param task_func 실행할 작업 함수 포인터
* @return 성공시 0, 실패시 음수 오류 코드
*/
MARK_BASELIB_C_API int threadpool_add_task(
	HANDLE pool,
	void (*task_func)(void*)
);

/**
* @brief 스레드 풀이 모든 작업을 완료할 때까지 대기
* @param pool 대기할 스레드 풀 핸들
* @return 없음
*/
MARK_BASELIB_C_API void threadpool_wait(
	HANDLE pool
);

/**
* @brief 스레드 풀이 모든 작업을 완료할 때까지 지정된 시간만큼 대기
* @param pool 대기할 스레드 풀 핸들
* @param milliseconds 대기할 시간 (밀리초 단위)
* @return 작업이 완료되면 TRUE, 타임아웃되면 FALSE
*/
MARK_BASELIB_C_API BOOL threadpool_wait_timeout(
	HANDLE pool,
	uint32 milliseconds
);

/**
* @brief 스레드 풀에 대기 중인 작업 수 조회
* @param pool 조회할 스레드 풀 핸들
* @return 대기 중인 작업 수
*/
MARK_BASELIB_C_API int threadpool_get_pending_count(
	HANDLE pool
);


#endif // __THREAD_POOL_H__
