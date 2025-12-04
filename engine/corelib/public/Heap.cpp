#include "pch.h"
#include "Heap.h"
#include "crt_memory.h"
#include "temp_pool.h"
#include "paged_object_pool.h"
#include "MemoryRecorder.h"
#include "MemoryStats.h"


enum HEAP_SIZE_IDX
{
	HEAP_64B = 0,
	HEAP_128B,
	HEAP_256B,
	HEAP_512B,

	HEAP_1KB,
	HEAP_2KB,
	HEAP_4KB,
	HEAP_8KB,

	HEAP_16KB,
	HEAP_32KB,
	HEAP_64KB,
	HEAP_128KB,

	HEAP_256KB,
	HEAP_512KB,
	HEAP_1MB,
	HEAP_2MB,

	MAX_HEAP_SIZE_IDX
};

MEM_SIZE HEAP_SIZE_TABLE[MAX_HEAP_SIZE_IDX] =
{
	64,			// HEAP_64B
	128,		// HEAP_128B
	256,		// HEAP_256B
	512,		// HEAP_512B

	1024,		// HEAP_1KB
	2048,		// HEAP_2KB
	4096,		// HEAP_4KB
	8192,		// HEAP_8KB

	16384,		// HEAP_16KB
	32768,		// HEAP_32KB
	65536,		// HEAP_64KB
	131072,		// HEAP_128KB

	262144,		// HEAP_256KB
	524288,		// HEAP_512KB
	1048576,	// HEAP_1MB
	2097152,	// HEAP_2MB
};

MEM_SIZE HEAP_BLOCK_COUNT_TABLE[MAX_HEAP_SIZE_IDX] = 
{
	1024,		// HEAP_64B
	1024,		// HEAP_128B
	1024,		// HEAP_256B
	1024,		// HEAP_512B

	512,		// HEAP_1KB
	512,		// HEAP_2KB
	256,		// HEAP_4KB
	256,		// HEAP_8KB

	256,		// HEAP_16KB
	256,		// HEAP_32KB
	128,		// HEAP_64KB
	128,		// HEAP_128KB

	64,			// HEAP_256KB
	64,			// HEAP_512KB
	16,			// HEAP_1MB
	8,			// HEAP_2MB
};

static HANDLE g_hPoolHeaps[MAX_HEAP_SIZE_IDX] = { nullptr };
static HANDLE g_hTempHeap = nullptr;
static MemoryRecorder* g_pMemoryRecorder = nullptr;

#if defined(USE_PROFILE_MEMORY)
static spin_lock_t g_TempHeapLock = { 0 };
static MEM_SIZE g_PeakAllocCount_Temp = 0; // 임시 메모리 풀 최대 할당 카운트
static MEM_SIZE g_PeakAllocSize_Temp = 0; // 임시 메모리 풀 최대 할당 사이즈
static MEM_SIZE g_UsedAllocCount_Temp = 0; // 임시 메모리 풀 현재 할당 카운트
static MEM_SIZE g_UsedAllocSize_Temp = 0; // 임시 메모리 풀 현재 할당 사이즈
#endif // USE_PROFILE_MEMORY

#if defined(__TARGET_COMPILER_GCC) || defined(__TARGET_COMPILER_CLANG)
static int get_heap_size_index(MEM_SIZE size)
{
	if (size <= 64) return 0;

	// 다음 2의 거듭제곱으로 올림
	size_t next_pow2 = 1ULL << (64 - __builtin_clzll(size - 1));

	// log2(next_pow2) - 6 (64 = 2^6이 첫 번째 인덱스)
	int index = __builtin_ctzll(next_pow2) - 6;

	return (index < MAX_HEAP_SIZE_IDX) ? index : MAX_HEAP_SIZE_IDX - 1;
}
#elif defined(__TARGET_COMPILER_MSC)
static int get_heap_size_index(MEM_SIZE size)
{
	if (size <= 64) return 0;

	unsigned long index_bit;

#	ifdef _WIN64
	// 64-bit Windows
	if(!_BitScanReverse64(&index_bit, size - 1))
		return -1;
#	else
	// 32-bit Windows
	if(!_BitScanReverse(&index_bit, (unsigned long)(size - 1)))
		return -1;
#	endif

	// index_bit는 최상위 비트 위치 (log2(size-1)과 동일)
	// 다음 2의 거듭제곱으로 올림 후 인덱스 계산
	int index = index_bit + 1 - 6;  // 64 = 2^6이 첫 인덱스

	return (index < MAX_HEAP_SIZE_IDX) ? index : MAX_HEAP_SIZE_IDX - 1;
}
#else
static int get_heap_size_index(MEM_SIZE size)
{
	for (int i = 0; i < MAX_HEAP_SIZE_IDX; ++i)
	{
		if (size <= HEAP_SIZE_TABLE[i])
			return i;
	}
	return -1;
}
#endif // Compiler check

