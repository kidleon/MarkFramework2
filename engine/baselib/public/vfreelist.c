#include "pch.h"
#include "vfreelist.h"
#include "linked_list.h"
#include "crt_memory.h"


typedef enum BLOCK_STATE
{
	BLOCK_FREE,
	BLOCK_SPLIT,
	BLOCK_ALLOCATED
} BLOCK_STATE;

struct free_block_t
{
	uint32 offset;
	struct free_block_t* next;
};

struct free_list_t
{
	uint32 total_size;
	uint32 min_block_size;
	uint32 max_level;
	uint32 total_blocks;
	struct free_block_t** free_lists;
	BLOCK_STATE* block_states;

	uint32 used_size;
	uint32 alloc_count;
};

#define INVALID_OFFSET 0xFFFFFFFF
#define INVALID_LEVEL 0xFFFFFFFF


static inline uint32 roundup_pow2(uint32 size)
{
	if (!size) return 1;
	size--;
	size |= size >> 1;
	size |= size >> 2;
	size |= size >> 4;
	size |= size >> 8;
	size |= size >> 16;

	return size + 1;
}

// 블록의 레벨을 반환하는 함수
static inline uint32 get_level(uint32 min_block_size, uint32 block_size)
{
	uint32 level = 0;
	uint32 size = min_block_size;
	while (size < block_size) 
	{
		size <<= 1;
		level++;
	}

	return level;
}

// 블록의 크기를 반환하는 함수
static inline uint32 get_block_size(uint32 min_block_size, uint32 level)
{
	return min_block_size << level;
}

// 블록의 인덱스를 반환하는 함수
static inline uint32 get_block_index(
	uint32 min_block_size, 
	uint32 level, 
	uint32 offset
)
{
	uint32 block_size = get_block_size(min_block_size, level);
	uint32 level_offs = (1 << level) - 1;
	uint32 block_in_level = offset / block_size;
	return level_offs + block_in_level;
}

// 두 블록이 서로 짝꿍인지 확인하는 함수
static inline uint32 get_buddy_offset(
	uint32 offset,
	uint32 block_size
) 
{
	return offset ^ block_size;
}

// 프리 리스트에 블록을 추가하는 함수
static inline BOOL add_to_free_list(
	struct free_list_t* free_list, 
	uint32 level,
	uint32 offset
) 
{
	struct free_block_t* block = (struct free_block_t*)malloc(sizeof(struct free_block_t));

	if (!block)
		return FALSE;

	block->offset = offset;
	block->next = free_list->free_lists[level];
	free_list->free_lists[level] = block;

	return TRUE;
}

// 프리 리스트에서 블록을 제거하는 함수
static inline void remove_from_free_list(
	struct free_list_t* free_list, 
	uint32 level,
	uint32 offset
)
{
	struct free_block_t* prev = NULL;
	struct free_block_t* curr = free_list->free_lists[level];

	while (NULL != curr) 
	{
		if (curr->offset == offset) 
		{
			if (prev == NULL) 
			{
				free_list->free_lists[level] = curr->next;
			} 
			else 
			{
				prev->next = curr->next;
			}

			free(curr);

			return;
		}

		prev = curr;
		curr = curr->next;
	}
}

static uint32 find_freeblock(
	struct free_list_t* free_list,
	uint32 level
)
{
	// 해당 레벨에 사용 가능한 블록이 있는지 확인
	if (free_list->free_lists[level])
	{
		struct free_block_t* block = free_list->free_lists[level];
		uint32 offset = block->offset;

		free_list->free_lists[level] = block->next;
		crt_free(block);

		return offset;
	}

	for (uint32 upper_level = level + 1; upper_level < free_list->max_level; upper_level++)
	{
		// 상위 레벨에서 사용 가능한 블록이 있는지 확인
		if (free_list->free_lists[upper_level])
		{
			struct free_block_t* block = free_list->free_lists[upper_level];
			uint32 offset = block->offset;
			free_list->free_lists[upper_level] = block->next;
			crt_free(block);

			// 블록을 분할하여 하위 레벨로 이동
			for (uint32 split_level = upper_level - 1; split_level > level; split_level--)
			{
				uint32 block_size = get_block_size(free_list->min_block_size, split_level);
				uint32 buddy_offset = offset + block_size;

				// 분할된 블록을 프리 리스트에 추가
				add_to_free_list(free_list, split_level, buddy_offset);

				// 블록 상태 업데이트
				uint32 block_index = get_block_index(free_list->min_block_size, split_level + 1, buddy_offset);
				if (block_index < free_list->total_blocks)
					free_list->block_states[block_index] = BLOCK_SPLIT;
			}

			return offset;
		}
	}

	return INVALID_OFFSET;
}

static uint32 find_blocklevel(struct free_list_t* free_list, uint32 offset)
{
	for (uint32 level = 0; level < free_list->max_level; level++)
	{
		uint32 block_index = get_block_index(free_list->min_block_size, level, offset);
		if (block_index < free_list->total_blocks &&
			BLOCK_ALLOCATED == free_list->block_states[block_index])
		{
			return level;
		}
	}

	return INVALID_LEVEL;
}

