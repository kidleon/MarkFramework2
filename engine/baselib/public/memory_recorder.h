#ifndef __MEMORY_RECORDER_H__
#define __MEMORY_RECORDER_H__


/**
* @brief 메모리 할당 기록기 초기화 함수
* @param pfnMemoryReporter 메모리 누수 보고 콜백 함수
* @return 메모리 할당 기록기 핸들
*/
MARK_BASELIB_C_API HANDLE memrec_init(
	void (*pfnMemoryReporter)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
	)
);

/**
* @brief 메모리 할당 기록기 종료 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @return 없음
*/
MARK_BASELIB_C_API void memrec_shutdown(HANDLE hRecorder);

/**
* @brief 메모리 누수 보고 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @return 없음
*/
MARK_BASELIB_C_API void memrec_report_leaks(HANDLE hRecorder);

/**
* @brief 시스템 콜 메모리 할당 기록 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @param ptr 할당된 메모리 포인터
* @param size 할당된 메모리 크기
* @param file 할당이 발생한 소스 파일 이름
* @param line 할당이 발생한 소스 파일의 라인 번호
* @param func 할당이 발생한 함수 이름
* @return 없음
*/
MARK_BASELIB_C_API void memrec_onalloc_syscall(
	HANDLE hRecorder,
	void* ptr,
	size_t size,
	const char* file,
	int line,
	const char* func
);

/**
* @brief 시스템 콜 메모리 해제 기록 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @param ptr 해제된 메모리 포인터
* @return 없음
*/
MARK_BASELIB_C_API void memrec_onfree_syscall(
	HANDLE hRecorder,
	void* ptr
);

/**
* @brief 풀 메모리 할당 기록 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @param ptr 할당된 메모리 포인터
* @param size 할당된 메모리 크기
* @param file 할당이 발생한 소스 파일 이름
* @param line 할당이 발생한 소스 파일의 라인 번호
* @param func 할당이 발생한 함수 이름
* @return 없음
*/
MARK_BASELIB_C_API void memrec_onalloc_pool(
	HANDLE hRecorder,
	void* ptr,
	size_t size,
	const char* file,
	int line,
	const char* func
);

/**
* @brief 풀 메모리 해제 기록 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @param ptr 해제된 메모리 포인터
* @return 없음
*/
MARK_BASELIB_C_API void memrec_onfree_pool(
	HANDLE hRecorder,
	void* ptr
);

/**
* @brief 시스템 콜 메모리 할당 카운트 조회 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @return 시스템 콜 메모리 할당 카운트
*/
MARK_BASELIB_C_API size_t memrec_getcount_syscall(HANDLE hRecorder);

/**
* @brief 풀 메모리 할당 카운트 조회 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @return 풀 메모리 할당 카운트
*/
MARK_BASELIB_C_API size_t memrec_getcount_pool(HANDLE hRecorder);

/**
* @brief 시스템 콜 메모리 할당 크기 조회 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @return 시스템 콜 메모리 할당 크기
*/
MARK_BASELIB_C_API size_t memrec_getsize_syscall(HANDLE hRecorder);

/**
* @brief 풀 메모리 할당 크기 조회 함수
* @param hRecorder 메모리 할당 기록기 핸들
* @return 풀 메모리 할당 크기
*/
MARK_BASELIB_C_API size_t memrec_getsize_pool(HANDLE hRecorder);


#endif // __MEMORY_RECORDER_H__
