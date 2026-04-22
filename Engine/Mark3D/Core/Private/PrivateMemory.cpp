#include "pch.h"
#include "PrivateMemory.h"


namespace mark
{
	static select_system_memory_resource* s_default_system_memory = nullptr;
	static sync_pool_memory_resource* s_default_sync_pool_memory_resource = nullptr;
	static usync_pool_memory_resource* s_default_usync_pool_memory_resource = nullptr;
	static temp_pool_memory_resource* s_default_temp_memory_resource = nullptr;

	vector<HANDLE, ALLOC::SYS<HANDLE>> s_temp_pool_handles;

	constexpr uint64_t CORE_TEMP_POOL_SIGNATURE = 0xFEEDFACEFEEDFACE;

	struct CORE_TEMP_POOL
	{
		uint64_t signature = CORE_TEMP_POOL_SIGNATURE; // 디버깅을 위한 시그니처
		std::pmr::monotonic_buffer_resource pool;

		CORE_TEMP_POOL(size_t size, select_system_memory_resource* sysmem_res)
			: pool(size, sysmem_res)
		{
		}

		CORE_TEMP_POOL(const CORE_TEMP_POOL&) = default;
		CORE_TEMP_POOL(CORE_TEMP_POOL&&) = default;

		inline void release()
		{
			pool.release();
		}
	};

	bool initialize_core_memory(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	)
	{
		try
		{
#if defined(__MEMORY_LIMIT_ENABLED__)
			if(0 == limited_memory_size)
				limited_memory_size = 1024 * 1024 * 1024; // 기본적으로 1GB 제한

			if (!s_default_system_memory)
			{
				s_default_system_memory = new select_system_memory_resource(limited_memory_size);
				std::pmr::set_default_resource(s_default_system_memory);
			}
#else
			if (!s_default_system_memory)
			{
				s_default_system_memory = new select_system_memory_resource();
				std::pmr::set_default_resource(s_default_system_memory);
			}
#endif // __MEMORY_LIMIT_ENABLED__

			if (!s_default_sync_pool_memory_resource)
			{
				std::pmr::pool_options sync_pool_options;
				sync_pool_options.max_blocks_per_chunk = sync_pool_count_per_chunk;
				sync_pool_options.largest_required_pool_block = sync_pool_max_size_per_block;
				s_default_sync_pool_memory_resource = new sync_pool_memory_resource(sync_pool_options);
			}

			if (!s_default_usync_pool_memory_resource)
			{
				std::pmr::pool_options unsync_pool_options;
				unsync_pool_options.max_blocks_per_chunk = unsync_pool_count_per_chunk;
				unsync_pool_options.largest_required_pool_block = unsync_pool_max_size_per_block;
				s_default_usync_pool_memory_resource = new usync_pool_memory_resource(unsync_pool_options);
			}

			if (!s_default_temp_memory_resource)
			{
				s_default_temp_memory_resource = new temp_pool_memory_resource(temp_buffer_size);
			}

			s_temp_pool_handles.reserve(16); // 임시 풀 핸들 초기 예약 (필요에 따라 확장)

			return true;
		}
		catch (const std::exception& e)
		{
			assert(false && e.what());

			shutdown_core_memory();

			return false;
		}
	}

	void shutdown_core_memory()
	{
		for (size_t i = 0; i < s_temp_pool_handles.size(); ++i)
		{
			HANDLE temp_pool_handle = s_temp_pool_handles[i];
			if (temp_pool_handle)
			{
				// 임시 풀 메모리 리소스 해제
				temp_pool_memory_resource* temp_res = reinterpret_cast<temp_pool_memory_resource*>(temp_pool_handle);
				temp_res->release();
				s_temp_pool_handles[i] = nullptr;
			}
		}
		s_temp_pool_handles.clear();

		CHECK_DELETE(s_default_temp_memory_resource);
		CHECK_DELETE(s_default_usync_pool_memory_resource);
		CHECK_DELETE(s_default_sync_pool_memory_resource);
		CHECK_DELETE(s_default_system_memory);
	}

	select_system_memory_resource* get_default_system_memory_resource_ptr() noexcept
	{
		return s_default_system_memory;
	}

	sync_pool_memory_resource* get_default_sync_pool_memory_resource_ptr() noexcept
	{
		return s_default_sync_pool_memory_resource;
	}

	usync_pool_memory_resource* get_default_usync_pool_memory_resource_ptr() noexcept
	{
		return s_default_usync_pool_memory_resource;
	}

