#include "pch.h"
#include "CoreHeap.h"
#include "PrivateMemory.h"



namespace mark
{
	constexpr uint64_t CORE_HEAP_SIGNATURE = 0xDEADBEEFDEADBEEF;
	constexpr uint64_t CORE_HEAP_TEMP_POOL_SIGNATURE = 0xFEEDFACEFEEDFACE;


	struct TEMP_POOL
	{
#if defined(DEBUG)
		uint64_t signature = CORE_HEAP_TEMP_POOL_SIGNATURE; // 디버깅을 위한 시그니처
#endif // DEBUG
		char* buffer = nullptr; // 외부에서 설정한 버퍼 포인터
		size_t buffer_size = 0; // 외부에서 설정한 버퍼 크기
		temp_pool_memory_resource pool;

		TEMP_POOL(size_t size, limited_memory_resource* sysmem_res)
			: pool(size, sysmem_res)
		{
		}
	};

	struct CORE_HEAP
	{
#if defined(DEBUG)
		uint64_t signature = CORE_HEAP_SIGNATURE; // 디버깅을 위한 시그니처
#endif // DEBUG

		limited_memory_resource _limited_sys_res;
		sync_pool_memory_resource _spool_res;
		usync_pool_memory_resource _upool_res;
		temp_pool_memory_resource _temp_res;

		std::pmr::vector<HANDLE> _temp_pools; // 임시 풀 메모리 리소스 핸들 목록

		CORE_HEAP() = default;

		explicit CORE_HEAP(
			size_t limited_memory_size,
			size_t sync_pool_count_per_chunk,
			size_t unsync_pool_count_per_chunk,
			size_t sync_pool_max_size_per_block,
			size_t unsync_pool_max_size_per_block,
			size_t temp_buffer_size
		)
			: _limited_sys_res(limited_memory_size)
			, _spool_res({ sync_pool_count_per_chunk, sync_pool_max_size_per_block })
			, _upool_res({ unsync_pool_count_per_chunk, unsync_pool_max_size_per_block })
			, _temp_res(temp_buffer_size)
			
		{
		}

		~CORE_HEAP()
		{
			 // 임시 풀 메모리 리소스 핸들 목록 정리
			for (HANDLE temp_pool_handle : _temp_pools)
			{
				TEMP_POOL* temp_pool = reinterpret_cast<TEMP_POOL*>(temp_pool_handle);
				if (!temp_pool || temp_pool->signature != CORE_HEAP_TEMP_POOL_SIGNATURE)
				{
					assert(false && "Invalid temp pool handle in CORE_HEAP destructor");
					continue;
				}

				if (temp_pool->signature == CORE_HEAP_TEMP_POOL_SIGNATURE)
				{
					temp_pool->pool.release();
					_limited_sys_res.deallocate(temp_pool, sizeof(TEMP_POOL), alignof(TEMP_POOL));
				}
				else
				{
					assert(false && "Invalid temp pool handle in CORE_HEAP destructor");
				}
			}

			_temp_pools.clear();
			signature = 0; // 시그니처 초기화 (디버깅용)
		}
	};

	[[nodiscard]] __FORCEINLINE bool is_valid_core_heap_handle(CORE_HEAP* core_heap)
	{
		if (!core_heap) [[unlikely]]
		{
			assert(false && "Null core heap handle in coreheap_destroy");
			return false;
		}

#if defined(DEBUG)
		if (core_heap->signature != CORE_HEAP_SIGNATURE)
		{
			assert(false && "Invalid core heap handle in coreheap_destroy");
			return false;
		}

		return true;
#endif // DEBUG

		return true;
	}

	[[nodiscard]] __FORCEINLINE bool is_valid_temp_pool_handle(TEMP_POOL* temp_pool)
	{
		if (!temp_pool)
		{
			assert(false && "Null temp pool handle");
			return false;
		}

#if defined(DEBUG)
		if (temp_pool->signature != CORE_HEAP_TEMP_POOL_SIGNATURE)
		{
			assert(false && "Invalid temp pool handle");
			return false;
		}
#endif // DEBUG

		return true;
	}


