#ifndef __MEMORY_STATS_H__
#define __MEMORY_STATS_H__


namespace mark
{
	struct MARKENGINE_API MemoryStats
	{
		size_t sys_alloc_count;
		size_t pool_alloc_count;
		size_t peak_temp_count;

		size_t sys_alloc_size;
		size_t pool_alloc_size;
		size_t peak_temp_size;
	};
}


#endif // __MEMORY_STATS_H__
