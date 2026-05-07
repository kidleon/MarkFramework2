#include "pch.h"
#include "memory_pool.h"
#include "core_heap.h"
#include "core_generic.h"
#include "spin_lock.h"
#include "log.h"


namespace mark
{
	constexpr static size_t BLOCK_SIZE[] =
	{
		256, 512, 1024, 2048,
		4096, 8192, 16384, 32768,
		65536, 131072, 262144, 524288,
		1048576, 2097152, 4194304, 8388608,
		16777216
	};

	constexpr static size_t BLOCK_POOL_COUNT[] =
	{
		64, 64, 64, 64,
		32, 32, 32, 32,
		16, 16, 16, 16,
		8, 8, 8, 8,
		4
	};

	constexpr size_t NUM_BLOCK = sizeof(BLOCK_SIZE) / sizeof(size_t);

	struct memory_pool_page
	{
		memory_block* blocks = nullptr;
		uint32_t block_count = 0;
		uint32_t page_index = 0;
	};

	struct memory_pool_table
	{
		sys_vector<memory_pool_page> pages;
		memory_block* free_list = nullptr;
		spin_lock_t lock = { 0 };
	};

	struct memory_pool_manager
	{
		memory_pool_table tables[NUM_BLOCK];
	};

	static memory_pool_manager* manager;

	void memory_pool::initialize()
	{
		manager = static_cast<memory_pool_manager*>(CORE_SYS_CALLOC(sizeof(memory_pool_manager)));
		new (manager) memory_pool_manager();

		for (size_t i = 0; i < NUM_BLOCK; ++i)
		{
			manager->tables[i].pages.reserve(256);
			manager->tables[i].free_list = nullptr;
			manager->tables[i].lock.stat = 0;
		}
	}

	void memory_pool::shutdown()
	{
		for (size_t i = 0; i < NUM_BLOCK; ++i)
		{
			memory_pool_table& table = manager->tables[i];
			for (memory_pool_page& page : table.pages)
			{
				CORE_SYS_FREE(page.blocks);
			}
			table.pages.clear();
			table.free_list = nullptr;
		}

		CORE_SYS_FREE(manager);
		manager = nullptr;
	}

	inline int32_t get_size_index(size_t size)
	{
		for (size_t i = 0; i < NUM_BLOCK; ++i)
		{
			if (size <= BLOCK_SIZE[i])
				return static_cast<int32_t>(i);
		}

		return -1;
	}

	memory_block* memory_pool::allocate(size_t size)
	{
		int32_t size_index = get_size_index(size);
		if (-1 == size_index)
		{
			memory_block* block = static_cast<memory_block*>(CORE_SYS_CALLOC(sizeof(memory_block)));
			block->size = (uint32_t)size;
			block->pooled = FALSE;

			return block;
		}

		memory_pool_table& table = manager->tables[size_index];

		AUTO_SPIN_LOCK loc(&table.lock);
		if (!table.free_list)
		{
			memory_pool_page new_page;
			new_page.block_count = (uint32_t)BLOCK_POOL_COUNT[size_index];
			new_page.page_index = static_cast<uint32_t>(table.pages.size());
			new_page.blocks = static_cast<memory_block*>(CORE_SYS_CALLOC(sizeof(memory_block) * new_page.block_count));

			for (uint32_t i = 0; i < new_page.block_count; ++i)
			{
				memory_block* block = &new_page.blocks[i];
				block->size = (uint32_t)BLOCK_SIZE[size_index];
				block->page_idx = new_page.page_index;
				block->slot_idx = i;
				block->pooled = TRUE;
				block->next = table.free_list;
				table.free_list = block;
			}

			table.pages.push_back(new_page);
		}

		memory_block* block = table.free_list;
		table.free_list = block->next;
		block->next = nullptr;

		return block;
	}

	void memory_pool::release(memory_block* block)
	{
		if (!block) return;

		if (!block->pooled)
		{
			CORE_SYS_FREE(block);
			return;
		}

		int32_t size_index = get_size_index(block->size);
		if (-1 == size_index)
		{
			SYS_LOG_ERR_F("Invalid block size {} for release", block->size);
			return;
		}

		memory_pool_table& table = manager->tables[size_index];

		AUTO_SPIN_LOCK loc(&table.lock);
		block->next = table.free_list;
		table.free_list = block;
	}
}
