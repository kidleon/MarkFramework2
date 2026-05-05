#pragma once


namespace mark
{
	struct memory_block
	{
		void* data;
		uint32_t size;
		uint32_t page_idx;
		uint32_t slot_idx;
		BOOL pooled;
		memory_block* next;
	};

	class MARKENGINE_API memory_pool
	{
	public:
		static void initialize();
		static void shutdown();

		static memory_block* allocate(size_t size);
		static void release(memory_block* block);

	};
}
