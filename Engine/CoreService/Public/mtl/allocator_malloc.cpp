#include "pch.h"
#include "allocator_malloc.h"
#include <mimalloc/mimalloc.h>
#include "memory_tracker.h"



namespace mtl
{
	void* sys_malloc(size_t n)
	{
		return mi_malloc(n);
	}

	void* sys_malloc(size_t n, size_t alignment)
	{
		if (alignment <= alignof(std::max_align_t))
			return mi_malloc(n);
		return mi_malloc_aligned(n, alignment);
	}

	void* sys_malloc(size_t n, size_t alignment, size_t offset)
	{
		if (alignment <= alignof(std::max_align_t) && offset == 0)
			return mi_malloc(n);

		if (offset == 0)
			return mi_malloc_aligned(n, alignment);

		return mi_malloc_aligned_at(n, alignment, offset);
	}

	void* sys_malloc(size_t n, std::source_location location)
	{
		void* ptr = mi_malloc(n);

#if defined(__MEMORY_TRACKER_ENABLED__)
		mark::memory_tracker::get().on_allocate(ptr, n, alignof(std::max_align_t), location);
#endif // __MEMORY_TRACKER_ENABLED__

		return ptr;
	}

	void* sys_malloc(size_t n, size_t alignment, std::source_location location)
	{
		void* ptr = nullptr;

		if (alignment <= alignof(std::max_align_t))
			ptr = mi_malloc(n);
		else
			ptr = mi_malloc_aligned(n, alignment);

#if defined(__MEMORY_TRACKER_ENABLED__)
		mark::memory_tracker::get().on_allocate(ptr, n, alignment, location);
#endif // __MEMORY_TRACKER_ENABLED__

		return ptr;
	}

	void* sys_malloc(size_t n, size_t alignment, size_t offset, std::source_location location)
	{
		void* ptr = nullptr;

		if (alignment <= alignof(std::max_align_t) && offset == 0)
			ptr = mi_malloc(n);
		else if (offset == 0)
			ptr = mi_malloc_aligned(n, alignment);
		else
			ptr = mi_malloc_aligned_at(n, alignment, offset);

#if defined(__MEMORY_TRACKER_ENABLED__)
		mark::memory_tracker::get().on_allocate(ptr, n, alignment, location);
#endif // __MEMORY_TRACKER_ENABLED__

		return ptr;
	}

	void sys_free(void* p)
	{
#if defined(__MEMORY_TRACKER_ENABLED__)
		mark::memory_tracker::get().on_deallocate(p);
#endif // __MEMORY_TRACKER_ENABLED__

		mi_free(p);
	}
}
