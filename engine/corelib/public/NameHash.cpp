#include "pch.h"
#include "NameHash.h"


NameHash::NameHash(const char* szName)
	: m_Hash(0)
{
	if (szName)
	{
		m_Hash = fnv1_cstr_c(szName, C_ANY);
	}
}
