#ifndef __CRT_MEMORY_H__
#define __CRT_MEMORY_H__


static inline void* crt_malloc(size_t bytes)
{
	return malloc(bytes);
}

static inline void* crt_realloc(void* ptr, size_t bytes)
{
	return realloc(ptr, bytes);
}

static inline void crt_free(void* ptr)
{
	if (ptr)
		free(ptr);
}

static inline void* crt_malloc_align(size_t bytes, size_t align)
{
#if defined(__TARGET_COMPILER_MSC)
	return _aligned_malloc(bytes, align);
#elif defined(__TARGET_COMPILER_GCC || __TARGET_COMPILER_CLANG)
	return aligned_alloc(align, bytes); //C11
#endif
}

/*
* aligned된 메모리 재할당은 Linux의 경우 직접 구현해야 한다. 
* malloc_usable_size os api는 메모리 정렬에 대한 사이즈까지 계산하지 않을수 있어서 안정성 문제로 일단 보류
static inline void* crt_realloc_align(void* ptr, size_t bytes, size_t align)
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
*/

static inline void crt_free_align(void* ptr)
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