	HANDLE coreheap_create(
		size_t limited_memory_size,
		size_t sync_pool_count_per_chunk,
		size_t unsync_pool_count_per_chunk,
		size_t sync_pool_max_size_per_block,
		size_t unsync_pool_max_size_per_block,
		size_t temp_buffer_size
	)
	{
		//CORE_HEAP* core_heap = CORE_SYS_NEW_ARGS(CORE_HEAP, limited_memory_size, sync_pool_count_per_chunk, unsync_pool_count_per_chunk, sync_pool_max_size_per_block, unsync_pool_max_size_per_block, temp_buffer_size);
		//return reinterpret_cast<HANDLE>(core_heap);
		return nullptr;
	}

	void coreheap_destroy(HANDLE heap_handle)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);

		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return;

		CORE_SYS_DELETE(CORE_HEAP, core_heap);
	}

#if defined(__MEMORY_TRACKING_ENABLED__)
	void* mark_sys_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			bytes,
			1, // count는 1로 고정 (단일 할당)
			alignment,
			loc
		);
		return ptr;
	}

	void* mark_spool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<sync_pool_memory_resource>(
			get_default_sync_pool_memory_resource_ptr(),
			bytes,
			1, // count는 1로 고정 (단일 할당)
			alignment,
			loc
		);
		return ptr;
	}

	void* mark_upool_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<usync_pool_memory_resource>(
			get_default_usync_pool_memory_resource_ptr(),
			bytes,
			1, // count는 1로 고정 (단일 할당)
			alignment,
			loc
		);
		return ptr;
	}

	void* mark_temp_alloc(
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			get_default_temp_memory_resource_ptr(),
			bytes,
			1, // count는 1로 고정 (단일 할당)
			alignment,
			loc
		);
		return ptr;
	}

	void* coreheap_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if(!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<limited_memory_resource>(
			&core_heap->_limited_sys_res,
			bytes,
			1,
			alignment,
			loc
		);

		return ptr;
	}

	void* coreheap_spool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<sync_pool_memory_resource>(
			&core_heap->_spool_res,
			bytes,
			1,
			alignment,
			loc
		);

		return ptr;
	}

	void* coreheap_upool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<usync_pool_memory_resource>(
			&core_heap->_upool_res,
			bytes,
			1,
			alignment,
			loc
		);

		return ptr;
	}

	void* coreheap_temp_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment,
		std::source_location loc
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			&core_heap->_temp_res,
			bytes,
			1,
			alignment,
			loc
		);

		return ptr;
	}

	HANDLE coreheap_temppool_create(
		HANDLE heap_handle,
		size_t size,
		std::source_location loc
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* temp_pool_mem = coreheap_sys_alloc(sizeof(TEMP_POOL), alignof(TEMP_POOL), loc);
		if (!temp_pool_mem) [[unlikely]]
			return nullptr;

		TEMP_POOL* temp_pool = new (temp_pool_mem) TEMP_POOL(size, core_heap->_limited_sysmem_res);
		core_heap->_temp_pools.push_back(reinterpret_cast<HANDLE>(temp_pool));

		return reinterpret_cast<HANDLE>(temp_pool);
	}
#else
	void* mark_sys_alloc(
		size_t bytes,
		size_t alignment
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			bytes,
			1, // count는 1로 고정 (단일 할당)
			alignment
		);
		return ptr;
	}

	void* mark_spool_alloc(
		size_t bytes,
		size_t alignment
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<sync_pool_memory_resource>(
			get_default_sync_pool_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);
		return ptr;
	}

	void* mark_upool_alloc(
		size_t bytes,
		size_t alignment
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<usync_pool_memory_resource>(
			get_default_usync_pool_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);

		return ptr;	
	}

	void* mark_temp_alloc(
		size_t bytes,
		size_t alignment
	)
	{
		void* ptr = private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			get_default_temp_memory_resource_ptr(),
			bytes,
			1,
			alignment
		);

		return ptr;
	}

	void* coreheap_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<limited_memory_resource>(
			&core_heap->_limited_sys_res,
			bytes,
			1,
			alignment
		);

		return ptr;
	}

	void* coreheap_spool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<sync_pool_memory_resource>(
			&core_heap->_spool_res,
			bytes,
			1,
			alignment
		);

		return ptr;
	}

	void* coreheap_upool_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<usync_pool_memory_resource>(
			&core_heap->_upool_res,
			bytes,
			1,
			alignment
		);

		return ptr;
	}

	void* coreheap_temp_alloc(
		HANDLE heap_handle,
		size_t bytes,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			&core_heap->_temp_res,
			bytes,
			1,
			alignment
		);

		return ptr;
	}

	HANDLE coreheap_temppool_create(
		HANDLE heap_handle,
		size_t size
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return nullptr;

		void* temp_pool_mem = mark_sys_alloc(sizeof(TEMP_POOL), alignof(TEMP_POOL));
		if (!temp_pool_mem) [[unlikely]]
			return nullptr;

		TEMP_POOL* temp_pool = new (temp_pool_mem) TEMP_POOL(size, &core_heap->_limited_sys_res);

		core_heap->_temp_pools.push_back(reinterpret_cast<HANDLE>(temp_pool));

		return reinterpret_cast<HANDLE>(temp_pool);
	}
