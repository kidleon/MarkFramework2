#include "pch.h"
#include "D3D11PrivateHeap.h"

#include "crt_memory.h"
#include "temp_pool.h"
#include "paged_object_pool.h"
#include "interlock_atomic.h"
#include "memory_recorder.h"


enum D3D11Heap_SIZE_IDX
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

size_t D3D11_HEAP_SIZE_TABLE[MAX_HEAP_SIZE_IDX] =
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

size_t D3D11_HEAP_BLOCK_COUNT_TABLE[MAX_HEAP_SIZE_IDX] =
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


struct D3D11HEAP
{
	HANDLE hPoolHeap[MAX_HEAP_SIZE_IDX];
	HANDLE hTempHeap;

	HANDLE hMemRecorder;

	volatile size_t TempPeakAllocCount;
	volatile size_t TempPeakAllocSize;
	volatile size_t TempUsedAllocCount;
	volatile size_t TempUsedAllocSize;
};

static D3D11HEAP g_D3D11Heap = {};

static inline int get_heap_size_index(size_t size)
{
	for (int i = 0; i < MAX_HEAP_SIZE_IDX; ++i)
	{
		if (size <= D3D11_HEAP_SIZE_TABLE[i])
			return i;

	}
	return -1;
}

BOOL D3D11Heap_Init(
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

	g_D3D11Heap.hTempHeap = hTempHeap;
	g_D3D11Heap.hMemRecorder = memrec_init(pfnMemoryReporter);
	
	return TRUE;
}

void D3D11Heap_Shutdown()
{
	for (int i = 0; i < MAX_HEAP_SIZE_IDX; ++i)
	{
		if (g_D3D11Heap.hPoolHeap[i])
		{
			destroy_paged_object_pool(g_D3D11Heap.hPoolHeap[i]);
			g_D3D11Heap.hPoolHeap[i] = nullptr;
		}
	}

	if (g_D3D11Heap.hTempHeap)
	{
		temppool_destroy(g_D3D11Heap.hTempHeap);
		g_D3D11Heap.hTempHeap = nullptr;
	}

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_shutdown(g_D3D11Heap.hMemRecorder);
		g_D3D11Heap.hMemRecorder = nullptr;
	}
}

HANDLE D3D11Heap_GetHandle()
{
	return (HANDLE)&g_D3D11Heap;
}

void* D3D11Heap_SysAlloc(size_t size, const char* file, int line, const char* func)
{
	void* pHeap = crt_malloc(size);
	if (!pHeap)
		return nullptr;

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onalloc_syscall(
			g_D3D11Heap.hMemRecorder,
			pHeap,
			size,
			file,
			line,
			func
		);
	}

	return pHeap;
}

void* D3D11Heap_SysRealloc(void* ptr, size_t newSize, const char* file, int line, const char* func)
{
	if (g_D3D11Heap.hMemRecorder && ptr)
	{
		memrec_onfree_syscall(
			g_D3D11Heap.hMemRecorder,
			ptr
		);
	}

	void* pHeap = crt_realloc(ptr, newSize);
	if (!pHeap)
		return nullptr;

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onalloc_syscall(
			g_D3D11Heap.hMemRecorder,
			pHeap,
			newSize,
			file,
			line,
			func
		);
	}

	return pHeap;
}

void D3D11Heap_SysFree(void* ptr)
{
	if (!ptr)
		return;

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onfree_syscall(
			g_D3D11Heap.hMemRecorder,
			ptr
		);
	}

	crt_free(ptr);
}

void* D3D11Heap_SysAllocAlign(size_t size, size_t alignment, const char* file, int line, const char* func)
{
	void* pHeap = crt_malloc_align(size, alignment);
	if (!pHeap)
		return nullptr;

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onalloc_syscall(
			g_D3D11Heap.hMemRecorder,
			pHeap,
			size,
			file,
			line,
			func
		);
	}

	return pHeap;

}

void D3D11Heap_SysFreeAlign(void* ptr)
{
	if (!ptr)
		return;

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onfree_syscall(
			g_D3D11Heap.hMemRecorder,
			ptr
		);
	}

	crt_free(ptr);
}

