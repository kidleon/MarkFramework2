#ifndef __PRIVATE_HEAP_H__
#define __PRIVATE_HEAP_H__

#include "MemoryStats.h"


struct MemoryStats;

/**
* @brief 글로벌 힙 할당기 초기화 함수
* @param temp_pool_size 임시 메모리 풀의 크기
* @param temp_pool_threadsafe 임시 메모리 풀이 스레드 세이프인지 여부
* @return 성공시 TRUE, 실패시 FALSE
*/
MARKENGINE_C_API BOOL Heap_Init(
	MEM_SIZE temp_pool_size,
	BOOL temp_pool_threadsafe,
	void (*pfnMemoryReporter)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
	)
);


/**
* @brief 힙 할당기 종료 함수
*/
MARKENGINE_C_API void Heap_Shutdown();

/**
* @brief 현재 메모리 사용 통계를 가져옵니다.
* @param stats 메모리 통계 정보를 받을 포인터
* @return 없음
*/
MARKENGINE_C_API void Heap_GetMemoryStats(
	MemoryStats* stats
);

/**
* @brief 메모리 누수를 보고합니다.
* @return 없음
*/
MARKENGINE_C_API void Heap_ReportMemoryLeaks();

/**
* @brief 시스템 콜을 통해 메모리를 할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.
* @param size 할당할 메모리 크기
* @param alignment 메모리 정렬 크기
* @param file 할당 요청이 발생한 소스 파일 이름
* @param line 할당 요청이 발생한 소스 파일의 라인 번호
* @param func 할당 요청이 발생한 함수 이름
* @return 할당된 메모리의 포인터, 실패시 nullptr
*/
#if defined(USE_PROFILE_MEMORY)
MARKENGINE_C_API void* SysHeapAllocAlign(
	MEM_SIZE size,
	size_t alignment,
	const char* file,
	int line,
	const char* func
);

#else
MARKENGINE_C_API void* SysHeapAllocAlign(
	MEM_SIZE size,
	size_t alignment
);

#endif // USE_PROFILE_MEMORY

/**
* @brief 시스템 콜을 통해 메모리를 할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.
* @param size 할당할 메모리 크기
* @param file 할당 요청이 발생한 소스 파일 이름
* @param line 할당 요청이 발생한 소스 파일의 라인 번호
* @param func 할당 요청이 발생한 함수 이름
* @return 할당된 메모리의 포인터, 실패시 nullptr
*/
#if defined(USE_PROFILE_MEMORY)
MARKENGINE_C_API void* SysHeapAlloc(
	MEM_SIZE size,
	const char* file,
	int line,
	const char* func
);
#else
MARKENGINE_C_API void* SysHeapAlloc(
	MEM_SIZE size
);

#endif // USE_PROFILE_MEMORY

/**
* @brief 시스템 콜을 통해 메모리를 재할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.ㄴ
* @param ptr 재할당할 메모리의 포인터
* @param size 재할당할 메모리 크기
* @param file 재할당 요청이 발생한 소스 파일 이름
* @param line 재할당 요청이 발생한 소스 파일의 라인 번호
* @param func 재할당 요청이 발생한 함수 이름
* @return 재할당된 메모리의 포인터, 실패시 nullptr
*/
#if defined(USE_PROFILE_MEMORY)
MARKENGINE_C_API void* SysHeapRealloc(
	void* ptr,
	MEM_SIZE size,
	const char* file,
	int line,
	const char* func
);
#else
MARKENGINE_C_API void* SysHeapRealloc(
	void* ptr,
	MEM_SIZE size
);

#endif // USE_PROFILE_MEMORY

/**
* @brief 시스템 콜을 통해 할당된 메모리를 해제합니다.
* @param ptr 해제할 메모리의 포인터
* @return 없음
*/
MARKENGINE_C_API void SysHeapFreeAlign(void* ptr);

/**
* @brief 시스템 콜을 통해 할당된 메모리를 해제합니다.
* @param ptr 해제할 메모리의 포인터
* @return 없음
*/
MARKENGINE_C_API void SysHeapFree(void* ptr);

