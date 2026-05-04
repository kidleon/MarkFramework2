#pragma once
#include <string>
#include "hash.h"


namespace mark
{
	struct MARKENGINE_API name_hash
	{
	public:
		name_hash() = default;
		name_hash(const char* str)
		{
			hash_value = hash::fnv32(str, strlen(str));
		}

		[[nodiscard]] inline uint32_t get_hash() const { return hash_value; }

		inline bool operator==(const name_hash& other) const { return hash_value == other.hash_value; }
		inline bool operator!=(const name_hash& other) const { return hash_value != other.hash_value; }

		inline bool operator<(const name_hash& other) const { return hash_value < other.hash_value; }
		inline bool operator>(const name_hash& other) const { return hash_value > other.hash_value; }

	private:
		uint32_t hash_value = 0;

	};
}

template<>
struct std::hash<mark::name_hash>
{
	inline size_t operator()(const mark::name_hash& n) const noexcept
	{
		return n.get_hash();
	}
};