void* D3D11Heap_PoolAlloc(size_t size, const char* file, int line, const char* func)
{
	if (size > D3D11_HEAP_SIZE_TABLE[MAX_HEAP_SIZE_IDX - 1])
	{
		void* ptr = D3D11Heap_SysAllocAlign(
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

	if (!g_D3D11Heap.hPoolHeap[heap_idx])
	{
		g_D3D11Heap.hPoolHeap[heap_idx] = create_paged_object_pool(
			(unsigned char)heap_idx,
			D3D11_HEAP_SIZE_TABLE[heap_idx],
			D3D11_HEAP_BLOCK_COUNT_TABLE[heap_idx],
			TRUE
		);

		if (!g_D3D11Heap.hPoolHeap[heap_idx])
			return nullptr;
	}

	void* ptr = paged_object_pool_alloc(g_D3D11Heap.hPoolHeap[heap_idx]);

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onalloc_pool(
			g_D3D11Heap.hMemRecorder,
			ptr,
			D3D11_HEAP_SIZE_TABLE[heap_idx],
			file,
			line,
			func
		);
	}

	return ptr;
}

void* D3D11Heap_PoolRealloc(void* ptr, size_t oldSize, size_t newSize, const char* file, int line, const char* func)
{
	void* new_ptr = D3D11Heap_PoolAlloc(newSize, file, line, func);

	if (!new_ptr)
		return ptr;

	if (g_D3D11Heap.hMemRecorder)
	{
		if (ptr)
		{
			memrec_onfree_pool(
				g_D3D11Heap.hMemRecorder,
				ptr
			);
		}

		memrec_onalloc_pool(
			g_D3D11Heap.hMemRecorder,
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
		D3D11Heap_PoolFree(ptr);
	}

	return new_ptr;
}

void D3D11Heap_PoolFree(void* ptr)
{
	if (!ptr)
		return;

	char* p = (char*)ptr;
	char heap_idx = p[-1];

	if (heap_idx == 0xFF)
	{
		// 시스템 콜로 할당된 메모리 해제
		void* original_ptr = (void*)((char*)ptr - DEFAULT_MEMORY_ALIGNMENT);
		D3D11Heap_SysFreeAlign(original_ptr);

		return;
	}

	if (g_D3D11Heap.hMemRecorder)
	{
		memrec_onfree_pool(
			g_D3D11Heap.hMemRecorder,
			ptr
		);
	}

	// 고정 크기 풀에서 할당된 메모리 해제
	paged_object_pool_free(g_D3D11Heap.hPoolHeap[heap_idx], ptr);
}

void* D3D11Heap_TempAlloc(size_t size)
{
	void* pTempHeap = temppool_alloc(g_D3D11Heap.hTempHeap, size);
	if (!pTempHeap)
		return nullptr;

	size_t UseAllocSize = g_D3D11Heap.TempUsedAllocSize;
	interlock_increment_l((long*)&g_D3D11Heap.TempUsedAllocCount, MEMORY_ORDER_RELAXED);
	interlock_store_l((long*)&g_D3D11Heap.TempUsedAllocSize, (long)(UseAllocSize + size), MEMORY_ORDER_RELAXED);

	return pTempHeap;
}

void D3D11Heap_TempReset()
{
	interlock_store_l((long*)&g_D3D11Heap.TempUsedAllocCount, 0, MEMORY_ORDER_ACQ_REL);
	interlock_store_l((long*)&g_D3D11Heap.TempUsedAllocSize, 0, MEMORY_ORDER_ACQ_REL);

	size_t PeakAllocCount = T_MAX(g_D3D11Heap.TempPeakAllocCount, g_D3D11Heap.TempUsedAllocCount);
	size_t PeakAllocSize = T_MAX(g_D3D11Heap.TempPeakAllocSize, g_D3D11Heap.TempUsedAllocSize);
	interlock_store_l((long*)&g_D3D11Heap.TempPeakAllocCount, (long)PeakAllocCount, MEMORY_ORDER_ACQ_REL);
	interlock_store_l((long*)&g_D3D11Heap.TempPeakAllocSize, (long)PeakAllocSize, MEMORY_ORDER_ACQ_REL);

	temppool_clear(g_D3D11Heap.hTempHeap);
}
