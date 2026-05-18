#pragma once
#include <source_location>


namespace mtl
{
	MARKENGINE_API void* sys_malloc(size_t n);

	MARKENGINE_API void* sys_malloc(size_t n, size_t alignment);

	MARKENGINE_API void* sys_malloc(size_t n, size_t alignment, size_t offset);

	MARKENGINE_API void* sys_malloc(size_t n, std::source_location location);

	MARKENGINE_API void* sys_malloc(size_t n, size_t alignment, std::source_location location);

	MARKENGINE_API void* sys_malloc(size_t n, size_t alignment, size_t offset, std::source_location location);

	MARKENGINE_API void sys_free(void* p);
}


