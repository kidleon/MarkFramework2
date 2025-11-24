#include "pch.h"
#include "fnv.h"


uint32 fnv1(
	const void* pData,
	size_t Bytes,
	uint32 Initial
)
{
	const uint8* data8 = (const uint8*)pData;
	const uint8* const data8end = data8 + Bytes;

	while (data8 < data8end)
		Initial = (Initial * 16777619) ^ *data8++;

	return Initial;
}

uint32 fnv1_cstr(
	const char* szStr,
	CHAR_CASE Case,
	uint32 Initial
)
{
	const uint8* pData = (const uint8*)szStr;

	uint32 c;

	switch (Case)
	{
	case C_ANY:
	{
		while ((c = (uint8)*pData++) != 0)
			Initial = (Initial * 16777619) ^ c;
		break;
	}

	case C_LOWER:
	{
		while ((c = (uint8)*pData++) != 0)
			Initial = (Initial * 16777619) ^ _tolower((char)c);
		break;
	}

	case C_UPPER:
	{
		while ((c = (uint8)*pData++) != 0)
			Initial = (Initial * 16777619) ^ _toupper((char)c);
		break;
	}
	}

	return Initial;
}

uint64 fnv64(
	const void* pData,
	size_t Bytes,
	uint64 Initial
)
{
	const uint8* data8 = (const uint8*)pData;
	const uint8* const data8end = data8 + Bytes;

	while (data8 < data8end)
		Initial = (Initial * UINT64_C(1099511628211)) ^ *data8++;

	return Initial;
}

uint64 fnv64_cstr(
	const char* szStr,
	CHAR_CASE Case,
	uint64 Initial
)
{
	const uint8* pData = (const uint8*)szStr;

	uint64 c;

	switch (Case)
	{
		case C_ANY:
		{
			while ((c = (uint8)*pData++) != 0)
				Initial = (Initial * UINT64_C(1099511628211)) ^ c;
			break;
		}

		case C_LOWER:
		{
			while ((c = (uint8)*pData++) != 0)
				Initial = (Initial * UINT64_C(1099511628211)) ^ _tolower((char)c);
			break;
		}

		case C_UPPER:
		{
			while ((c = (uint8)*pData++) != 0)
				Initial = (Initial * UINT64_C(1099511628211)) ^ _toupper((char)c);
			break;
		}
	}

	return Initial;
}