#include <cstdint>
#include "predefine.h"


namespace mark
{
	struct resource_handle_impl
	{
		union
		{
			struct
			{
				uint64_t type : 8;
				uint64_t page_idx : 16;
				uint64_t index : 16;
				uint64_t version : 8;
			};
			uint64_t id;
		};

		constexpr static uint64_t invalid_handle = 0xffffffffu;
	};


	[[nodiscard]] inline uint8_t get_resource_type(resource_handle handle) noexcept
	{
		return (uint8_t)((resource_handle_impl)handle).type;
	}

	[[nodiscard]] inline int16_t get_resource_page_idx(resource_handle handle) noexcept
	{
		return (int16_t)((resource_handle_impl)handle).page_idx;
	}

	[[nodiscard]] inline int16_t get_resource_index(resource_handle handle) noexcept
	{
		return (int16_t)((resource_handle_impl)handle).index;
	}

	[[nodiscard]] inline uint8_t get_resource_version(resource_handle handle) noexcept
	{
		return (uint8_t)((resource_handle_impl)handle).version;
	}

	enum class RESOURCE_TYPE : uint8_t
	{
		PRIMITIVE_BUFFER = 0,
		MAX
	};

	enum class RESOURCE_STATS : uint8_t
	{
		EMPTY = 0,
		LOADING,
		UNLOADING,
		LOADED,

		MAX
	};

}
