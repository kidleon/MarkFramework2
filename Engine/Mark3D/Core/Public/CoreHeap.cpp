#include "pch.h"
#include "CoreHeap.h"
#include <mimalloc/mimalloc.h>


#if defined(__MEMORY_TRACKER_ENABLED__)
#include "MemoryTracker.h"
#define MEM_ON_ALLOCATE(ptr, bytes, alignment, loc) mark::memory_tracker::get().on_allocate(ptr, bytes, alignment, loc)
#define MEM_ON_DEALLOCATE(ptr) mark::memory_tracker::get().on_deallocate(ptr)
#else
#define MEM_ON_ALLOCATE(ptr, bytes, alignment, loc) ((void)0)
#define MEM_ON_DEALLOCATE(ptr) ((void)0)
#endif // __MEMORY_TRACKING_ENABLED__

namespace mark
{
	struct temp_pool
	{
		size_t buffer_size;
		size_t current_offset;
		void* buffer;
		bool own_buffer;

		temp_pool()
			: buffer_size(0)
			, current_offset(0)
			, buffer(nullptr)
			, own_buffer(false)
		{
		}

		temp_pool(size_t size)
			: buffer_size(0)
			, current_offset(0)
			, buffer(nullptr)
			, own_buffer(true)
		{
			buffer_size = size;
			current_offset = 0;
			buffer = sys_alloc(size);
		}

		temp_pool(void* external_buffer, size_t size)
			: buffer_size(size)
			, current_offset(0)
			, buffer(external_buffer)
			, own_buffer(false)
		{
		}

		~temp_pool() noexcept
		{
			if (own_buffer && buffer)
				sys_free(buffer);
			buffer = nullptr;
			buffer_size = 0;
			current_offset = 0;
		}

		[[nodiscard]] inline void* alloc(size_t bytes, size_t alignment)
		{
			size_t current_address = reinterpret_cast<size_t>(buffer) + current_offset;
			size_t aligned_address = (current_address + alignment - 1) & ~(alignment - 1);
			size_t padding = aligned_address - current_address;

			if (current_offset + padding + bytes > buffer_size) 
				return nullptr; // 버퍼 오버플로우

			current_offset += padding + bytes;
			return reinterpret_cast<void*>(aligned_address);
		}

		[[nodiscard]] inline void* alloc(size_t bytes)
		{
			return alloc(bytes, alignof(std::max_align_t));
		}

		inline void reset()
		{
			current_offset = 0;
		}
	};

	static temp_pool* g_temp_pool = nullptr;

	bool initialize_core_memory(size_t default_temp_size)
	{
		if (!g_temp_pool && 0 < default_temp_size)
		{
			void* raw_pool = sys_alloc(sizeof(temp_pool));
			g_temp_pool = new (raw_pool) temp_pool(default_temp_size); // placement new로 temp_pool 객체 생성
		}

		return true;
	}

	void shutdown_core_memory()
	{
		if (g_temp_pool)
		{
			g_temp_pool->~temp_pool();
			sys_free(g_temp_pool);
			g_temp_pool = nullptr;
		}
	}

#if defined(__MEMORY_TRACKER_ENABLED__)
	void* sys_alloc(size_t bytes, std::source_location loc)
	{
		void* ptr = mi_malloc(bytes);
		MEM_ON_ALLOCATE(ptr, bytes, 0, loc);
		return ptr;
	}

	void* sys_alloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* ptr = mi_malloc_aligned(bytes, alignment);
		MEM_ON_ALLOCATE(ptr, bytes, alignment, loc);
		return ptr;
	}

	void* sys_calloc(size_t bytes, std::source_location loc)
	{
		void* ptr = mi_calloc(1, bytes);
		MEM_ON_ALLOCATE(ptr, bytes, 0, loc);
		return ptr;
	}

	void* sys_calloc(size_t bytes, size_t alignment, std::source_location loc)
	{
		void* ptr = mi_calloc_aligned(1, bytes, alignment);
		MEM_ON_ALLOCATE(ptr, bytes, alignment, loc);
		return ptr;
	}

	void* sys_realloc(void* ptr, size_t new_bytes, std::source_location loc)
	{
		MEM_ON_DEALLOCATE(ptr);
		void* new_ptr = mi_realloc(ptr, new_bytes);
		MEM_ON_ALLOCATE(new_ptr, new_bytes, 0, loc);
		return new_ptr;
	}

	void* sys_realloc(void* ptr, size_t new_bytes, size_t alignment, std::source_location loc)
	{
		MEM_ON_DEALLOCATE(ptr);
		void* new_ptr = mi_realloc_aligned(ptr, new_bytes, alignment);
		MEM_ON_ALLOCATE(new_ptr, new_bytes, alignment, loc);
		return new_ptr;
	}