	temp_pool_memory_resource* get_default_temp_memory_resource_ptr() noexcept
	{
		return s_default_temp_memory_resource;
	}

#if defined(__MEMORY_TRACKING_ENABLED__)
	void* core_sys_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_spool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_sync_pool_memory_resource_ptr(),
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_upool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_unsync_pool_memory_resource_ptr(),
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_temp_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_temp_memory_resource_ptr(),
			bytes,
			1,
			alignment,
			loc
		);
	}

	void* core_sys_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_system_memory_resource_ptr(),
			bytes,
			count,
			alignment,
			loc
		);
	}

	void* core_spool_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_sync_pool_memory_resource_ptr(),
			bytes,
			count,
			alignment,
			loc
		);
	}

	void* core_upool_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_usync_pool_memory_resource_ptr(),
			bytes,
			count,
			alignment,
			loc
		);
	}

	void* core_temp_alloc(
		size_t bytes,
		size_t count,
		size_t alignment,
		std::source_location loc
	)
	{
		return private_core_detail::alloc_impl_with_header(
			get_default_temp_memory_resource_ptr(),
			bytes,
			count,
			alignment,
			loc
		);
	}
#else
	void* core_sys_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);
	}

	void* core_spool_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<sync_pool_memory_resource>(
			get_default_sync_pool_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);
	}

	void* core_upool_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<usync_pool_memory_resource>(
			get_default_usync_pool_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);
	}

	void* core_temp_alloc(size_t bytes, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			get_default_temp_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);
	}

	void* core_sys_alloc(size_t bytes, size_t count, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			bytes,
			count,
			alignment
		);
	}

	void* core_spool_alloc(size_t bytes, size_t count, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<sync_pool_memory_resource>(
			get_default_sync_pool_memory_resource_ptr(),
			bytes,
			count,
			alignment
		);
	}

	void* core_upool_alloc(size_t bytes, size_t count, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<usync_pool_memory_resource>(
			get_default_usync_pool_memory_resource_ptr(),
			bytes,
			count,
			alignment
		);

	}
	void* core_temp_alloc(size_t bytes, size_t count, size_t alignment)
	{
		return private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			get_default_temp_memory_resource_ptr(),
			bytes,
			count,
			alignment
		);
	}
#endif // __MEMORY_TRACKING_ENABLED__

	void core_sys_free(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return;

		private_core_detail::free_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			ptr,
			alignment
		);
	}

	void core_spool_free(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return;

		private_core_detail::free_impl_with_header<sync_pool_memory_resource>(
			get_default_sync_pool_memory_resource_ptr(),
			ptr,
			alignment
		);
	}

	void core_upool_free(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return;

		private_core_detail::free_impl_with_header<usync_pool_memory_resource>(
			get_default_usync_pool_memory_resource_ptr(),
			ptr,
			alignment
		);
	}

	void core_temp_reset()
	{
		auto temp_pool = get_default_temp_memory_resource_ptr();
		if (temp_pool)
		{
			temp_pool->release();
		}
	}

	size_t get_alloc_count(void* ptr, size_t alignment)
	{
		if (!ptr) [[unlikely]] return 0;
		private_core_detail::heap_header* header =
			(private_core_detail::heap_header*)((char*)ptr - private_core_detail::get_heap_header_size(alignment));
		return header->count;
	}


	// temppool 관련 함수
	HANDLE temppool_create(size_t initial_size)
	{
		CORE_TEMP_POOL* temp_pool = CORE_SYS_NEW(CORE_TEMP_POOL)(initial_size, get_default_system_memory_resource_ptr());
		return reinterpret_cast<HANDLE>(temp_pool);
	}

	void temppool_destroy(HANDLE pool_handle)
	{
		if (!pool_handle)
			[[unlikely]] return;

		CORE_TEMP_POOL* temp_pool = reinterpret_cast<CORE_TEMP_POOL*>(pool_handle);

#if !defined(DEBUG)
		if (temp_pool->signature != CORE_TEMP_POOL_SIGNATURE)
		{
			assert(false && "Invalid temp pool handle");
			return nullptr;
		}
#endif // !MASTER

		temp_pool->pool.release(); // 풀 메모리 리소스 해제
		CORE_SYS_DELETE(CORE_TEMP_POOL, temp_pool); // CORE_TEMP_POOL 구조체 자체 해제
	}

	void* temppool_alloc(HANDLE pool_handle, size_t bytes, size_t alignment)
	{
		if (!pool_handle)
			[[unlikely]] return nullptr;

		CORE_TEMP_POOL* temp_pool = reinterpret_cast<CORE_TEMP_POOL*>(pool_handle);
#if !defined(DEBUG)
		if (temp_pool->signature != CORE_TEMP_POOL_SIGNATURE)
		{
			assert(false && "Invalid temp pool handle");
			return nullptr;
		}
#endif // !MASTER

		return temp_pool->pool.allocate(bytes, alignment);
	}

	void temppool_reset(HANDLE pool_handle)
	{
		if (!pool_handle)
			[[unlikely]] return;

		CORE_TEMP_POOL* temp_pool = reinterpret_cast<CORE_TEMP_POOL*>(pool_handle);
#if !defined(DEBUG)
		if (temp_pool->signature != CORE_TEMP_POOL_SIGNATURE)
		{
			assert(false && "Invalid temp pool handle");
			return nullptr;
		}
#endif // !MASTER

		temp_pool->pool.release();
	}
}

