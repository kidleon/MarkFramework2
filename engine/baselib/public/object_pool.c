#include "pch.h"
#include "object_pool.h"
#include "crt_memory.h"
#include "spin_lock.h"


struct opp_block_t
{
	struct opp_block_t* next;
};

struct opp_t
{
	size_t block_size; 	// 블록 사이즈
	size_t block_count_per_page; // 블록 갯수
	size_t aligned_block_size; // 블록 사이즈 + padding for alignment
	BOOL thread_safe; // Thread Safe 여부
	struct spin_lock_t lock; // Spin Lock for thread safety
	struct opp_block_t* blocks;
	void* block_orig;
};


HANDLE create_object_pool(
	size_t block_size,
	size_t block_count_per_page,
	BOOL thread_safe
)
{
	if (!block_size || !block_count_per_page)
		return NULL;

	struct opp_t* opp = (struct opp_t*)crt_malloc(sizeof(struct opp_t));
	if (!opp)
		return NULL;

	opp->block_size = block_size;
	opp->block_count_per_page = block_count_per_page;
	opp->aligned_block_size = ALIGNED_SIZE(block_size, DEFAULT_MEMORY_BLOCK_ALIGNMENT);
	opp->thread_safe = thread_safe;
	if (thread_safe)
		init_spin_lock(&opp->lock);

	opp->blocks = (struct opp_block_t*)crt_malloc_align(
		opp->aligned_block_size * block_count_per_page,
		DEFAULT_MEMORY_BLOCK_ALIGNMENT
	);
	memset(opp->blocks, 0, opp->aligned_block_size * block_count_per_page);
	opp->block_orig = opp->blocks; // 원본 블록 포인터 저장
	
	for (size_t i = 0; i < block_count_per_page - 1; ++i)
	{
		struct opp_block_t* next_block = &opp->blocks[i + 1];
		opp->blocks[i].next = next_block;
	}
	opp->blocks[block_count_per_page - 1].next = NULL; // 마지막 블록은 NULL로 설정

	if (!opp->blocks)
	{
		crt_free_align(opp);
		return NULL;
	}

	return (HANDLE)opp;
}

void destroy_object_pool(
	HANDLE pool_handle
)
{
	if (!pool_handle) return;

	struct opp_t* opp = (struct opp_t*)pool_handle;
	if (opp->block_orig)
		crt_free_align((void*)opp->block_orig);
	crt_free((void*)opp);
}

void* object_pool_alloc(
	HANDLE pool_handle
)
{
	if (!pool_handle)
		return NULL;

	struct opp_t* opp = (struct opp_t*)pool_handle;

	if (opp->thread_safe)
	{
		acquire_spin_lock(&opp->lock);

		struct opp_block_t* block = opp->blocks;
		opp->blocks = block->next;

		release_spin_lock(&opp->lock);

		return (void*)block;
	}
	else
	{
		struct opp_block_t* block = opp->blocks;
		opp->blocks = block->next;

		return (void*)block;
	}
}

void object_pool_release(
	HANDLE pool_handle,
	void* ptr
)
{
	if (!pool_handle || !ptr)
		return;

	struct opp_t* opp = (struct opp_t*)pool_handle;
	struct opp_block_t* block = (struct opp_block_t*)ptr;

	if (opp->thread_safe)
	{
		acquire_spin_lock(&opp->lock);
		block->next = opp->blocks;
		opp->blocks = block;
		release_spin_lock(&opp->lock);
	}
	else
	{
		block->next = opp->blocks;
		opp->blocks = block;
	}
}