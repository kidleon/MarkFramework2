#pragma once
#include "core_generic.h"


namespace mark
{
	// 페이지 최대 512장, 페이지당 블록 최대 256개.
	// page_index는 uint16_t(0..511), block_index는 uint8_t(0..255).
	static constexpr uint16_t MEMORY_BLOCK_POOL_MAX_PAGES = 512;
	static constexpr uint16_t MEMORY_BLOCK_POOL_MAX_BLOCKS_PER_PAGE = 256;

	// generation == 0 은 invalid 핸들 (zero-init 호환).
	// release할 때마다 슬롯 generation이 증가하므로, 해제된 슬롯의 옛 핸들은
	// 같은 슬롯이 재할당되어도 generation 불일치로 stale 판정됨.
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
