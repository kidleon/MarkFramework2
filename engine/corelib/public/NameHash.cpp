#include "pch.h"
#include "NameHash.h"

#if defined(_DEBUG) || defined(_DEBUG_)
#include "strings.h"
#endif // defined(_DEBUG) || defined(_DEBUG_)


NameHash::NameHash(const char* szName)
	: m_Hash(0)
{
	if (szName)
	{
		m_Hash = fnv1_cstr_c(szName, C_ANY);
#if defined(_DEBUG) || defined(_DEBUG_)
		fstrlcpy(m_szDebugName, szName, 124);
#endif // defined(_DEBUG) || defined(_DEBUG_)
	}
}
