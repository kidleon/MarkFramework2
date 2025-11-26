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


HANDLE create_paged_object_pool(
	unsigned char pool_index,
	MEM_SIZE block_size,
	MEM_SIZE block_count_per_page,
	BOOL thread_safe
)
{
	if (!block_size || !block_count_per_page)
		return NULL;

	struct popp_t* opp = (struct popp_t*)crt_malloc_align(
		sizeof(struct popp_t), 
		DEFAULT_MEMORY_ALIGNMENT
	);

	if (!opp)
		return NULL;

	opp->index = pool_index;

	opp->block_size = block_size;
	opp->block_count_per_page = block_count_per_page;
	opp->aligned_block_size = ALIGNED_SIZE(block_size, DEFAULT_MEMORY_ALIGNMENT) + DEFAULT_MEMORY_ALIGNMENT;
	opp->system_alloc_size = 0;
	opp->alloc_size = 0;
	opp->free_size = 0;
	opp->pages = nullptr;
	opp->free_list = nullptr;

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
	popp_page_t* page = opp->pages;
	while (page)
	{
		popp_page_t* next_page = page->next_page;
		if (page->page_orig)
			crt_free_align(page->page_orig);
		crt_free_align(page);
		page = next_page;
	}

	crt_free_align((void*)opp);
}

popp_page_t* create_page(
	MEM_SIZE aligned_block_size,
	MEM_SIZE block_count_per_page
)
{
	popp_page_t* new_page = (popp_page_t*)crt_malloc_align(
		sizeof(popp_page_t),
		DEFAULT_MEMORY_ALIGNMENT
	);

	if (!new_page)
		return NULL;

	new_page->page_orig = crt_malloc_align(
		aligned_block_size * block_count_per_page,
		DEFAULT_MEMORY_ALIGNMENT
	);

	if (!new_page->page_orig)
	{
		crt_free_align(new_page);
		return nullptr;
	}

	new_page->first_block = (popp_block_t*)new_page->page_orig;
	new_page->last_block = (popp_block_t*)((char*)new_page->first_block + (block_count_per_page - 1) * aligned_block_size);

	for (int i = 0; i < block_count_per_page - 1; ++i)
	{
		popp_block_t* current_block = (popp_block_t*)((char*)new_page->first_block + i * aligned_block_size);
		popp_block_t* next_block = (popp_block_t*)((char*)new_page->first_block + (i + 1) * aligned_block_size);
		current_block->next = next_block;
	}

	new_page->last_block->next = nullptr;

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
		popp_page_t* new_page = create_page(
			opp->aligned_block_size,
			opp->block_count_per_page
		);
		if (!new_page)
			goto LB_FAILED;
		
		opp->system_alloc_size += 
			ALIGNED_SIZE(sizeof(popp_page_t), DEFAULT_MEMORY_ALIGNMENT) 
			+ opp->aligned_block_size 
			* opp->block_count_per_page;

		new_page->next_page = opp->pages;
		opp->pages = new_page;
		opp->free_list = new_page->first_block;
		opp->free_size += opp->aligned_block_size * opp->block_count_per_page;
	}

	popp_block_t* block = opp->free_list;
	opp->free_list = block->next;
	opp->alloc_size += opp->block_size;
	opp->free_size -= opp->block_size;

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}

	//*(MEM_SIZE*)block = opp->block_size;

	char* heap = (char*)block + DEFAULT_MEMORY_ALIGNMENT;
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
	popp_block_t* block = (popp_block_t*)((char*)ptr - DEFAULT_MEMORY_ALIGNMENT);

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

	size_t result = opp->system_alloc_size + sizeof(popp_t);

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}

	return result;
}

/*
struct alignas(16) popp_block_t
{
	unsigned index;
	unsigned reserved;
	popp_block_t* next;
	char ptr[0];
};

struct alignas(16) popp_page_t
{
	void* page_orig;
	popp_page_t* next_page;
	popp_block_t* blocks;
};

struct alignas(16) popp_t
{
	size_t block_size; 	// 블록 사이즈
	size_t block_count_per_page; // 블록 갯수
	size_t aligned_block_size; // 블록 사이즈 + padding for alignment
	BOOL thread_safe; // Thread Safe 여부
	struct spin_lock_t lock; // Spin Lock for thread safety
	struct popp_page_t* pages;
	struct popp_block_t* free_list;
};

__inline size_t popp_compute_memory_alignment(size_t alloc_size, size_t alignment) noexcept
{
	if (alignment == 0)
		return alloc_size;
	size_t padding = (alignment - (alloc_size & (alignment - 1))) & (alignment - 1);
	return alloc_size + padding;
}

HANDLE create_paged_object_pool(
	size_t block_size,
	size_t block_count_per_page,
	BOOL thread_safe
)
{
	if (!block_size || !block_count_per_page)
		return NULL;

	struct popp_t* opp = (struct popp_t*)crt_malloc_align(sizeof(struct popp_t), 16);
	if (!opp)
		return NULL;

	opp->block_size = block_size;
	opp->block_count_per_page = block_count_per_page;
	opp->aligned_block_size = popp_compute_memory_alignment(sizeof(popp_block_t) + block_size, 16);
	opp->thread_safe = thread_safe;

	if (thread_safe)
		init_spin_lock(&opp->lock);

	opp->pages = nullptr;
	opp->free_list = nullptr;

	return (HANDLE)opp;
}

void destroy_paged_object_pool(
	HANDLE pool_handle
)
{
	if (!pool_handle) return;

	struct popp_t* opp = (struct popp_t*)pool_handle;
	popp_page_t* page = opp->pages;

	while (page)
	{
		popp_page_t* next_page = page->next_page;
		if (page->page_orig)
			crt_free_align(page->page_orig);
		crt_free_align(page);
		page = next_page;
	}

	crt_free_align((void*)opp);
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
		popp_page_t* new_page = (popp_page_t*)crt_malloc_align(
			sizeof(popp_page_t),
			16
		);

		if (!new_page)
		{
			if (opp->thread_safe)
			{
				release_spin_lock(&opp->lock);
			}
			return NULL;
		}

		new_page->page_orig = crt_malloc_align(
			opp->aligned_block_size * opp->block_count_per_page,
			16
		);

		if (!new_page->page_orig)
		{
			crt_free_align(new_page);
			if (opp->thread_safe)
			{
				release_spin_lock(&opp->lock);
			}
			return NULL;
		}

		new_page->blocks = (popp_block_t*)new_page->page_orig;

		for (size_t i = 0; i < opp->block_count_per_page; ++i)
		{
			popp_block_t* block = (popp_block_t*)((char*)new_page->blocks + i * opp->aligned_block_size);
			block->index = 0;
			block->next = opp->free_list;
			opp->free_list = block;
		}

		new_page->next_page = opp->pages;
		opp->pages = new_page;
	}

	popp_block_t* block = opp->free_list;

	opp->free_list = block->next;

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}

	return (void*)block->ptr;
}

void paged_object_pool_release(
	HANDLE pool_handle,
	void* ptr
)
{
	if (!pool_handle || !ptr)
		return;

	struct popp_t* opp = (struct popp_t*)pool_handle;
	popp_block_t* block = (popp_block_t*)((char*)ptr - offsetof(popp_block_t, ptr));
	if (opp->thread_safe)
	{
		acquire_spin_lock(&opp->lock);
	}

	block->next = opp->free_list;
	opp->free_list = block;

	if (opp->thread_safe)
	{
		release_spin_lock(&opp->lock);
	}
}
*/