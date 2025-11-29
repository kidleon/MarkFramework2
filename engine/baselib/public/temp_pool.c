#include "pch.h"
#include "temp_pool.h"
#include "crt_memory.h"
#include "spin_lock.h"


struct tpp_t
{
	size_t pool_size; 	// 블록 사이즈
	size_t aligned_block_size; // 블록 사이즈 + padding for alignment
	BOOL thread_safe; // Thread Safe 여부
	struct spin_lock_t lock; // Spin Lock for thread safety
	void* heap;
	size_t cur_pos;
	size_t padding;
};

HANDLE create_temp_pool(
	size_t pool_size,
	BOOL thread_safe
)
{
	if (!pool_size)
		return NULL;
	struct tpp_t* tpp = (struct tpp_t*)crt_malloc(sizeof(struct tpp_t));
	if (!tpp)
		return NULL;
	tpp->pool_size = pool_size;
	tpp->aligned_block_size = ALIGNED_SIZE(pool_size, DEFAULT_MEMORY_BLOCK_ALIGNMENT);
	tpp->thread_safe = thread_safe;
	if (thread_safe)
		init_spin_lock(&tpp->lock);
	tpp->heap = crt_malloc_align(tpp->aligned_block_size, DEFAULT_MEMORY_BLOCK_ALIGNMENT);
	tpp->cur_pos = 0;
	tpp->padding = 0;

	if (!tpp->heap)
	{
		crt_free_align(tpp);
		return NULL;
	}

	return (HANDLE)tpp;
}

void destroy_temp_pool(
	HANDLE pool_handle
)
{
	if (!pool_handle) return;
	struct tpp_t* tpp = (struct tpp_t*)pool_handle;
	if (tpp->heap)
		crt_free_align(tpp->heap);
	crt_free((void*)tpp);
}

void* temp_pool_alloc(
	HANDLE pool_handle,
	size_t alloc_size
)
{
	if (!pool_handle || !alloc_size)
		return NULL;

	struct tpp_t* tpp = (struct tpp_t*)pool_handle;

	if (tpp->thread_safe)
	{
		if (tpp->aligned_block_size < alloc_size)
			return NULL; // 요청한 크기가 블록 사이즈보다 크면 할당 불가

		acquire_spin_lock(&tpp->lock);

		if (tpp->cur_pos + alloc_size > tpp->pool_size)
		{
			release_spin_lock(&tpp->lock);
			return NULL; // 할당 불가
		}

		void* heap = (void*)((char*)tpp->heap + tpp->cur_pos);
		tpp->cur_pos += alloc_size;

		release_spin_lock(&tpp->lock);

		return heap;
	}
	else
	{
		if (tpp->aligned_block_size < alloc_size || 
			tpp->cur_pos + alloc_size > tpp->pool_size)
			return NULL; // 할당 불가

		void* heap = (void*)((char*)tpp->heap + tpp->cur_pos);
		tpp->cur_pos += alloc_size;

		return heap;
	}
}

void temp_pool_clear(
	HANDLE pool_handle
)
{
	if (!pool_handle) return;
	struct tpp_t* tpp = (struct tpp_t*)pool_handle;
	if (tpp->thread_safe)
		acquire_spin_lock(&tpp->lock);
	tpp->cur_pos = 0; // 현재 위치를 초기화
	if (tpp->thread_safe)
		release_spin_lock(&tpp->lock);
}