#pragma once


namespace mark
{
	MARKENGINE_API HANDLE coreheap_create(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	);

	MARKENGINE_API void coreheap_destroy(
		HANDLE heap_handle
	);

#if defined(__MEMORY_TRACKING_ENABLED__)
	MARKENGINE_API void* mark_sys_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	MARKENGINE_API void* mark_spool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	MARKENGINE_API void* mark_upool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	MARKENGINE_API void* mark_temp_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc = std::source_location::current()
	);

	MARKENGINE_API void* coreheap_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	MARKENGINE_API void* coreheap_spool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	MARKENGINE_API void* coreheap_upool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	MARKENGINE_API void* coreheap_temp_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	);

	MARKENGINE_API HANDLE coreheap_temppool_create(
		HANDLE heap_handle,
		size_t size,
		std::source_location loc
	);

#else
	MARKENGINE_API void* mark_sys_alloc(
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* mark_spool_alloc(
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* mark_upool_alloc(
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* mark_temp_alloc(
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* coreheap_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* coreheap_spool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* coreheap_upool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void* coreheap_temp_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API HANDLE coreheap_temppool_create(
		HANDLE heap_handle,
		size_t size
	);

#endif // __MEMORY_TRACKING_ENABLED__
	MARKENGINE_API void mark_sys_free(
		void* ptr,
		size_t alignment
	);

	MARKENGINE_API void mark_spool_free(
		void* ptr,
		size_t alignment
	);

	MARKENGINE_API void mark_upool_free(
		void* ptr,
		size_t alignment
	);

	MARKENGINE_API void coreheap_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	);

	MARKENGINE_API void coreheap_spool_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	);

	MARKENGINE_API void coreheap_upool_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	);

	MARKENGINE_API void coreheap_temp_reset(
		HANDLE heap_handle
	);

	MARKENGINE_API void* coreheap_temppool_alloc(
		HANDLE temppool_handle,
		size_t bytes,
		size_t alignment
	);

	MARKENGINE_API void coreheap_temppool_reset(
		HANDLE temppool_handle
	);

	MARKENGINE_API void coreheap_temppool_destroy(
		HANDLE heap_handle,
		HANDLE temppool_handle
	);
}