#else
	void* sys_alloc(size_t bytes)
	{
		return mi_malloc(bytes);
	}

	void* sys_alloc(size_t bytes, size_t alignment)
	{
		return mi_malloc_aligned(bytes, alignment);
	}

	void* sys_calloc(size_t bytes)
	{
		return mi_calloc(1, bytes);
	}

	void* sys_calloc(size_t bytes, size_t alignment)
	{
		return mi_calloc_aligned(1, bytes, alignment);
	}

	void* sys_realloc(void* ptr, size_t new_bytes)
	{
		return mi_realloc(ptr, new_bytes);
	}

	void* sys_realloc(void* ptr, size_t new_bytes, size_t alignment)
	{
		return mi_realloc_aligned(ptr, new_bytes, alignment);
	}
#endif // __MEMORY_TRACKER_ENABLED__

	void sys_free(void* ptr)
	{
		MEM_ON_DEALLOCATE(ptr);
		mi_free(ptr);
	}

	void* temp_alloc(size_t bytes)
	{
		if (g_temp_pool)
			return g_temp_pool->alloc(bytes);
		return nullptr;
	}

	void* temp_alloc(size_t bytes, size_t alignment)
	{
		if (g_temp_pool)
			return g_temp_pool->alloc(bytes, alignment);
		return nullptr;
	}

	void* temp_calloc(size_t bytes)
	{
		void* ptr = temp_alloc(bytes);
		if (ptr)
			std::memset(ptr, 0, bytes);
		return ptr;
	}

	void* temp_calloc(size_t bytes, size_t alignment)
	{
		void* ptr = temp_alloc(bytes, alignment);
		if (ptr)
			std::memset(ptr, 0, bytes);
		return ptr;
	}

	void temp_reset()
	{
		if (g_temp_pool)
			g_temp_pool->reset();
	}

	HANDLE temppool_create(size_t buffer_size)
	{
		temp_pool* pool = CORE_NEW(temp_pool)(buffer_size);
		return reinterpret_cast<HANDLE>(pool);
	}

	void temppool_destroy(HANDLE temp_handle)
	{
		if (temp_handle)
		{
			temp_pool* pool = reinterpret_cast<temp_pool*>(temp_handle);
			CORE_DELETE(temp_pool, pool);
		}
	}

	void* temppool_alloc(HANDLE temp_handle, size_t bytes)
	{
		if (temp_handle)
		{
			temp_pool* pool = reinterpret_cast<temp_pool*>(temp_handle);
			return pool->alloc(bytes);
		}

		return nullptr;
	}

	void* temppool_alloc(HANDLE temp_handle, size_t bytes, size_t alignment)
	{
		if (temp_handle)
		{
			temp_pool* pool = reinterpret_cast<temp_pool*>(temp_handle);
			return pool->alloc(bytes, alignment);
		}
		return nullptr;
	}

	void* temppool_calloc(HANDLE temp_handle, size_t bytes)
	{
		void* ptr = temppool_alloc(temp_handle, bytes);
		if (ptr)
			std::memset(ptr, 0, bytes);
		return ptr;
	}

	void* temppool_calloc(HANDLE temp_handle, size_t bytes, size_t alignment)
	{
		void* ptr = temppool_alloc(temp_handle, bytes, alignment);
		if (ptr)
			std::memset(ptr, 0, bytes);
		return ptr;
	}

	void temppool_reset(HANDLE temp_handle)
	{
		if (temp_handle)
		{
			temp_pool* pool = reinterpret_cast<temp_pool*>(temp_handle);
			pool->reset();
		}
	}
}
