#pragma once
#include "internal.h"


namespace mtl
{
	MTL_API void* sys_malloc(size_t n);

	MTL_API void* sys_malloc(size_t n, size_t alignment);

	MTL_API void* sys_malloc(size_t n, size_t alignment, size_t offset);

	MTL_API void sys_free(void* p);
}