BOOL Heap_Init(
	MEM_SIZE temp_pool_size,
	BOOL temp_pool_threadsafe,
	void (*pfnMemoryReporter)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
	)
)
{
#if defined(USE_PROFILE_MEMORY)
	if (!g_pMemoryRecorder)
	{
		g_pMemoryRecorder = new MemoryRecorder();
		g_pMemoryRecorder->Initialize(pfnMemoryReporter);
	}
#endif // USE_PROFILE_MEMORY

	if (!g_hTempHeap)
	{
		g_hTempHeap = create_temp_pool(
			temp_pool_size,
			temp_pool_threadsafe
		);

		if (!g_hTempHeap)
			return FALSE;
	}

	return TRUE;
}

void Heap_Shutdown()
{
	for (int i = 0; i < MAX_HEAP_SIZE_IDX; ++i)
	{
		if (g_hPoolHeaps[i])
		{
			destroy_paged_object_pool(g_hPoolHeaps[i]);
			g_hPoolHeaps[i] = nullptr;
		}
	}

	if (g_hTempHeap)
	{
		destroy_temp_pool(g_hTempHeap);
		g_hTempHeap = nullptr;
	}

	CHECK_DELETE(g_pMemoryRecorder);
}

void Heap_GetMemoryStats(
	MemoryStats* stats
)
{
	if (!stats) return;

#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->GetAllocStats(
			&stats->sys_alloc_count,
			&stats->pool_alloc_count,
			&stats->sys_alloc_size,
			&stats->pool_alloc_size
		);
	}

	acquire_spin_lock(&g_TempHeapLock);
	stats->peak_temp_count = g_PeakAllocCount_Temp;
	stats->peak_temp_size = g_PeakAllocSize_Temp;
	release_spin_lock(&g_TempHeapLock);

#else
	stats->sys_alloc_count = 0;
	stats->pool_alloc_count = 0;
	stats->sys_alloc_size = 0;
	stats->pool_alloc_size = 0;
	stats->peak_temp_count = 0;
	stats->peak_temp_size = 0;
#endif // USE_PROFILE_MEMORY
}

void Heap_ReportMemoryLeaks()
{
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->ReportMemoryLeaks();
	}
}

#if defined(USE_PROFILE_MEMORY)
void* HeapAllocAlign(
	MEM_SIZE size,
	size_t alignment,
	const char* file,
	int line,
	const char* func
)
#else
void* HeapAllocAlign(
	MEM_SIZE size,
	size_t alignment
)
#endif // USE_PROFILE_MEMORY
{
	void* pHeap = crt_malloc_align(size, alignment);
	if (!pHeap)
		return nullptr;

#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->OnAlloc_Syscall(
			pHeap,
			size,
			file,
			line,
			func
		);
	}
#endif // USE_PROFILE_MEMORY

	return pHeap;
}

#if defined(USE_PROFILE_MEMORY)
void* HeapAlloc2(
	MEM_SIZE size,
	const char* file,
	int line,
	const char* func
)
#else
void* HeapAlloc_(
	MEM_SIZE size
)
#endif // USE_PROFILE_MEMORY
{
	void* pHeap = crt_malloc(size);
	if (!pHeap)
		return nullptr;

#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->OnAlloc_Syscall(
			pHeap,
			size,
			file,
			line,
			func
		);
	}
#endif // USE_PROFILE_MEMORY

	return pHeap;
}

#if defined(USE_PROFILE_MEMORY)
void* HeapRealloc(
	void* ptr,
	MEM_SIZE size,
	const char* file,
	int line,
	const char* func
)
#else
void* HeapRealloc(
	void* ptr,
	MEM_SIZE size
)
#endif // USE_PROFILE_MEMORY
{
#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
		g_pMemoryRecorder->OnFree_Syscall(ptr);
#endif // USE_PROFILE_MEMORY

	void* pHeap = crt_realloc(ptr, size);

	if (!pHeap)
		return nullptr;

#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->OnAlloc_Syscall(
			pHeap,
			size,
			file,
			line,
			func
		);
	}
