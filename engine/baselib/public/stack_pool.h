#ifndef __STACK_POOL_H__
#define __STACK_POOL_H__


typedef void* STACK_MARKER;

/**
* @brief 스택 풀 생성
* @param pool_size 스택 풀의 전체 크기
* @param thread_safe 스레드 세이프 모드 여부
* @return 생성된 스택 풀 핸들, 실패시 NULL
*/
MARK_BASELIB_C_API HANDLE stackpool_create(
	MEM_SIZE pool_size,
	BOOL thread_safe
);

/**
* @brief 스택 풀 파괴
* @param handle 파괴할 스택 풀 핸들
* @return 없음
*/
MARK_BASELIB_C_API void stackpool_destroy(
	HANDLE handle
);

/**
* @brief 현재 스택 포인터 위치를 마커로 반환, Thread-Safe 모드일 경우 락을 획득한다. 
* 반드시 release_marker와 쌍으로 사용해야 함, ThreadSafe 모드일 경우 퍼포먼스 저하 유의
* @param handle 스택 풀 핸들
* @return 현재 스택 포인터 위치 마커
*/
MARK_BASELIB_C_API STACK_MARKER stackpool_getmarker(
	HANDLE handle
);

/**
* @brief 지정된 마커 위치로 스택 포인터를 되돌림, Thread-Safe 모드일 경우 락을 해제한다.
* @param handle 스택 풀 핸들
* @param marker 되돌릴 마커 위치
* @return 없음
*/
MARK_BASELIB_C_API void stackpool_releasemarker(
	HANDLE handle,
	STACK_MARKER marker
);

/**
* @brief 스택 풀에서 메모리 할당
* @param handle 스택 풀 핸들
* @param size 할당할 메모리 크기
* @return 할당된 메모리 포인터, 실패시 NULL
*/
MARK_BASELIB_C_API void* stackpool_alloc(
	HANDLE handle,
	MEM_SIZE size
);

/**
* @brief 스택 풀에서 사용 중인 메모리 크기 반환
* @param handle 스택 풀 핸들
* @return 사용 중인 메모리 크기
*/
MARK_BASELIB_C_API MEM_SIZE stackpool_get_used(
	HANDLE handle
);

/**
* @brief 스택 풀의 전체 용량 반환
* @param handle 스택 풀 핸들
* @return 스택 풀의 전체 용량
*/
MARK_BASELIB_C_API MEM_SIZE stackpool_get_capacity(
	HANDLE handle
);

/**
* @brief 스택 풀 초기화 (모든 할당 해제)
* @param handle 스택 풀 핸들
* @return 없음
*/
MARK_BASELIB_C_API void stackpool_reset(
	HANDLE handle
);

#endif // __STACK_POOL_H__