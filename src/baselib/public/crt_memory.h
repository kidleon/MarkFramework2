#ifndef __CRT_MEMORY_H__
#define __CRT_MEMORY_H__


inline size_t compute_memory_alignment(
	size_t alloc_size,
	size_t alignment
) noexcept
{
	if (!alignment)
		return alloc_size;
	size_t padding = (alignment - (alloc_size & (alignment - 1))) & (alignment - 1);
	return alloc_size + padding;
}

inline void* crt_malloc_align(size_t bytes, size_t align)
{
#if defined(__TARGET_COMPILER_MSC)
	return _aligned_malloc(bytes, align);
#elif defined(__TARGET_COMPILER_GCC)
	return aligned_alloc(align, bytes); //C11
#endif
}

inline void* crt_realloc_align(void* ptr, size_t bytes, size_t align)
{
#if defined(__TARGET_COMPILER_MSC)
	return _aligned_realloc(ptr, bytes, align);
#elif defined(__TARGET_COMPILER_GCC)
	void* new_ptr = aligned_alloc(align, bytes); 
	if (!new_ptr) 
		return NULL;

	if (ptr) 
	{
		size_t old_size = malloc_usable_size(ptr); // GNU extension
		size_t copy_size = old_size < bytes ? old_size : bytes;
		memcpy(new_ptr, ptr, copy_size);
		free(ptr);
	}

	return new_ptr;
#endif
}

inline void crt_free_align(void* ptr)
{
#if defined(__TARGET_COMPILER_MSC)
	if (ptr)
		_aligned_free(ptr);
#else
	if (ptr)
		free(ptr);
#endif
}


#endif // __CRT_MEMORY_H__