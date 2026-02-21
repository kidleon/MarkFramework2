#include "pch.h"
#include "CoreHeap.h"
#include "crt_memory.h"
#include "temp_pool.h"
#include "paged_object_pool.h"
#include "interlock_atomic.h"
//#include "MemoryRecorder.h"
#include "memory_recorder.h"


enum COREHEAP_SIZE_IDX
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

size_t COREHEAP_SIZE_TABLE[MAX_HEAP_SIZE_IDX] =
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

size_t COREHEAP_BLOCK_COUNT_TABLE[MAX_HEAP_SIZE_IDX] =
{
	512,		// HEAP_64B
	512,		// HEAP_128B
	512,		// HEAP_256B
	512,		// HEAP_512B

	256,		// HEAP_1KB
	256,		// HEAP_2KB
	256,		// HEAP_4KB
	256,		// HEAP_8KB

	128,		// HEAP_16KB
	128,		// HEAP_32KB
	128,		// HEAP_64KB
	128,		// HEAP_128KB

	32,			// HEAP_256KB
	32,			// HEAP_512KB
	8,			// HEAP_1MB
	4,			// HEAP_2MB
};


struct CoreHeap
{
	HANDLE hPoolHeap[MAX_HEAP_SIZE_IDX];
	HANDLE hTempHeap;

	HANDLE hMemRecorder;

	volatile size_t TempPeakAllocCount;
	volatile size_t TempPeakAllocSize;
	volatile size_t TempUsedAllocCount;
	volatile size_t TempUsedAllocSize;
};

static CoreHeap g_CoreHeap = {};

static inline int get_heap_size_index(size_t size)
{
	for (int i = 0; i < MAX_HEAP_SIZE_IDX; ++i)
	{
		if (size <= COREHEAP_SIZE_TABLE[i])
			return i;
	
	}
	return -1;
}

BOOL CoreHeap_Init(
	size_t TempSize,
	void (*pfnMemoryReporter)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
	)
)
{
	HANDLE hTempHeap = temppool_create(TempSize, FALSE);
	if (!hTempHeap)
		return FALSE;

	g_CoreHeap.hTempHeap = hTempHeap;
	g_CoreHeap.hMemRecorder = memrec_init(64, pfnMemoryReporter);

	return TRUE;
}

void CoreHeap_Shutdown()
{
	for (int i = 0; i < MAX_HEAP_SIZE_IDX; ++i)
	{
		if (g_CoreHeap.hPoolHeap[i])
		{
			destroy_paged_object_pool(g_CoreHeap.hPoolHeap[i]);
			g_CoreHeap.hPoolHeap[i] = nullptr;
		}
	}

	if (g_CoreHeap.hTempHeap)
	{
		temppool_destroy(g_CoreHeap.hTempHeap);
		g_CoreHeap.hTempHeap = nullptr;
	}

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_shutdown(g_CoreHeap.hMemRecorder);
		g_CoreHeap.hMemRecorder = nullptr;
	}
}

HANDLE CoreHeap_GetHandle()
{
	return (HANDLE)&g_CoreHeap;
}

void* CoreHeap_SysAlloc(size_t size, const char* file, int line, const char* func)
{
	void* pHeap = crt_malloc(size);
	if (!pHeap)
		return nullptr;

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onalloc_syscall(
			g_CoreHeap.hMemRecorder,
			pHeap,
			size,
			file,
			line,
			func
		);
	}

	return pHeap;
}

void* CoreHeap_SysRealloc(void* ptr, size_t newSize, const char* file, int line, const char* func)
{
	if (g_CoreHeap.hMemRecorder && ptr)
	{
		memrec_onfree_syscall(
			g_CoreHeap.hMemRecorder,
			ptr
		);
	}

	void* pHeap = crt_realloc(ptr, newSize);
	if (!pHeap)
		return nullptr;
	
	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onalloc_syscall(
			g_CoreHeap.hMemRecorder,
			pHeap,
			newSize,
			file,
			line,
			func
		);
	}

	return pHeap;
}

void CoreHeap_SysFree(void* ptr)
{
	if (!ptr)
		return;

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onfree_syscall(
			g_CoreHeap.hMemRecorder,
			ptr
		);
	}

	crt_free(ptr);
}

void* CoreHeap_SysAllocAlign(size_t size, size_t alignment, const char* file, int line, const char* func)
{
	void* pHeap = crt_malloc_align(size, alignment);
	if (!pHeap)
		return nullptr;

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onalloc_syscall(
			g_CoreHeap.hMemRecorder,
			pHeap,
			size,
			file,
			line,
			func
		);
	}

	return pHeap;
	
}

