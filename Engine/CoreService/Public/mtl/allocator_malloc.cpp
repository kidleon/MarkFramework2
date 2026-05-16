#include "allocator_malloc.h"
#include <mimalloc/mimalloc.h>


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

	void sys_free(void* p)
	{
		mi_free(p);
	}
}
