#include "pch.h"
#include "memory_recorder.h"
#include "hash_table.h"
#include "spin_lock.h"
#include "crt_memory.h"
#include "paged_object_pool.h"
#include "fnv.h"


struct memory_record_t
{
	HANDLE paged_pool;
	struct HASH_TABLE* sysalloc_table; // 시스템 콜 할당 해시 테이블
	struct HASH_TABLE* poolalloc_table; // 풀 할당 해시 테이블

	struct spin_lock_t sys_lock; // 시스템 콜 할당 잠금
	struct spin_lock_t pool_lock; // 풀 할당 잠금

	volatile size_t syscall_alloc_count; // 시스템 콜 할당 카운트
	volatile size_t pool_alloc_count; // 풀 할당 카운트
	volatile size_t syscall_alloc_size; // 시스템 콜 할당 사이즈
	volatile size_t pool_alloc_size; // 풀 할당 사이즈

	void (*pfnLeakReporter)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
	);
};

struct memory_block_t
{
	void* ptr;
	size_t size;
	char* file;
	char* func;
	int line;
	int reserve;

	struct HASH_NODE hash_node;
};

HANDLE memrec_init(
	void (*pfnMemoryReporter)(
		const char* type,
		const char* file,
		int line,
		const char* func,
		size_t size
		)
)
{
	struct memory_record_t* recorder = (struct memory_record_t*)crt_malloc(sizeof(struct memory_record_t));
	if (!recorder)
		return NULL;

	memset(recorder, 0, sizeof(struct memory_record_t));

	recorder->pfnLeakReporter = pfnMemoryReporter;
	recorder->paged_pool = create_paged_object_pool(
		0,
		sizeof(struct memory_block_t),
		4096,
		TRUE
	);

	if (!recorder->paged_pool)
	{
		crt_free(recorder);
		return NULL;
	}

	recorder->sysalloc_table = create_hash_table(2048);
	recorder->poolalloc_table = create_hash_table(2048);

	init_spin_lock(&recorder->sys_lock);
	init_spin_lock(&recorder->pool_lock);

	return (HANDLE)recorder;
}

void memrec_shutdown(HANDLE hRecorder)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return;

	if (recorder->poolalloc_table)
	{
		delete_hash_table(recorder->poolalloc_table);
		recorder->poolalloc_table = NULL;
	}

	if (recorder->sysalloc_table)
	{
		delete_hash_table(recorder->sysalloc_table);
		recorder->sysalloc_table = NULL;
	}

	if (recorder->paged_pool)
	{
		destroy_paged_object_pool(recorder->paged_pool);
		recorder->paged_pool = NULL;
	}

	recorder->pfnLeakReporter = NULL;
}

void memrec_report_leaks(HANDLE hRecorder)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return;
}

void memrec_onalloc_syscall(
	HANDLE hRecorder,
	void* ptr,
	size_t size,
	const char* file,
	int line,
	const char* func
)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return;

	struct memory_block_t* node = (struct memory_block_t*)paged_object_pool_alloc(recorder->paged_pool);
	if (!node)
		return;

	node->ptr = ptr;
	node->size = size;
	node->file = (char*)file;
	node->func = (char*)func;
	node->line = line;
	node->hash_node.data = (void*)node;

	uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

	acquire_spin_lock(&recorder->sys_lock);

	insert_hash_node(
		recorder->sysalloc_table,
		hash_key,
		&node->hash_node
	);

	recorder->syscall_alloc_count++;
	recorder->syscall_alloc_size += size;

	release_spin_lock(&recorder->sys_lock);
}

void memrec_onfree_syscall(
	HANDLE hRecorder,
	void* ptr
)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder || !ptr)
		return;

	uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

	acquire_spin_lock(&recorder->sys_lock);
	struct memory_block_t* node = (struct memory_block_t*)query_hash_node(recorder->sysalloc_table, hash_key);
	if (node)
	{
		delete_hash_node(recorder->sysalloc_table, hash_key);
		recorder->syscall_alloc_count--;
		recorder->syscall_alloc_size -= node->size;
	}
	release_spin_lock(&recorder->sys_lock);

	paged_object_pool_free(recorder->paged_pool, node);
}

void memrec_onalloc_pool(
	HANDLE hRecorder,
	void* ptr,
	size_t size,
	const char* file,
	int line,
	const char* func
)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return;

	struct memory_block_t* node = (struct memory_block_t*)paged_object_pool_alloc(recorder->paged_pool);
	if (!node)
		return;

	node->ptr = ptr;
	node->size = size;
	node->file = (char*)file;
	node->func = (char*)func;
	node->line = line;
	node->hash_node.data = (void*)node;

	uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

	acquire_spin_lock(&recorder->pool_lock);
	insert_hash_node(
		recorder->poolalloc_table,
		hash_key,
		&node->hash_node
	);

	recorder->pool_alloc_count++;
	recorder->pool_alloc_size += size;

	release_spin_lock(&recorder->pool_lock);
}

void memrec_onfree_pool(
	HANDLE hRecorder,
	void* ptr
)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder || !ptr)
		return;

	uint64 hash_key = fnv64(ptr, sizeof(void*), 0); // 해시값 계산

	acquire_spin_lock(&recorder->pool_lock);

	struct memory_block_t* node = (struct memory_block_t*)query_hash_node(recorder->poolalloc_table, hash_key);
	if (node)
	{
		delete_hash_node(recorder->poolalloc_table, hash_key);
		recorder->pool_alloc_count--;
		recorder->pool_alloc_size -= node->size;
	}

	release_spin_lock(&recorder->pool_lock);

	paged_object_pool_free(recorder->paged_pool, node);
}

size_t memrec_getcount_syscall(HANDLE hRecorder)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return 0;

	return recorder->syscall_alloc_count;
}

size_t memrec_getcount_pool(HANDLE hRecorder)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return 0;

	return recorder->pool_alloc_count;
}

size_t memrec_getsize_syscall(HANDLE hRecorder)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return 0;

	return recorder->syscall_alloc_size;
}

size_t memrec_getsize_pool(HANDLE hRecorder)
{
	struct memory_record_t* recorder = (struct memory_record_t*)hRecorder;
	if (!recorder)
		return 0;

	return recorder->pool_alloc_size;
}