static BOOL freelist_initialize(
	struct free_list_t* freelist,
	uint32 size,
	uint32 min_block_size
)
{
	if ((size & (size - 1)) != 0)
		return FALSE;

	if ((min_block_size & (min_block_size - 1)) != 0)
		return FALSE;

	freelist->total_size = size;
	freelist->min_block_size = min_block_size;
	freelist->max_level = 0;

	uint32 temp_size = size;
	while (temp_size > min_block_size)
	{
		temp_size >>= 1;
		freelist->max_level++;
	}
	freelist->max_level++; // min_block_size 레벨 포함

	// free lists 할당
	freelist->free_lists = (struct free_block_t**)calloc(freelist->max_level, sizeof(struct free_block_t*));
	if (!freelist->free_lists)
		return FALSE;

	// block states 할당
	freelist->total_blocks = (1 << freelist->max_level) - 1;
	freelist->block_states = (BLOCK_STATE*)calloc(freelist->total_blocks, sizeof(BLOCK_STATE));
	if (!freelist->block_states)
	{
		crt_free(freelist->free_lists);
		return FALSE;
	}

	// 초기 전체 블록을 프리 리스트에 추가
	add_to_free_list(freelist, freelist->max_level - 1, 0);

	freelist->used_size = 0;
	freelist->alloc_count = 0;

	return TRUE;
}

static void freelist_cleanup(
	struct free_list_t* freelist
)
{
	// 프리 리스트의 모든 블록 해제
	for (uint32 level = 0; level < freelist->max_level; level++) 
	{
		struct free_block_t* block = freelist->free_lists[level];
		while (block) 
		{
			struct free_block_t* next = block->next;
			crt_free(block);
			block = next;
		}
	}
	crt_free(freelist->free_lists);
	crt_free(freelist->block_states);
}

static uint32 freelist_alloc(
	struct free_list_t* freelist,
	uint32 size
)
{
	// 요청된 크기를 2의 제곱수로 올림
	uint32 block_size = roundup_pow2(size);

	if (block_size < freelist->min_block_size)
		block_size = freelist->min_block_size;

	if (block_size > freelist->total_size)
		return INVALID_OFFSET;

	uint32 level = get_level(freelist->min_block_size, block_size);
	if (INVALID_LEVEL == level)
		return INVALID_OFFSET;

	uint32 offset = find_freeblock(freelist, level);
	if (INVALID_OFFSET == offset)
		return INVALID_OFFSET;

	// 블록 상태 업데이트
	uint32 block_index = get_block_index(freelist->min_block_size, level, offset);
	if (block_index < freelist->total_blocks)
		freelist->block_states[block_index] = BLOCK_ALLOCATED;

	freelist->used_size += block_size;
	freelist->alloc_count++;

	return offset;
}

static void freelist_free(
	struct free_list_t* freelist,
	uint32 offset
)
{
	if (offset >= freelist->total_size)
		return;

	uint32 level = find_blocklevel(freelist, offset);
	if (INVALID_LEVEL == level)
		return;

	uint32 block_size = get_block_size(freelist->min_block_size, level);
	uint32 block_index = get_block_index(freelist->min_block_size, level, offset);

	// 블록 상태 업데이트
	if (block_index < freelist->total_blocks)
		freelist->block_states[block_index] = BLOCK_FREE;

	// 블록 병합
	while (level < freelist->max_level - 1)
	{
		uint32 buddy_offset = get_buddy_offset(offset, block_size);
		uint32 buddy_index = get_block_index(freelist->min_block_size, level, buddy_offset);

		// 버디 블록이 freelist에 있는지 확인
		if (buddy_index >= freelist->total_blocks ||
			freelist->block_states[buddy_index] != BLOCK_FREE)
		{
			break; // 병합 불가
		}

		// 짝꿍 블록 제거
		remove_from_free_list(freelist, level, buddy_offset);

		// 오프셋과 블록 크기 업데이트
		if (buddy_offset < offset)
			offset = buddy_offset;

		block_size <<= 1;
		level++;

		block_index = get_block_index(freelist->min_block_size, level, offset);
		if (block_index < freelist->total_blocks)
			freelist->block_states[block_index] = BLOCK_FREE;
	}

	// 병합된 블록을 프리 리스트에 추가
	add_to_free_list(freelist, level, offset);

	freelist->used_size -= get_block_index(freelist->min_block_size, level, offset);
	freelist->alloc_count--;
}

HANDLE vfreelist_create(
	size_t size,
	size_t min_block_size
)
{
	struct free_list_t* freelist = (struct free_list_t*)malloc(sizeof(struct free_list_t));

	if (!freelist)
		return NULL;

	if (!freelist_initialize(freelist, (uint32)size, (uint32)min_block_size))
	{
		crt_free(freelist);
		return NULL;
	}

	return (HANDLE)freelist;
}

void vfreelist_destroy(
	HANDLE hFreelist
)
{
	struct free_list_t* freelist = (struct free_list_t*)hFreelist;
	if (!freelist)
		return;

	freelist_cleanup(freelist);

	crt_free(freelist);
}

uint32 vfreelist_alloc(
	HANDLE hFreelist,
	size_t size
)
{
	struct free_list_t* freelist = (struct free_list_t*)hFreelist;
	if (!freelist)
		return INVALID_OFFSET;

	return freelist_alloc(freelist, (uint32)size);
}

void vfreelist_free(
	HANDLE hFreelist,
	uint64 offset
)
{
	struct free_list_t* freelist = (struct free_list_t*)hFreelist;
	if (!freelist)
		return;
	freelist_free(freelist, (uint32)offset);
}