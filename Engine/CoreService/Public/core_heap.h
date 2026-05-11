#pragma once
#include <source_location>


namespace mark
{
	MARKENGINE_API bool initialize_core_memory(size_t default_temp_size);
	MARKENGINE_API void shutdown_core_memory();

#if defined(__MEMORY_TRACKER_ENABLED__)
	[[nodiscard]] MARKENGINE_API void* sys_alloc(size_t bytes, std::source_location loc = std::source_location::current());
	[[nodiscard]] MARKENGINE_API void* sys_alloc(size_t bytes, size_t alignment, std::source_location loc = std::source_location::current());
	[[nodiscard]] MARKENGINE_API void* sys_calloc(size_t bytes, std::source_location loc = std::source_location::current());
	[[nodiscard]] MARKENGINE_API void* sys_calloc(size_t bytes, size_t alignment, std::source_location loc = std::source_location::current());
	[[nodiscard]] MARKENGINE_API void* sys_realloc(void* ptr, size_t new_bytes, std::source_location loc = std::source_location::current());
	[[nodiscard]] MARKENGINE_API void* sys_realloc(void* ptr, size_t new_bytes, size_t alignment, std::source_location loc = std::source_location::current());
#else
	[[nodiscard]] MARKENGINE_API void* sys_alloc(size_t bytes);
	[[nodiscard]] MARKENGINE_API void* sys_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] MARKENGINE_API void* sys_calloc(size_t bytes);
	[[nodiscard]] MARKENGINE_API void* sys_calloc(size_t bytes, size_t alignment);
	[[nodiscard]] MARKENGINE_API void* sys_realloc(void* ptr, size_t new_bytes);
	[[nodiscard]] MARKENGINE_API void* sys_realloc(void* ptr, size_t new_bytes, size_t alignment);
#endif // __MEMORY_TRACKER_ENABLED__

	MARKENGINE_API void sys_free(void* ptr);

	[[nodiscard]] MARKENGINE_API void* temp_alloc(size_t bytes);
	[[nodiscard]] MARKENGINE_API void* temp_alloc(size_t bytes, size_t alignment);
	[[nodiscard]] MARKENGINE_API void* temp_calloc(size_t bytes);
	[[nodiscard]] MARKENGINE_API void* temp_calloc(size_t bytes, size_t alignment);
	MARKENGINE_API void temp_reset();

	[[nodiscard]] MARKENGINE_API HANDLE temppool_create(size_t buffer_size);
	MARKENGINE_API void temppool_destroy(HANDLE temp_handle);

	[[nodiscard]] MARKENGINE_API void* temppool_alloc(HANDLE temp_handle, size_t bytes);
	[[nodiscard]] MARKENGINE_API void* temppool_alloc(HANDLE temp_handle, size_t bytes, size_t alignment);
	[[nodiscard]] MARKENGINE_API void* temppool_calloc(HANDLE temp_handle, size_t bytes);
	[[nodiscard]] MARKENGINE_API void* temppool_calloc(HANDLE temp_handle, size_t bytes, size_t alignment);
	MARKENGINE_API void temppool_reset(HANDLE temp_handle);
}

#if defined(__MEMORY_TRACKER_ENABLED__)
#define CORE_SYS_ALLOC(bytes)						::mark::sys_alloc(bytes, std::source_location::current())
#define CORE_SYS_ALLOC_ALIGNED(bytes, alignment)	::mark::sys_alloc(bytes, alignment, std::source_location::current())
#define CORE_SYS_CALLOC(bytes)						::mark::sys_calloc(bytes, std::source_location::current())
#define CORE_SYS_CALLOC_ALIGNED(bytes, alignment)	::mark::sys_calloc(bytes, alignment, std::source_location::current())
#define CORE_SYS_REALLOC(ptr, new_bytes)			::mark::sys_realloc(ptr, new_bytes, std::source_location::current())
#define CORE_SYS_REALLOC_ALIGNED(ptr, new_bytes, alignment)	::mark::sys_realloc(ptr, new_bytes, alignment, std::source_location::current())
#else
#define CORE_SYS_ALLOC(bytes)						::mark::sys_alloc(bytes)
#define CORE_SYS_ALLOC_ALIGNED(bytes, alignment)	::mark::sys_alloc(bytes, alignment)
#define CORE_SYS_CALLOC(bytes)						::mark::sys_calloc(bytes)
#define CORE_SYS_CALLOC_ALIGNED(bytes, alignment)	::mark::sys_calloc(bytes, alignment)
#define CORE_SYS_REALLOC(ptr, new_bytes)			::mark::sys_realloc(ptr, new_bytes)
#define CORE_SYS_REALLOC_ALIGNED(ptr, new_bytes, alignment)	::mark::sys_realloc(ptr, new_bytes, alignment)
#endif // __MEMORY_TRACKER_ENABLED__
#define CORE_SYS_FREE(ptr)							if(ptr) { ::mark::sys_free(ptr); ptr = nullptr; }
#define CORE_TEMP_ALLOC(bytes)						::mark::temp_alloc(bytes)
#define CORE_TEMP_ALLOC_ALIGNED(bytes, alignment)	::mark::temp_alloc(bytes, alignment)
#define CORE_TEMP_CALLOC(bytes)						::mark::temp_calloc(bytes)
#define CORE_TEMP_CALLOC_ALIGNED(bytes, alignment)	::mark::temp_calloc(bytes, alignment)
#define CORE_TEMP_RESET()							::mark::temp_reset()

#define CORE_NEW(T) 								new (CORE_SYS_ALLOC(sizeof(T))) T
#define CORE_NEW_ALIGNED(T, A)						new (CORE_SYS_ALLOC_ALIGNED(sizeof(T), A)) T
#define CORE_NEW_ARRAY(T, Count)					new (CORE_SYS_ALLOC(sizeof(T) * (Count))) T[Count]
#define CORE_NEW_ARRAY_ALIGNED(T, A, Count)			new (CORE_SYS_ALLOC_ALIGNED(sizeof(T) * (Count), A)) T[Count]
#define CORE_DELETE(T, Ptr)							{ T* p = Ptr; if(p){ (p)->~T(); sys_free((void*)p); } }
#define CORE_DELETE_ARRAY(T, Ptr, Count) \
{ \
    T* p = Ptr; \
    if(p) \
    { \
        /* 역순 소멸 — new[]와 대칭을 맞추기 위함 */ \
        for(size_t i = Count; i > 0; --i) \
        {\
            p[i-1].~T();\
        }\
        ::mark::sys_free((void*)p); \
    }\
}