#endif // USE_PROFILE_MEMORY

	return pHeap;
}

void HeapFreeAlign(void* ptr)
{
#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->OnFree_Syscall(ptr);
	}
#endif // USE_PROFILE_MEMORY

	crt_free_align(ptr);
}

void HeapFree_(void* ptr)
{
#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->OnFree_Syscall(ptr);
	}
#endif // USE_PROFILE_MEMORY

	crt_free(ptr);
}

#if defined(USE_PROFILE_MEMORY)
void* PoolAlloc(
	MEM_SIZE size,
	const char* file,
	int line,
	const char* func
)
#else
void* PoolAlloc(
	MEM_SIZE size
)
#endif  // USE_PROFILE_MEMORY
{
	if (size > HEAP_SIZE_TABLE[MAX_HEAP_SIZE_IDX - 1])
	{
		void* ptr = HeapAllocAlign(
			ALIGNED_SIZE(size, DEFAULT_MEMORY_ALIGNMENT) + DEFAULT_MEMORY_ALIGNMENT,
			DEFAULT_MEMORY_ALIGNMENT
#if defined(USE_PROFILE_MEMORY)
			,file,
			line,
			func
#endif // USE_PROFILE_MEMORY
		);

		if (!ptr)
			return nullptr;

		char* p = (char*)ptr + DEFAULT_MEMORY_ALIGNMENT;
		p[-1] = (char)0xFF; // 고정 크기 풀이 아님을 표시

		return (void*)p;
	}

	int heap_idx = get_heap_size_index(size);
	if (-1 == heap_idx)
		return nullptr;

	if (!g_hPoolHeaps[heap_idx])
	{
		g_hPoolHeaps[heap_idx] = create_paged_object_pool(
			(unsigned char)heap_idx,
			HEAP_SIZE_TABLE[heap_idx],
			HEAP_BLOCK_COUNT_TABLE[heap_idx],
			TRUE
		);

		if (!g_hPoolHeaps[heap_idx])
			return nullptr;
	}

	void* ptr = paged_object_pool_alloc(g_hPoolHeaps[heap_idx]);

#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
	{
		g_pMemoryRecorder->OnAlloc_Pool(
			ptr,
			HEAP_SIZE_TABLE[heap_idx],
			file,
			line,
			func
		);
	}
#endif // USE_PROFILE_MEMORY

	return ptr;
}

void PoolFree(void* ptr)
{
	if (!ptr)
		return;

	char* p = (char*)ptr;
	char heap_idx = p[-1];

	if (heap_idx == 0xFF)
	{
		// 시스템 콜로 할당된 메모리 해제
		void* original_ptr = (void*)((char*)ptr - DEFAULT_MEMORY_ALIGNMENT);
		HeapFreeAlign(original_ptr);

		return;
	}

#if defined(USE_PROFILE_MEMORY)
	if (g_pMemoryRecorder)
		g_pMemoryRecorder->OnFree_Pool(ptr);
#endif // USE_PROFILE_MEMORY

	// 고정 크기 풀에서 할당된 메모리 해제
	paged_object_pool_free(g_hPoolHeaps[heap_idx], ptr);
}

void* TempAlloc(MEM_SIZE size)
{
	if (!g_hTempHeap)
		return nullptr;

	void* pTempHeap = temp_pool_alloc(g_hTempHeap, size);
	if (!pTempHeap)
		return nullptr;

#if defined(USE_PROFILE_MEMORY)

	acquire_spin_lock(&g_TempHeapLock);

	g_UsedAllocCount_Temp++;
	g_UsedAllocSize_Temp += size;

	release_spin_lock(&g_TempHeapLock);

#endif // USE_PROFILE_MEMORY

	return pTempHeap;
}

void TempReset()
{
	if (!g_hTempHeap)
		return;

#if defined(USE_PROFILE_MEMORY)

	acquire_spin_lock(&g_TempHeapLock);

	g_PeakAllocCount_Temp = T_MAX(g_PeakAllocCount_Temp, g_UsedAllocCount_Temp);
	g_PeakAllocSize_Temp = T_MAX(g_PeakAllocSize_Temp, g_UsedAllocSize_Temp);
	g_UsedAllocCount_Temp = 0;
	g_UsedAllocSize_Temp = 0;

	release_spin_lock(&g_TempHeapLock);

#endif // USE_PROFILE_MEMORY

	temp_pool_clear(g_hTempHeap);
}

	

	