void CoreHeap_SysFreeAlign(void* ptr)
{
	if (!ptr)
		return;

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onfree_syscall(
			g_CoreHeap.hMemRecorder,
			ptr
		);
	}

	crt_free(ptr);
}

void* CoreHeap_PoolAlloc(size_t size, const char* file, int line, const char* func)
{
	if (size > COREHEAP_SIZE_TABLE[MAX_HEAP_SIZE_IDX - 1])
	{
		void* ptr = CoreHeap_SysAllocAlign(
			ALIGNED_SIZE(size, DEFAULT_MEMORY_ALIGNMENT) + DEFAULT_MEMORY_ALIGNMENT,
			DEFAULT_MEMORY_ALIGNMENT,
			file,
			line,
			func
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

	if (!g_CoreHeap.hPoolHeap[heap_idx])
	{
		g_CoreHeap.hPoolHeap[heap_idx] = create_paged_object_pool(
			(unsigned char)heap_idx,
			COREHEAP_SIZE_TABLE[heap_idx],
			COREHEAP_BLOCK_COUNT_TABLE[heap_idx],
			TRUE
		);

		if (!g_CoreHeap.hPoolHeap[heap_idx])
			return nullptr;
	}

	void* ptr = paged_object_pool_alloc(g_CoreHeap.hPoolHeap[heap_idx]);

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onalloc_pool(
			g_CoreHeap.hMemRecorder,
			ptr,
			COREHEAP_SIZE_TABLE[heap_idx],
			file,
			line,
			func
		);
	}

	return ptr;
}

void* CoreHeap_PoolRealloc(void* ptr, size_t oldSize, size_t newSize, const char* file, int line, const char* func)
{
	void* new_ptr = CoreHeap_PoolAlloc(newSize, file, line, func);

	if (!new_ptr)
		return ptr;

	if (g_CoreHeap.hMemRecorder)
	{
		if (ptr)
		{
			memrec_onfree_pool(
				g_CoreHeap.hMemRecorder,
				ptr
			);
		}

		memrec_onalloc_pool(
			g_CoreHeap.hMemRecorder,
			new_ptr,
			newSize,
			file,
			line,
			func
		);
	}

	if (ptr)
	{
		memcpy(new_ptr, ptr, oldSize);
		CoreHeap_PoolFree(ptr);
	}

	return new_ptr;
}

void CoreHeap_PoolFree(void* ptr)
{
	if (!ptr)
		return;

	char* p = (char*)ptr;
	char heap_idx = p[-1];

	if (heap_idx == 0xFF)
	{
		// 시스템 콜로 할당된 메모리 해제
		void* original_ptr = (void*)((char*)ptr - DEFAULT_MEMORY_ALIGNMENT);
		CoreHeap_SysFreeAlign(original_ptr);

		return;
	}

	if (g_CoreHeap.hMemRecorder)
	{
		memrec_onfree_pool(
			g_CoreHeap.hMemRecorder,
			ptr
		);
	}

	// 고정 크기 풀에서 할당된 메모리 해제
	paged_object_pool_free(g_CoreHeap.hPoolHeap[heap_idx], ptr);
}

void* CoreHeap_TempAlloc(size_t size)
{
	void* pTempHeap = temppool_alloc(g_CoreHeap.hTempHeap, size);
	if (!pTempHeap)
		return nullptr;

	size_t UseAllocSize = g_CoreHeap.TempUsedAllocSize;
	interlock_increment_l((long*)&g_CoreHeap.TempUsedAllocCount, MEMORY_ORDER_RELAXED);
	interlock_store_l((long*)&g_CoreHeap.TempUsedAllocSize, (long)(UseAllocSize + size), MEMORY_ORDER_RELAXED);

	return pTempHeap;
}

void CoreHeap_TempReset()
{
	interlock_store_l((long*)&g_CoreHeap.TempUsedAllocCount, 0, MEMORY_ORDER_ACQ_REL);
	interlock_store_l((long*)&g_CoreHeap.TempUsedAllocSize, 0, MEMORY_ORDER_ACQ_REL);

	size_t PeakAllocCount = T_MAX(g_CoreHeap.TempPeakAllocCount, g_CoreHeap.TempUsedAllocCount);
	size_t PeakAllocSize = T_MAX(g_CoreHeap.TempPeakAllocSize, g_CoreHeap.TempUsedAllocSize);
	interlock_store_l((long*)&g_CoreHeap.TempPeakAllocCount, (long)PeakAllocCount, MEMORY_ORDER_ACQ_REL);
	interlock_store_l((long*)&g_CoreHeap.TempPeakAllocSize, (long)PeakAllocSize, MEMORY_ORDER_ACQ_REL);

	temppool_clear(g_CoreHeap.hTempHeap);
}
