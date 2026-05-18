#include "pch.h"
#include "allocator.h"


namespace mtl
{
	namespace
	{
		mallocator g_default_allocator{ "default" };
	}

	mallocator& get_default_allocator() noexcept
	{
		return g_default_allocator;
	}
}