/**
* @brief 고정 크기 메모리 풀에서 메모리를 할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.
* @param size 할당할 메모리 크기
* @return 할당된 메모리의 포인터, 실패시 nullptr
*/
#if defined(USE_PROFILE_MEMORY)
MARKENGINE_C_API void* PoolAlloc(
	MEM_SIZE size,
	const char* file,
	int line,
	const char* func
);
#else
MARKENGINE_C_API void* PoolAlloc(
	MEM_SIZE size
);
#endif  // USE_PROFILE_MEMORY

/**
* @brief 고정 크기 메모리 풀에서 할당된 메모리를 해제합니다.
* @param ptr 해제할 메모리의 포인터
* @return 없음
*/
MARKENGINE_C_API void PoolFree(void* ptr);

/**
* @brief 임시 메모리를 할당합니다. 임시 메모리는 ResetTemp 함수를 호출하여 한 번에 해제할 수 있습니다.
* @param size 할당할 메모리 크기
* @return 할당된 메모리의 포인터, 실패시 nullptr
*/
MARKENGINE_C_API void* TempAlloc(MEM_SIZE size);

/**
* @brief 임시 메모리를 한 번에 해제합니다.
* @return 없음
*/
MARKENGINE_C_API void TempReset();


#if defined(USE_PROFILE_MEMORY)
#	define MARK_SYS_ALLOC(size) SysHeapAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_SYS_FREE(ptr) SysHeapFree(ptr)
#	define MARK_SYS_REALLOC(ptr, size) SysHeapRealloc(ptr, size, __FILE__, __LINE__, __FUNCTION__)

#	define MARK_SYS_ALLOC_ALGIN(size, alignment) SysHeapAllocAlign(size, alignment, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_SYS_FREE_ALIGN(ptr) SysHeapFreeAlign(ptr)

#	define MARK_POOL_ALLOC(size) PoolAlloc(size, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_POOL_FREE(ptr) PoolFree(ptr)

#	define MARK_TEMP_ALLOC(size) TempAlloc(size)
#	define MARK_TEMP_RESET() TempReset()

#	define MARK_NEW(type) new (MARK_SYS_ALLOC(sizeof(type))) type
#	define MARK_NEW_ARRAY(type, count) new (MARK_SYS_ALLOC(sizeof(type) * (count))) type[count]
#	define MARK_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); MARK_SYS_FREE(p); p = nullptr; } }

#	define MARK_POOL_NEW(type) new (MARK_POOL_ALLOC(sizeof(type))) type
#	define MARK_POOL_NEW_ARRAY(type, count) new (MARK_POOL_ALLOC(sizeof(type) * (count))) type[count]
#	define MARK_POOL_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); MARK_POOL_FREE(p); p = nullptr; } }
#else
#	define MARK_SYS_ALLOC(size) SysHeapAlloc(size)
#	define MARK_SYS_ALLOC(size) SysHeapAlloc(size)
#	define MARK_SYS_FREE(ptr) SysHeapFree(ptr)
#	define MARK_SYS_REALLOC(ptr, size) SysHeapRealloc(ptr, size)

#	define MARK_SYS_ALLOC_ALGIN(size, alignment) SysHeapAllocAlign(size, alignment)
#	define MARK_SYS_FREE_ALIGN(ptr) SysHeapFreeAlign(ptr)

#	define MARK_POOL_ALLOC(size) PoolAlloc(size)
#	define MARK_POOL_FREE(ptr) PoolFree(ptr)

#	define MARK_TEMP_ALLOC(size) TempAlloc(size)
#	define MARK_TEMP_RESET() TempReset()

#	define MARK_NEW(type) new (MARK_SYS_ALLOC(sizeof(type))) type
#	define MARK_NEW_ARRAY(type, count) new (MARK_SYS_ALLOC(sizeof(type) * (count))) type[count]
#	define MARK_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); MARK_SYS_FREE(p); p = nullptr; } }

#	define MARK_POOL_NEW(type) new (MARK_POOL_ALLOC(sizeof(type))) type
#	define MARK_POOL_NEW_ARRAY(type, count) new (MARK_POOL_ALLOC(sizeof(type) * (count))) type[count]
#	define MARK_POOL_DELETE(ptr, type) {type* p = ptr; if(p) { (p)->~type(); MARK_POOL_FREE(p); p = nullptr; } }
#endif // USE_PROFILE_MEMORY

#endif // __PRIVATE_HEAP_H__
