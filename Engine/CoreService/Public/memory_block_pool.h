#pragma once
#include "core_generic.h"


namespace mark
{
	static constexpr uint16_t MEMORY_BLOCK_POOL_MAX_PAGES = 512;
	static constexpr uint16_t MEMORY_BLOCK_POOL_MAX_BLOCKS_PER_PAGE = 256;

	struct memory_block_id
	{
		uint16_t page_index;
		uint8_t  block_index;
		uint8_t  generation;
	};

	MARKENGINE_API HANDLE create_memory_block_pool(
		size_t block_size,
		size_t blocks_per_page
	);

	MARKENGINE_API void destroy_memory_block_pool(
		HANDLE pool
	);

	MARKENGINE_API memory_block_id acquire_memory_block(
		HANDLE pool
	);

	MARKENGINE_API void release_memory_block(
		HANDLE pool,
		memory_block_id block_id
	);

	MARKENGINE_API void* get_memory_block_memory(
		HANDLE pool,
		memory_block_id block_id
	);

	MARKENGINE_API bool is_memory_block_alive(
		HANDLE pool,
		memory_block_id block_id
	);
}