#endif // __MEMORY_TRACKING_ENABLED__


	void mark_sys_free(
		void* ptr,
		size_t alignment
	)
	{
		private_core_detail::free_impl_with_header<select_system_memory_resource>(
			get_default_system_memory_resource_ptr(),
			ptr,
			alignment
		);
	}

	void mark_spool_free(
		void* ptr,
		size_t alignment
	)
	{
		private_core_detail::free_impl_with_header<sync_pool_memory_resource>(
			get_default_sync_pool_memory_resource_ptr(),
			ptr,
			alignment
		);
	}

	void mark_upool_free(
		void* ptr,
		size_t alignment
	)
	{
		private_core_detail::free_impl_with_header<usync_pool_memory_resource>(
			get_default_usync_pool_memory_resource_ptr(),
			ptr,
			alignment
		);
	}

	void coreheap_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return;

		private_core_detail::free_impl_with_header<limited_memory_resource>(
			&core_heap->_limited_sys_res,
			ptr,
			alignment
		);
	}

	void coreheap_spool_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return;

		private_core_detail::free_impl_with_header<sync_pool_memory_resource>(
			&core_heap->_spool_res,
			ptr,
			alignment
		);
		
	}

	void coreheap_upool_free(
		HANDLE heap_handle,
		void* ptr,
		size_t alignment
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return;

		private_core_detail::free_impl_with_header<usync_pool_memory_resource>(
			&core_heap->_upool_res,
			ptr,
			alignment
		);
	}

	void coreheap_temp_reset(
		HANDLE heap_handle
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return;

		core_heap->_temp_res.release();
	}

	void* coreheap_temppool_alloc(
		HANDLE temppool_handle,
		size_t bytes,
		size_t alignment
	)
	{
		TEMP_POOL* temp_pool = reinterpret_cast<TEMP_POOL*>(temppool_handle);
		if (!is_valid_temp_pool_handle(temp_pool)) [[unlikely]]
			return nullptr;

		void* ptr = private_core_detail::alloc_impl_with_header<temp_pool_memory_resource>(
			&temp_pool->pool,
			bytes,
			1,
			alignment
		);

		return ptr;
	}

	void coreheap_temppool_reset(
		HANDLE temppool_handle
	)
	{
		TEMP_POOL* temp_pool = reinterpret_cast<TEMP_POOL*>(temppool_handle);
		if (!is_valid_temp_pool_handle(temp_pool)) [[unlikely]]
			return;

		temp_pool->pool.release();
	}

	void coreheap_temppool_destroy(
		HANDLE heap_handle,
		HANDLE temppool_handle
	)
	{
		CORE_HEAP* core_heap = reinterpret_cast<CORE_HEAP*>(heap_handle);
		if (!is_valid_core_heap_handle(core_heap)) [[unlikely]]
			return;

		TEMP_POOL* temp_pool = reinterpret_cast<TEMP_POOL*>(temppool_handle);
		if (!is_valid_temp_pool_handle(temp_pool)) [[unlikely]]
			return;

		temp_pool->pool.release();

		temp_pool->~TEMP_POOL();
		mark_sys_free(temp_pool, alignof(TEMP_POOL));

		auto it = std::find(core_heap->_temp_pools.begin(), core_heap->_temp_pools.end(), temppool_handle);
		if (it != core_heap->_temp_pools.end())
		{
			core_heap->_temp_pools.erase(it);
		}
		else
		{
			assert(false && "Temp pool handle not found in core heap's temp pool list during destruction");
		}
	}
}
