#include "pch.h"
#include "paged_object_pool.h"
#include "spin_lock.h"
#include "crt_memory.h"


struct popp_block_t
{
	struct popp_block_t* next;
};

struct popp_page_t
{
	void* page_orig;
	struct popp_page_t* next_page;
	struct popp_block_t* first_block;
	struct popp_block_t* last_block;
};

struct popp_t
{
	MEM_SIZE block_size; 	// 블록 사이즈
	MEM_SIZE block_count_per_page; // 블록 갯수
	MEM_SIZE aligned_block_size; // 블록 사이즈 + padding(8)

	MEM_SIZE system_alloc_size; // 시스템 콜로 할당된 메모리 사이즈
	MEM_SIZE alloc_size; // 할당된 총 메모리 사이즈
	MEM_SIZE free_size; // FREE 총 메모리 사이즈

	struct popp_page_t* pages;
	struct popp_block_t* free_list;

	struct spin_lock_t lock; // Spin Lock for thread safety
	BOOL thread_safe; // Thread Safe 여부

	unsigned char index;
	unsigned char reserved[7];
};


MARK_BASELIB_C_API HANDLE create_paged_object_pool(
	unsigned char pool_index,
	MEM_SIZE block_size,
	MEM_SIZE block_count_per_page,
	BOOL thread_safe
)
{
	if (!block_size || !block_count_per_page)
		return NULL;

	struct popp_t* opp = (struct popp_t*)crt_malloc(
		sizeof(struct popp_t) 
	);

	if (!opp)
		return NULL;

	opp->index = pool_index;

	opp->block_size = block_size;
	opp->block_count_per_page = block_count_per_page;
	opp->aligned_block_size = ALIGNED_SIZE(block_size, DEFAULT_MEMORY_BLOCK_ALIGNMENT) + DEFAULT_MEMORY_BLOCK_ALIGNMENT;
	opp->system_alloc_size = 0;
	opp->alloc_size = 0;
	opp->free_size = 0;
	opp->pages = NULL;
	opp->free_list = NULL;

	if (thread_safe)
		init_spin_lock(&opp->lock);

	opp->thread_safe = thread_safe;

	return (HANDLE)opp;
}

void destroy_paged_object_pool(
	HANDLE pool_handle
)
{
	if (!pool_handle) return;

	struct popp_t* opp = (struct popp_t*)pool_handle;
	struct popp_page_t* page = opp->pages;
	while (page)
	{
		struct popp_page_t* next_page = page->next_page;
		if (page->page_orig)
			crt_free_align(page->page_orig);
		crt_free(page);
		page = next_page;
	}

	crt_free((void*)opp);
}

static struct popp_page_t* create_page(
	MEM_SIZE aligned_block_size,
	MEM_SIZE block_count_per_page
)
{
	struct popp_page_t* new_page = (struct popp_page_t*)crt_malloc(
		sizeof(struct popp_page_t)
	);

	if (!new_page)
		return NULL;

	new_page->page_orig = crt_malloc_align(
		aligned_block_size * block_count_per_page,
		DEFAULT_MEMORY_BLOCK_ALIGNMENT
	);

	if (!new_page->page_orig)
	{
		crt_free_align(new_page);
		return NULL;
	}

	new_page->first_block = (struct popp_block_t*)new_page->page_orig;
	new_page->last_block = (struct popp_block_t*)((char*)new_page->first_block + (block_count_per_page - 1) * aligned_block_size);

	for (int i = 0; i < block_count_per_page - 1; ++i)
	{
		struct popp_block_t* cur_block = (struct popp_block_t*)((char*)new_page->first_block + i * aligned_block_size);
		struct popp_block_t* next_block = (struct popp_block_t*)((char*)new_page->first_block + (i + 1) * aligned_block_size);
		cur_block->next = next_block;
	}

	new_page->last_block->next = NULL;

	return new_page;
}

void* paged_object_pool_alloc(
	HANDLE pool_handle
)
{
	if (!pool_handle)
		return NULL;

	struct popp_t* opp = (struct popp_t*)pool_handle;

	if (opp->thread_safe)
	{
		acquire_spin_lock(&opp->lock);
	}

	if (!opp->free_list)
	{
		// 새로운 페이지 할당
		struct popp_page_t* new_page = create_page(
			opp->aligned_block_size,
			opp->block_count_per_page
		);
		if (!new_page)
			goto LB_FAILED;
		
		opp->system_alloc_size += 
			ALIGNED_SIZE(sizeof(struct popp_page_t), DEFAULT_MEMORY_ALIGNMENT) 
			+ opp->aligned_block_size 
			* opp->block_count_per_page;

		new_page->next_page = opp->pages;
		opp->pages = new_page;
		opp->free_list = new_page->first_block;
		opp->free_size += opp->aligned_block_size * opp->block_count_per_page;
	}

	struct popp_block_t* block = opp->free_list;
	opp->free_list = block->next;
	opp->alloc_size += opp->block_size;
	opp->free_size -= opp->block_size;

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}

	//*(MEM_SIZE*)block = opp->block_size;

	char* heap = (char*)block + DEFAULT_MEMORY_BLOCK_ALIGNMENT;
	heap[-1] = opp->index; // pool index 기록

	return (void*)heap;

LB_FAILED:
	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}

	return NULL;
}

void paged_object_pool_free(
	HANDLE pool_handle,
	void* ptr
)
{
	if (!pool_handle || !ptr)
		return;

	struct popp_t* opp = (struct popp_t*)pool_handle;
	struct popp_block_t* block = (struct popp_block_t*)((char*)ptr - DEFAULT_MEMORY_BLOCK_ALIGNMENT);

	if (opp->thread_safe)
	{
		acquire_spin_lock(&opp->lock);
	}

	block->next = opp->free_list;
	opp->free_list = block;
	opp->alloc_size -= opp->block_size;
	opp->free_size += opp->block_size;

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}
}

size_t paged_object_pool_get_used_size(
	HANDLE pool_handle
)
{
	if (!pool_handle)
		return 0;

	struct popp_t* opp = (struct popp_t*)pool_handle;

	if (opp->thread_safe)
	{
		acquire_spin_lock(&opp->lock);
	}

	size_t result = opp->system_alloc_size + sizeof(struct popp_t);

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}

	return result;
}
