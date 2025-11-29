#include "pch.h"
#include "fnv.h"


uint32 fnv1(
	const void* data,
	size_t bytes,
	uint32 initial
)
{
	const uint8* data8 = (const uint8*)data;
	const uint8* const data8end = data8 + bytes;

	while (data8 < data8end)
		initial = (initial * 16777619) ^ *data8++;

	return initial;
}

uint32 fnv1_c(
	const void* data,
	size_t bytes
)
{
	const uint8* data8 = (const uint8*)data;
	const uint8* const data8end = data8 + bytes;

	uint32 result = FNV1_INITIAL_VALUE;

	while (data8 < data8end)
		result = (result * 16777619) ^ *data8++;

	return result;
}


uint32 fnv1_cstr(
	const char* str,
	CHAR_CASE char_case,
	uint32 initial
)
{
	const uint8* pData = (const uint8*)str;

	uint32 c;

	switch (char_case)
	{
		case C_ANY:
		{
			while ((c = (uint8)*pData++) != 0)
				initial = (initial * 16777619) ^ c;
			break;
		}

		case C_LOWER:
		{
			while ((c = (uint8)*pData++) != 0)
				initial = (initial * 16777619) ^ _tolower((char)c);
			break;
		}

		case C_UPPER:
		{
			while ((c = (uint8)*pData++) != 0)
				initial = (initial * 16777619) ^ _toupper((char)c);
			break;
		}
	}

	return initial;
}

uint32 fnv1_cstr_c(
	const char* str,
	CHAR_CASE char_case
)
{
	const uint8* pData = (const uint8*)str;

	uint32 c;

	uint32 initial = FNV1_INITIAL_VALUE;

	switch (char_case)
	{
		case C_ANY:
		{
			while ((c = (uint8)*pData++) != 0)
				initial = (initial * 16777619) ^ c;
			break;
		}

		case C_LOWER:
		{
			while ((c = (uint8)*pData++) != 0)
				initial = (initial * 16777619) ^ _tolower((char)c);
			break;
		}

		case C_UPPER:
		{
			while ((c = (uint8)*pData++) != 0)
				initial = (initial * 16777619) ^ _toupper((char)c);
			break;
		}
	}

	return initial;
}

uint64 fnv64(
	const void* data,
	size_t bytes,
	uint64 initial
)
{
	const uint8* data8 = (const uint8*)data;
	const uint8* const data8end = data8 + bytes;

	while (data8 < data8end)
		initial = (initial * UINT64_C(1099511628211)) ^ *data8++;

	return initial;
}

uint64 fnv64_c(
	const void* data,
	size_t bytes
)
{
	const uint8* data8 = (const uint8*)data;
	const uint8* const data8end = data8 + bytes;

	uint64 initial = FNV64_INITIAL_VALUE;

	while (data8 < data8end)
		initial = (initial * UINT64_C(1099511628211)) ^ *data8++;

	return initial;
}

uint64 fnv64_cstr(
	const char* str,
	CHAR_CASE char_case,
	uint64 initial
)
{
	const uint8* pData = (const uint8*)str;

	uint64 c;

	switch (char_case)
	{
		case C_ANY:
		{
			while ((c = (uint8)*str++) != 0)
				initial = (initial * UINT64_C(1099511628211)) ^ c;
			break;
		}

		case C_LOWER:
		{
			while ((c = (uint8)*str++) != 0)
				initial = (initial * UINT64_C(1099511628211)) ^ _tolower((char)c);
			break;
		}

		case C_UPPER:
		{
			while ((c = (uint8)*str++) != 0)
				initial = (initial * UINT64_C(1099511628211)) ^ _toupper((char)c);
			break;
		}
	}

	return initial;
}

uint64 fnv64_cstr_c(
	const char* str,
	CHAR_CASE char_case
)
{
	const uint8* pData = (const uint8*)str;

	uint64 c;

	uint64 initial = FNV64_INITIAL_VALUE;

	switch (char_case)
	{
		case C_ANY:
		{
			while ((c = (uint8)*str++) != 0)
				initial = (initial * UINT64_C(1099511628211)) ^ c;
			break;
		}

		case C_LOWER:
		{
			while ((c = (uint8)*str++) != 0)
				initial = (initial * UINT64_C(1099511628211)) ^ _tolower((char)c);
			break;
		}

		case C_UPPER:
		{
			while ((c = (uint8)*str++) != 0)
				initial = (initial * UINT64_C(1099511628211)) ^ _toupper((char)c);
			break;
		}
	}

	return initial;
}