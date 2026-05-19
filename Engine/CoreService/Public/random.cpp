#include "pch.h"
#include "random.h"


namespace mark
{
	namespace
	{
		constexpr uint64_t DEFAULT_SEED = 0x9E3779B97F4A7C15ULL;

		inline uint64_t xorshift64star(uint64_t& s) noexcept
		{
			s ^= s >> 12;
			s ^= s << 25;
			s ^= s >> 27;
			return s * 0x2545F4914F6CDD1DULL;
		}

		inline uint64_t mix_seed(uint64_t s) noexcept
		{
			uint64_t z = s + 0x9E3779B97F4A7C15ULL;
			z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
			z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
			return z ^ (z >> 31);
		}
	}

	random::random() noexcept
		: _state(DEFAULT_SEED)
	{
	}

	random::random(uint64_t s) noexcept
		: _state(s ? s : DEFAULT_SEED)
	{
	}

	void random::seed(uint64_t s)
	{
		_state = mix_seed(s);
		if (_state == 0)
			_state = DEFAULT_SEED;
	}

	int16_t random::next_int16()
	{
		return static_cast<int16_t>(xorshift64star(_state) & 0xFFFFu);
	}

	int16_t random::next_int16(int16_t min, int16_t max)
	{
		if (min >= max) return min;
		const uint32_t range = static_cast<uint32_t>(max - min) + 1u;
		const uint32_t r = static_cast<uint32_t>(xorshift64star(_state)) % range;
		return static_cast<int16_t>(min + static_cast<int16_t>(r));
	}

	uint16_t random::next_uint16()
	{
		return static_cast<uint16_t>(xorshift64star(_state) & 0xFFFFu);
	}

	uint16_t random::next_uint16(uint16_t min, uint16_t max)
	{
		if (min >= max) return min;
		const uint32_t range = static_cast<uint32_t>(max - min) + 1u;
		const uint32_t r = static_cast<uint32_t>(xorshift64star(_state)) % range;
		return static_cast<uint16_t>(min + r);
	}

	int32_t random::next_int32()
	{
		return static_cast<int32_t>(xorshift64star(_state) & 0xFFFFFFFFu);
	}

	int32_t random::next_int32(int min, int max)
	{
		if (min >= max) return min;
		const uint64_t range = static_cast<uint64_t>(static_cast<int64_t>(max) - static_cast<int64_t>(min)) + 1ull;
		const uint64_t r = xorshift64star(_state) % range;
		return static_cast<int32_t>(static_cast<int64_t>(min) + static_cast<int64_t>(r));
	}

	uint32_t random::next_uint32()
	{
		return static_cast<uint32_t>(xorshift64star(_state) & 0xFFFFFFFFu);
	}

	uint32_t random::next_uint32(uint32_t min, uint32_t max)
	{
		if (min >= max) return min;
		const uint64_t range = static_cast<uint64_t>(max - min) + 1ull;
		const uint64_t r = xorshift64star(_state) % range;
		return min + static_cast<uint32_t>(r);
	}

	int64_t random::next_int64()
	{
		return static_cast<int64_t>(xorshift64star(_state));
	}

	int64_t random::next_int64(int64_t min, int64_t max)
	{
		if (min >= max) return min;
		const uint64_t range = static_cast<uint64_t>(max - min) + 1ull;
		uint64_t r;
		if (range == 0)
		{
			r = xorshift64star(_state);
		}
		else
		{
			r = xorshift64star(_state) % range;
		}
		return static_cast<int64_t>(static_cast<uint64_t>(min) + r);
	}

	uint64_t random::next_uint64()
	{
		return xorshift64star(_state);
	}

	uint64_t random::next_uint64(uint64_t min, uint64_t max)
	{
		if (min >= max) return min;
		const uint64_t span = max - min;
		uint64_t r;
		if (span == UINT64_MAX)
		{
			r = xorshift64star(_state);
		}
		else
		{
			r = xorshift64star(_state) % (span + 1ull);
		}
		return min + r;
	}

	float random::next_float()
	{
		const uint32_t v = static_cast<uint32_t>(xorshift64star(_state) >> 32);
		return static_cast<float>(v >> 8) * (1.0f / 16777216.0f);
	}

	float random::next_float(float min, float max)
	{
		if (min >= max) return min;
		return min + next_float() * (max - min);
	}
}
