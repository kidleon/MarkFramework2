#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

#include "MemoryStats.h"


namespace mark
{
	struct MemoryStats;

	class MARKENGINE_API MemoryManager
	{
	public:
		/**
		* @brief 메모리 매니저를 초기화합니다.
		* @param temp_pool_size 임시 메모리 풀의 크기
		* @param temp_pool_threadsafe 임시 메모리 풀이 스레드 세이프인지 여부
		* @return 성공시 TRUE, 실패시 FALSE
		*/
		static BOOL Initialize(
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
		* @brief 메모리 매니저를 종료합니다.
		* @return 없음
		*/
		static void Shutdown();

		/**
		* @brief 시스템 콜을 통해 메모리를 할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.
		* @param size 할당할 메모리 크기
		* @param alignment 메모리 정렬 크기
		* @param file 할당 요청이 발생한 소스 파일 이름
		* @param line 할당 요청이 발생한 소스 파일의 라인 번호
		* @param func 할당 요청이 발생한 함수 이름
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* AllocAlign_SysCall(
			MEM_SIZE size,
			size_t alignment,
			const char* file,
			int line,
			const char* func
		);

		/**
		* @brief 시스템 콜을 통해 메모리를 할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.
		* @param size 할당할 메모리 크기
		* @param file 할당 요청이 발생한 소스 파일 이름
		* @param line 할당 요청이 발생한 소스 파일의 라인 번호
		* @param func 할당 요청이 발생한 함수 이름
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* Alloc_SysCall(
			MEM_SIZE size,
			const char* file,
			int line,
			const char* func
		);

		/**
		* @brief 시스템 콜을 통해 메모리를 할당합니다. 메모리 프로파일링을 수행하지 않습니다.
		* @param size 할당할 메모리 크기
		* @param alignment 메모리 정렬 크기
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* AllocAlign_SysCall(
			MEM_SIZE size,
			size_t alignment
		);

		/**
		* @brief 시스템 콜을 통해 메모리를 할당합니다. 메모리 프로파일링을 수행하지 않습니다.
		* @param size 할당할 메모리 크기
		* @param alignment 메모리 정렬 크기
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* Alloc_SysCall(
			MEM_SIZE size
		);

		/**
		* @brief 시스템 콜을 통해 메모리를 재할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.ㄴ
		* @param ptr 재할당할 메모리의 포인터
		* @param size 재할당할 메모리 크기
		* @param file 재할당 요청이 발생한 소스 파일 이름
		* @param line 재할당 요청이 발생한 소스 파일의 라인 번호
		* @param func 재할당 요청이 발생한 함수 이름
		* @return 재할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* Realloc_Syscall(
			void* ptr,
			MEM_SIZE size,
			const char* file,
			int line,
			const char* func
		);

		/**
		* @brief 시스템 콜을 통해 메모리를 재할당합니다. 메모리 프로파일링을 수행하지 않습니다.
		* @param ptr 재할당할 메모리의 포인터
		* @param size 재할당할 메모리 크기
		* @return 재할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* Realloc_Syscall(
			void* ptr,
			MEM_SIZE size
		);

		/**
		* @brief 시스템 콜을 통해 할당된 메모리를 해제합니다.
		* @param ptr 해제할 메모리의 포인터
		* @return 없음
		*/
		static void FreeAlign_SysCall(void* ptr);

		/**
		* @brief 시스템 콜을 통해 할당된 메모리를 해제합니다.
		* @param ptr 해제할 메모리의 포인터
		* @return 없음
		*/
		static void Free_SysCall(void* ptr);

		/**
		* @brief 고정 크기 메모리 풀에서 메모리를 할당합니다. USE_MEMORY_PROFILE가 정의된 경우 메모리 프로파일링을 수행합니다.
		* @param size 할당할 메모리 크기
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* AllocPool(
			MEM_SIZE size,
			const char* file,
			int line,
			const char* func
		);

		/**
		* @brief 고정 크기 메모리 풀에서 메모리를 할당합니다. 메모리 프로파일링을 수행하지 않습니다.
		* @param size 할당할 메모리 크기
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* AllocPool(
			MEM_SIZE size
		);

		/**
		* @brief 고정 크기 메모리 풀에서 할당된 메모리를 해제합니다.
		* @param ptr 해제할 메모리의 포인터
		* @return 없음
		*/
		static void FreePool(void* ptr);

		/**
		* @brief 임시 메모리를 할당합니다. 임시 메모리는 ResetTemp 함수를 호출하여 한 번에 해제할 수 있습니다. 
		* @param size 할당할 메모리 크기
		* @return 할당된 메모리의 포인터, 실패시 nullptr
		*/
		static void* AllocTemp(
			MEM_SIZE size
		);

		/**
		* @brief 임시 메모리를 한 번에 해제합니다.
		* @return 없음
		*/
		static void ResetTemp();

		/**
		* @brief 현재 메모리 사용 통계를 가져옵니다.
		* @param stats 메모리 통계 정보를 받을 포인터
		* @return 없음
		*/
		static void GetMemoryStats(
			MemoryStats* stats
		);

		/**
		* @brief 메모리 누수를 보고합니다.
		* @return 없음
		*/
		static void ReportMemoryLeaks();

	};
}

#if defined(USE_PROFILE_MEMORY)
#	define MARK_ALLOC_ALIGN_SYSCALL(size, alignment) mark::MemoryManager::AllocAlign_SysCall(size, alignment, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_FREE_ALIGN_SYSCALL(ptr) mark::MemoryManager::FreeAlign_SysCall(ptr)

#	define MARK_ALLOC_SYSCALL(size) mark::MemoryManager::Alloc_SysCall(size, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_REALLOC_SYSCALL(ptr, size) mark::MemoryManager::Realloc_Syscall(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_FREE_SYSCALL(ptr) mark::MemoryManager::Free_SysCall(ptr)

#	define MARK_ALLOC_POOL(size) mark::MemoryManager::AllocPool(size, __FILE__, __LINE__, __FUNCTION__)
#	define MARK_FREE_POOL(ptr) mark::MemoryManager::FreePool(ptr)

#	define MARK_ALLOC_TEMP(size) mark::MemoryManager::AllocTemp(size)
#	define MARK_RESET_TEMP() mark::MemoryManager::ResetTemp()

#	define MARK_NEW(type) new (MARK_ALLOC_POOL(sizeof(type))) type
#	define MARK_NEW_ARRAY(type, count) new (MARK_ALLOC_POOL(sizeof(type) * (count))) type[count]
#	define MARK_DELETE(ptr, type) do { if(ptr) { (ptr)->~type(); MARK_FREE_POOL(ptr); ptr = nullptr; } } while(0)
#else
#	define MARK_ALLOC_ALIGN_SYSCALL(size, alignment) mark::MemoryManager::AllocAlign_SysCall(size, alignment)
#	define MARK_FREE_ALIGN_SYSCALL(ptr) mark::MemoryManager::FreeAlign_SysCall(ptr)

#	define MARK_ALLOC_SYSCALL(size) mark::MemoryManager::Alloc_SysCall(size)
#	define MARK_REALLOC_SYSCALL(ptr, size) mark::MemoryManager::Realloc_Syscall(ptr, size)
#	define MARK_FREE_SYSCALL(ptr) mark::MemoryManager::Free_SysCall(ptr)

#	define MARK_ALLOC_POOL(size) mark::MemoryManager::AllocPool(size)
#	define MARK_FREE_POOL(ptr) mark::MemoryManager::FreePool(ptr)

#	define MARK_ALLOC_TEMP(size) mark::MemoryManager::AllocTemp(size)
#	define MARK_RESET_TEMP() mark::MemoryManager::ResetTemp()

#	define MARK_NEW(type) new (MARK_ALLOC_POOL(sizeof(type))) type
#	define MARK_NEW_ARRAY(type, count) new (MARK_ALLOC_POOL(sizeof(type) * (count))) type[count]
#	define MARK_DELETE(ptr, type) do { if(ptr) { (ptr)->~type(); MARK_FREE_POOL(ptr); ptr = nullptr; } } while(0)
#endif // USE_PROFILE_MEMORY

#endif // __MEMORY_MANAGER_H__
