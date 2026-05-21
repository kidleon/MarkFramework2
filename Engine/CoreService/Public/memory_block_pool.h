#pragma once
#include "core_generic.h"
#include <fixed_vector.h>


namespace mark
{
	struct memory_block_id
	{
		uint16_t page_index;
		uint16_t block_index;
	};

	class memory_block_pool
	{
	public:
		memory_block_pool();
		~memory_block_pool();

		memory_block_id acquire();
		void release(const memory_block_id& block_id);

		void* get_block_memory(const memory_block_id& block_id) const;

	private:
		struct memory_block_page
		{
			void* memory;
		};

		sys_list<memory_block_id> free_blocks;
		mtl::fixed_vector<memory_block_page, 256> pages;

	};
}
