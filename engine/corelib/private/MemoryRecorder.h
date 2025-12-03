#ifndef __MEMORY_RECORDER_H__
#define __MEMORY_RECORDER_H__


struct HASH_TABLE;

/**
* @brief 메모리 할당 기록기 클래스
* @details 시스템 콜 및 풀 메모리 할당을 기록하고 메모리 누수를 보고하는 기능을 제공합니다.
*/
class MemoryRecorder
{
	typedef void (*pfnReportMemoryLeak)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
	);

public:
	MemoryRecorder();
	~MemoryRecorder();

	// 초기화
	BOOL Initialize(pfnReportMemoryLeak leak_reporter);
		
	// 정리
	void Cleanup();

	// 시스템 콜 할당 기록
	void OnAlloc_Syscall(
		void* ptr, 
		size_t size,
		const char* file,
		int line,
		const char* func
	);

	// 시스템 콜 해제 기록
	void OnFree_Syscall(void* ptr);

	// 풀 할당 기록
	void OnAlloc_Pool(
		void* ptr,
		size_t size,
		const char* file,
		int line,
		const char* func
	);

	// 풀 해제 기록
	void OnFree_Pool(void* ptr);

	// 메모리 누수 보고
	void ReportMemoryLeaks() noexcept;

	MEM_SIZE GetAllocCount_Syscall() noexcept; // 시스템 콜 할당 카운트 조회
	MEM_SIZE GetAllocCount_Pool() noexcept; // 풀 할당 카운트 조회

	MEM_SIZE GetAllocSize_Syscall() noexcept; // 시스템 콜 할당 사이즈 조회
	MEM_SIZE GetAllocSize_Pool() noexcept; // 풀 할당 사이즈 조회

	// 할당 통계 조회 (LOCK 한번에 조회한다)
	void GetAllocStats(
		MEM_SIZE* pSyscallCount,
		MEM_SIZE* pPoolCount,
		MEM_SIZE* pSyscallSize,
		MEM_SIZE* pPoolSize
	) noexcept;

private:
	HANDLE m_hPool; // 메모리 블록 풀 핸들

	HASH_TABLE* m_pMemTable_SysAlloc; // 시스템 콜 할당 해시 테이블
	HASH_TABLE* m_pMemTable_PoolAlloc; // 풀 할당 해시 테이블

	spin_lock_t m_SysLock; // 시스템 콜 할당 잠금
	spin_lock_t m_PoolLock; // 풀 할당 잠금

	pfnReportMemoryLeak m_pfnReportMemoryLeak; // 메모리 누수 보고 함수 포인터

	MEM_SIZE m_AllocCount_Syscall; // 시스템 콜 할당 카운트
	MEM_SIZE m_AllocCount_Pool; // 풀 할당 카운트
	MEM_SIZE m_AllocSize_Syscall; // 시스템 콜 할당 사이즈
	MEM_SIZE m_AllocSize_Pool; // 풀 할당 사이즈

};



#endif // __MEMORY_RECORDER_H__
