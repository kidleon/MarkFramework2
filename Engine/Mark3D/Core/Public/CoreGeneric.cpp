#include "pch.h"
#include "CoreGeneric.h"
#include "PrivateMemory.h"


namespace mark
{
	std::pmr::memory_resource* get_core_system_memory_resource_ptr() noexcept
	{
		return get_default_system_memory_resource_ptr();
	}

	std::pmr::memory_resource* get_core_sync_pool_memory_resource_ptr() noexcept
	{
		return get_default_sync_pool_memory_resource_ptr();
	}

	std::pmr::memory_resource* get_core_usync_pool_memory_resource_ptr() noexcept
	{
		return get_default_usync_pool_memory_resource_ptr();
	}

	std::pmr::memory_resource* get_core_temp_memory_resource_ptr() noexcept
	{
		return get_default_temp_memory_resource_ptr();
	}
}
