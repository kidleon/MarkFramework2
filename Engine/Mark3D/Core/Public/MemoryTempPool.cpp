#include "pch.h"
#include "MemoryTempPool.h"


namespace mark
{
	constexpr static uint64_t TEMPALLOC_MAGIC = 0xDEADBEEFDEADBEEF;

	struct tempalloc
	{
		uint64_t magic;
		size_t total_size;
		size_t used_size;
		void* buffer;
	};

	constexpr static size_t DEFAULT_TEMPALLOC_ALIGNMENT = 16;

	HANDLE temppool_create(size_t size)
	{
		tempalloc* temp_alloc = (tempalloc*)CORE_SYS_ALLOC(sizeof(tempalloc));
		if (!temp_alloc) return nullptr;

		temp_alloc->magic = TEMPALLOC_MAGIC;
		temp_alloc->total_size = size;
		temp_alloc->used_size = 0;
		temp_alloc->buffer = CORE_SYS_ALLOC_A(size, DEFAULT_TEMPALLOC_ALIGNMENT);

		return (HANDLE)temp_alloc;
	}

	void temppool_destroy(HANDLE temp_alloc_handle)
	{
		if (!temp_alloc_handle) return;

		tempalloc* temp_alloc = (tempalloc*)temp_alloc_handle;
		if (temp_alloc->magic != TEMPALLOC_MAGIC)
		{
			assert(false && "Invalid tempalloc handle");
			return;
		}

		if (temp_alloc->buffer)
			CORE_SYS_FREE_A(temp_alloc->buffer, DEFAULT_TEMPALLOC_ALIGNMENT);

		CORE_SYS_FREE(temp_alloc);
	}

	void* temppool_alloc(
		HANDLE temp_alloc_handle,
		size_t size
	)
	{
		if (!temp_alloc_handle) return nullptr;

		tempalloc* temp_alloc = (tempalloc*)temp_alloc_handle;
		if (temp_alloc->magic != TEMPALLOC_MAGIC)
		{
			assert(false && "Invalid tempalloc handle");
			return nullptr;
		}

		size_t real_size = ALIGN_UP(size, DEFAULT_TEMPALLOC_ALIGNMENT);

		if (temp_alloc->used_size + real_size > temp_alloc->total_size)
			return nullptr; // Not enough space

		size_t current_address = (size_t)temp_alloc->buffer + temp_alloc->used_size;
		temp_alloc->used_size += real_size;

		return (void*)current_address;
	}

	void temppool_reset(HANDLE temp_alloc_handle)
	{
		if (!temp_alloc_handle) return;

		tempalloc* temp_alloc = (tempalloc*)temp_alloc_handle;
		if (temp_alloc->magic != TEMPALLOC_MAGIC)
		{
			assert(false && "Invalid tempalloc handle");
			return;
		}

		temp_alloc->used_size = 0;
	}
}

