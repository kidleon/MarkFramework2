#include "pch.h"
#include "strings.h"
#include "ctypes.h"


size_t fstrlen(
	const char* str
)
{
	size_t len = (size_t)-1; // EABase 1.0.14 and later recognize ssize_t for all platforms.

	do
	{
		++len;
	} while (*str++);

	return (size_t)len;
}

size_t fstrlen(
	const wchar_t* str
)
{
	size_t len = (size_t)-1; // EABase 1.0.14 and later recognize ssize_t for all platforms.

	do
	{
		++len;
	} while (*str++);

	return (size_t)len;
}

size_t fstrlen(
	const char16_t* str
)
{
	size_t len = (size_t)-1; // EABase 1.0.14 and later recognize ssize_t for all platforms.

	do
	{
		++len;
	} while (*str++);

	return (size_t)len;
}

size_t fstrlen(
	const char32_t* str
)
{
	size_t len = (size_t)-1; // EABase 1.0.14 and later recognize ssize_t for all platforms.

	do
	{
		++len;
	} while (*str++);

	return (size_t)len;
}


char* fstrcpy(
	char* dest,
	const char* src
)
{
	const char* s = src;
	char* d = dest;

	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

wchar_t* fstrcpy(
	wchar_t* dest,
	const wchar_t* src
)
{
	const wchar_t* s = src;
	wchar_t* d = dest;

	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

char16_t* fstrcpy(
	char16_t* dest, 
	const char16_t* src
)
{
	const char16_t* s = src;
	char16_t* d = dest;

	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

char32_t* fstrcpy(
	char32_t* dest,
	const char32_t* src
)
{
	const char32_t* s = src;
	char32_t* d = dest;

	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

char* fstrncpy(
	char* dest, 
	const char* src,
	size_t n
)
{
	const char* s = src;
	char* d = dest;

	n++;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			while (--n)
				*d++ = 0;
			break;
		}
	}

	return dest;
}

wchar_t* fstrncpy(
	wchar_t* dest,
	const wchar_t* src,
	size_t n
)
{
	const wchar_t* s = src;
	wchar_t* d = dest;

	n++;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			while (--n)
				*d++ = 0;
			break;
		}
	}

	return dest;
}

char16_t* fstrncpy(
	char16_t* dest,
	const char16_t* src,
	size_t n
)
{
	const char16_t* s = src;
	char16_t* d = dest;

	n++;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			while (--n)
				*d++ = 0;
			break;
		}
	}

	return dest;
}

char32_t* fstrncpy(
	char32_t* dest, 
	const char32_t* src, 
	size_t n
)
{
	const char32_t* s = src;
	char32_t* d = dest;

	n++;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			while (--n)
				*d++ = 0;
			break;
		}
	}

	return dest;
}

size_t fstrlcpy(
	char* dest, 
	const char* src, 
	size_t dest_size
)
{
	const char* s = src;
	size_t n = dest_size;

	if (n && --n)
	{
		do {
			if ((*dest++ = *s++) == 0)
				break;
		} while (--n);
	}

	if (!n)
	{
		if (dest_size)
			*dest = 0;
		while (*s++)
		{
		}
	}

	return (size_t)(s - src - 1);
}

size_t fstrlcpy(
	wchar_t* dest, 
	const wchar_t* src, 
	size_t dest_size
)
{
	const wchar_t* s = src;
	size_t n = dest_size;

	if (n && --n)
	{
		do {
			if ((*dest++ = *s++) == 0)
				break;
		} while (--n);
	}

	if (!n)
	{
		if (dest_size)
			*dest = 0;
		while (*s++)
		{
		}
	}

	return (size_t)(s - src - 1);
}

size_t fstrlcpy(
	char16_t* dest, 
	const char16_t* src,
	size_t dest_size
)
{
	const char16_t* s = src;
	size_t n = dest_size;

	if (n && --n)
	{
		do {
			if ((*dest++ = *s++) == 0)
				break;
		} while (--n);
	}

	if (!n)
	{
		if (dest_size)
			*dest = 0;
		while (*s++)
		{
		}
	}

	return (size_t)(s - src - 1);
}

size_t fstrlcpy(
	char32_t* dest, 
	const char32_t* src,
	size_t dest_size
)
{
	const char32_t* s = src;
	size_t n = dest_size;

	if (n && --n)
	{
		do {
			if ((*dest++ = *s++) == 0)
				break;
		} while (--n);
	}

	if (!n)
	{
		if (dest_size)
			*dest = 0;
		while (*s++)
		{
		}
	}

	return (size_t)(s - src - 1);
}


char* fstrcat(
	char* dest, 
	const char* src
)
{
	const char* s = src;
	char* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

wchar_t* fstrcat(
	wchar_t* dest, 
	const wchar_t* src
)
{
	const wchar_t* s = src;
	wchar_t* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

char16_t* fstrcat(
	char16_t* dest,
	const char16_t* src
)
{
	const char16_t* s = src;
	char16_t* d = dest;

	--d;
	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

char32_t* fstrcat(
	char32_t* dest, 
	const char32_t* src
)
{
	const char32_t* s = src;
	char32_t* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	while ((*d++ = *s++) != 0)
	{
	} // Do nothing.

	return dest;
}

char* fstrncat(
	char* dest, 
	const char* src,
	size_t n
)
{
	const char* s = src;
	char* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	++n;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			--d;
			break;
		}
	}
	*d = 0;

	return dest;
}

wchar_t* fstrncat(
	wchar_t* dest,
	const wchar_t* src, 
	size_t n
)
{
	const wchar_t* s = src;
	wchar_t* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	++n;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			--d;
			break;
		}
	}
	*d = 0;

	return dest;
}

char16_t* fstrncat(
	char16_t* dest, 
	const char16_t* src,
	size_t n
)
{
	const char16_t* s = src;
	char16_t* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	++n;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			--d;
			break;
		}
	}
	*d = 0;

	return dest;
}

char32_t* fstrncat(
	char32_t* dest, 
	const char32_t* src,
	size_t n
)
{
	const char32_t* s = src;
	char32_t* d = dest;

	while (*d++) {} // Do nothing.
	--d;
	++n;
	while (--n)
	{
		if ((*d++ = *s++) == 0)
		{
			--d;
			break;
		}
	}
	*d = 0;

	return dest;
}

size_t fstrlcat(
	char* dest,
	const char* src, 
	size_t dest_size
)
{
	const size_t d = dest_size ? fstrlen(dest) : 0;
	const size_t s = fstrlen(src);
	const size_t t = s + d;

	//	assert((dest_size == 0) || (d < dest_size), "Destination string is longer than the specified capacity! "
				//	  "Either an out of bounds write has occurred previous to this call or the specified capacity is incorrect.");

	if (t < dest_size)
		memcpy(dest + d, src, (s + 1) * sizeof(*src));
	else
	{
		if (dest_size)
		{
			memcpy(dest + d, src, ((dest_size - d) - 1) * sizeof(*src));
			dest[dest_size - 1] = 0;
		}
	}

	return t;
}

size_t fstrlcat(
	wchar_t* dest,
	const wchar_t* src,
	size_t dest_size
)
{
	const size_t d = dest_size ? fstrlen(dest) : 0;
	const size_t s = fstrlen(src);
	const size_t t = s + d;

	//	assert((dest_size == 0) || (d < dest_size), "Destination string is longer than the specified capacity! "
			//   "Either an out of bounds write has occurred previous to this call or the specified capacity is incorrect.");

	if (t < dest_size)
		memcpy(dest + d, src, (s + 1) * sizeof(*src));
	else
	{
		if (dest_size)
		{
			memcpy(dest + d, src, ((dest_size - d) - 1) * sizeof(*src));
			dest[dest_size - 1] = 0;
		}
	}

	return t;
}

size_t fstrlcat(
	char16_t* dest, 
	const char16_t* src, 
	size_t dest_size
)
{
	const size_t d = dest_size ? fstrlen(dest) : 0;
	const size_t s = fstrlen(src);
	const size_t t = s + d;

	//	assert((dest_size == 0) || (d < dest_size), "Destination string is longer than the specified capacity! "
//"Either an out of bounds write has occurred previous to this call or the specified capacity is incorrect.");

	if (t < dest_size)
		memcpy(dest + d, src, (s + 1) * sizeof(*src));
	else
	{
		if (dest_size)
		{
			memcpy(dest + d, src, ((dest_size - d) - 1) * sizeof(*src));
			dest[dest_size - 1] = 0;
		}
	}

	return t;
}

size_t fstrlcat(
	char32_t* dest, 
	const char32_t* src,
	size_t dest_size
)
{
	const size_t d = dest_size ? fstrlen(dest) : 0;
	const size_t s = fstrlen(src);
	const size_t t = s + d;

	//	assert((dest_size == 0) || (d < dest_size), "Destination string is longer than the specified capacity! "
	//"Either an out of bounds write has occurred previous to this call or the specified capacity is incorrect.");

	if (t < dest_size)
		memcpy(dest + d, src, (s + 1) * sizeof(*src));
	else
	{
		if (dest_size)
		{
			memcpy(dest + d, src, ((dest_size - d) - 1) * sizeof(*src));
			dest[dest_size - 1] = 0;
		}
	}

	return t;
}

char* fstrupr(char* s)
{
	char* t = s;

	while (*t)
	{
		if ((uint8_t)*t <= 127)
			*t = (char)__toupper(*t);
		++t;
	}

	return s;
}

wchar_t* fstrupr(wchar_t* s)
{
	wchar_t* t = s;

	while (*t)
	{
		const wchar_t c = *t;
		*t++ = __toupper(c);
	}

	return s;
}

char16_t* fstrupr(char16_t* s)
{
	char16_t* t = s;

	while (*t)
	{
		const char16_t c = *t;
		*t++ = __toupper(c);
	}

	return s;
}

char32_t* fstrupr(char32_t* s)
{
	char32_t* t = s;

	while (*t)
	{
		const char32_t c = *t;
		*t++ = __toupper(c);
	}

	return s;
}


char* fstrlwr(char* s)
{
	char* t = s;

	while (*t)
	{
		if ((uint8_t)*t <= 127)
			*t = (char)__tolower(*t);
		++t;
	}

	return s;
}

wchar_t* fstrlwr(wchar_t* s)
{
	wchar_t* t = s;

	while (*t)
	{
		const wchar_t c = *t;
		*t++ = __tolower(c);
	}

	return s;
}

char16_t* fstrlwr(char16_t* s)
{
	char16_t* t = s;

	while (*t)
	{
		const char16_t c = *t;
		*t++ = __tolower(c);
	}

	return s;
}

char32_t* fstrlwr(char32_t* s)
{
	char32_t* t = s;

	while (*t)
	{
		const char32_t c = *t;
		*t++ = __tolower(c);
	}

	return s;
}

char* fstrchr(
	const char* src, 
	char c
)
{
	do
	{
		if (*src == c)
			return (char*)src;
	} while (*src++);

	return nullptr;
}

wchar_t* fstrchr(
	const wchar_t* src,
	wchar_t c
)
{
	do
	{
		if (*src == c)
			return (wchar_t*)src;
	} while (*src++);

	return nullptr;
}

char16_t* fstrchr(
	const char16_t* src, 
	char16_t c
)
{
	do
	{
		if (*src == c)
			return (char16_t*)src;
	} while (*src++);

	return nullptr;
}

char32_t* fstrchr(
	const char32_t* src, 
	char32_t c
)
{
	do
	{
		if (*src == c)
			return (char32_t*)src;
	} while (*src++);

	return nullptr;
}

char* fstrnchr(
	const char* src, 
	char c, 
	size_t n
)
{
	while (n-- > 0)
	{
		if (*src == c)
		{
			return (char*)src;
		}
		if (*src == '\0')
		{
			return nullptr;
		}
		src++;
	}
	return nullptr;
}

wchar_t* fstrnchr(
	const wchar_t* src, 
	wchar_t c, 
	size_t n
)
{
	while (n-- > 0)
	{
		if (*src == c)
		{
			return (wchar_t*)src;
		}
		if (*src == '\0')
		{
			return nullptr;
		}
		src++;
	}
	return nullptr;
}

char16_t* fstrnchr(
	const char16_t* src, 
	char16_t c, 
	size_t n
)
{
	while (n-- > 0)
	{
		if (*src == c)
		{
			return (char16_t*)src;
		}
		if (*src == '\0')
		{
			return nullptr;
		}
		src++;
	}
	return nullptr;
}

char32_t* fstrnchr(
	const char32_t* src, 
	char32_t c,
	size_t n
)
{
	while (n-- > 0)
	{
		if (*src == c)
		{
			return (char32_t*)src;
		}
		if (*src == '\0')
		{
			return nullptr;
		}
		src++;
	}
	return nullptr;
}


size_t fstrcspn(
	const char* str1, 
	const char* str2
)
{
	const char* cur = str1;

	while (*cur)
	{
		for (const char* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *cur)
				return (size_t)(cur - str1);
		}

		++cur;
	}

	return (size_t)(cur - str1);
}

size_t fstrcspn(
	const wchar_t* str1, 
	const wchar_t* str2
)
{
	const wchar_t* cur = str1;

	while (*cur)
	{
		for (const wchar_t* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *cur)
				return (size_t)(cur - str1);
		}

		++cur;
	}

	return (size_t)(cur - str1);
}

size_t fstrcspn(
	const char16_t* str1, 
	const char16_t* str2
)
{
	const char16_t* cur = str1;

	while (*cur)
	{
		for (const char16_t* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *cur)
				return (size_t)(cur - str1);
		}

		++cur;
	}

	return (size_t)(cur - str1);
}

size_t fstrcspn(
	const char32_t* str1, 
	const char32_t* str2
)
{
	const char32_t* cur = str1;

	while (*cur)
	{
		for (const char32_t* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *cur)
				return (size_t)(cur - str1);
		}

		++cur;
	}

	return (size_t)(cur - str1);
}


char* fstrpbrk(
	const char* str1,
	const char* str2
)
{
	while (*str1)
	{
		for (const char* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *str1)
				return (char*)str1;
		}

		++str1;
	}

	return nullptr;
}

wchar_t* fstrpbrk(
	const wchar_t* str1, 
	const wchar_t* str2
)
{
	while (*str1)
	{
		for (const wchar_t* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *str1)
				return (wchar_t*)str1;
		}

		++str1;
	}

	return nullptr;
}

char16_t* fstrpbrk(
	const char16_t* str1, 
	const char16_t* str2
)
{
	while (*str1)
	{
		for (const char16_t* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *str1)
				return (char16_t*)str1;
		}

		++str1;
	}

	return nullptr;
}

char32_t* fstrpbrk(
	const char32_t* str1, 
	const char32_t* str2
)
{
	while (*str1)
	{
		for (const char32_t* pCharSet = str2; *pCharSet; ++pCharSet)
		{
			if (*pCharSet == *str1)
				return (char32_t*)str1;
		}

		++str1;
	}

	return nullptr;
}

char* fstrrchr(
	const char* src,
	char c
)
{
	const char* found = nullptr;
	char curr;

	while ((curr = *src++) != 0)
	{
		if (curr == c)
			found = (src - 1);
	}

	if (found)
		return (char*)found;

	return c ? nullptr : (char*)(src - 1);
}

wchar_t* fstrrchr(
	const wchar_t* src, 
	wchar_t c
)
{
	const wchar_t* found = nullptr;
	wchar_t curr;

	while ((curr = *src++) != 0)
	{
		if (curr == c)
			found = (src - 1);
	}

	if (found)
		return (wchar_t*)found;

	return c ? nullptr : (wchar_t*)(src - 1);
}

char16_t* fstrrchr(
	const char16_t* src, 
	char16_t c
)
{
	const char16_t* found = nullptr;
	char16_t curr;

	while ((curr = *src++) != 0)
	{
		if (curr == c)
			found = (src - 1);
	}

	if (found)
		return (char16_t*)found;

	return c ? nullptr : (char16_t*)(src - 1);
}

char32_t* fstrrchr(
	const char32_t* src,
	char32_t c
)
{
	const char32_t* found = nullptr;
	char32_t curr;

	while ((curr = *src++) != 0)
	{
		if (curr == c)
			found = (src - 1);
	}

	if (found)
		return (char32_t*)found;

	return c ? nullptr : (char32_t*)(src - 1);
}

size_t fstrspn(
	const char* str1, 
	const char* str2
)
{
	const char* curr = str1;

	while (*curr)
	{
		for (const char* str2_curr = str2; *str2_curr != *curr; ++str2_curr)
		{
			if (*str2_curr == 0)
				return (size_t)(curr - str1);
		}

		++curr;
	}

	return (size_t)(curr - str1);
}

size_t fstrspn(
	const wchar_t* str1, 
	const wchar_t* str2
)
{
	const wchar_t* curr = str1;

	while (*curr)
	{
		for (const wchar_t* str2_curr = str2; *str2_curr != *curr; ++str2_curr)
		{
			if (*str2_curr == 0)
				return (size_t)(curr - str1);
		}

		++curr;
	}

	return (size_t)(curr - str1);
}

size_t fstrspn(
	const char16_t* str1, 
	const char16_t* str2
)
{
	const char16_t* curr = str1;

	while (*curr)
	{
		for (const char16_t* str2_curr = str2; *str2_curr != *curr; ++str2_curr)
		{
			if (*str2_curr == 0)
				return (size_t)(curr - str1);
		}

		++curr;
	}

	return (size_t)(curr - str1);
}

size_t fstrspn(
	const char32_t* str1, 
	const char32_t* str2
)
{
	const char32_t* curr = str1;

	while (*curr)
	{
		for (const char32_t* str2_curr = str2; *str2_curr != *curr; ++str2_curr)
		{
			if (*str2_curr == 0)
				return (size_t)(curr - str1);
		}

		++curr;
	}

	return (size_t)(curr - str1);
}

char* fstrstr(
	const char* src,
	const char* sub
)
{
	char* s1 = (char*)src - 1;
	char* p1 = (char*)sub - 1;
	char  c0, c1, c2;

	if ((c0 = *++p1) == 0)   // An empty pSubString results in success, return pString.
		return (char*)src;

	while ((c1 = *++s1) != 0)
	{
		if (c1 == c0)
		{
			const char* s2 = (s1 - 1);
			const char* p2 = (p1 - 1);

			while ((c1 = *++s2) == (c2 = *++p2) && c1) {} // Do nothing

			if (!c2)
				return (char*)s1;
		}
	}

	return nullptr;
}

wchar_t* fstrstr(
	const wchar_t* src, 
	const wchar_t* sub
)
{
	wchar_t* s1 = (wchar_t*)src - 1;
	wchar_t* p1 = (wchar_t*)sub - 1;
	wchar_t  c0, c1, c2;

	if ((c0 = *++p1) == 0)   // An empty pSubString results in success, return pString.
		return (wchar_t*)src;

	while ((c1 = *++s1) != 0)
	{
		if (c1 == c0)
		{
			const wchar_t* s2 = (s1 - 1);
			const wchar_t* p2 = (p1 - 1);

			while ((c1 = *++s2) == (c2 = *++p2) && c1) {} // Do nothing

			if (!c2)
				return (wchar_t*)s1;
		}
	}

	return nullptr;
}

char16_t* fstrstr(
	const char16_t* src,
	const char16_t* sub
)
{
	char16_t* s1 = (char16_t*)src - 1;
	char16_t* p1 = (char16_t*)sub - 1;
	char16_t  c0, c1, c2;

	if ((c0 = *++p1) == 0)   // An empty pSubString results in success, return pString.
		return (char16_t*)src;

	while ((c1 = *++s1) != 0)
	{
		if (c1 == c0)
		{
			const char16_t* s2 = (s1 - 1);
			const char16_t* p2 = (p1 - 1);

			while ((c1 = *++s2) == (c2 = *++p2) && c1) {} // Do nothing

			if (!c2)
				return (char16_t*)s1;
		}
	}

	return nullptr;
}

char32_t* fstrstr(
	const char32_t* src, 
	const char32_t* sub
)
{
	char32_t* s1 = (char32_t*)src - 1;
	char32_t* p1 = (char32_t*)sub - 1;
	char32_t  c0, c1, c2;

	if ((c0 = *++p1) == 0)   // An empty pSubString results in success, return pString.
		return (char32_t*)src;

	while ((c1 = *++s1) != 0)
	{
		if (c1 == c0)
		{
			const char32_t* s2 = (s1 - 1);
			const char32_t* p2 = (p1 - 1);

			while ((c1 = *++s2) == (c2 = *++p2) && c1) {} // Do nothing

			if (!c2)
				return (char32_t*)s1;
		}
	}

	return nullptr;
}

char* fstristr(
	const char* src, 
	const char* sub
)
{
	const char* cp = src;

	if (!*sub)
		return (char*)src;

	while (*cp)
	{
		const char* s = cp;
		const char* t = sub;

		while (*s && *t && (__tolower(*s) == __tolower(*t)))
			++s, ++t;

		if (*t == 0)
			return (char*)cp;
		++cp;
	}

	return 0;
}

wchar_t* fstristr(
	const wchar_t* src, 
	const wchar_t* sub
)
{
	const wchar_t* cp = src;

	if (!*sub)
		return (wchar_t*)src;

	while (*cp)
	{
		const wchar_t* s = cp;
		const wchar_t* t = sub;

		while (*s && *t && (__tolower(*s) == __tolower(*t)))
			++s, ++t;

		if (*t == 0)
			return (wchar_t*)cp;
		++cp;
	}

	return 0;
}

char16_t* fstristr(
	const char16_t* src, 
	const char16_t* sub
)
{
	const char16_t* cp = src;

	if (!*sub)
		return (char16_t*)src;

	while (*cp)
	{
		const char16_t* s = cp;
		const char16_t* t = sub;

		while (*s && *t && (__tolower(*s) == __tolower(*t)))
			++s, ++t;

		if (*t == 0)
			return (char16_t*)cp;
		++cp;
	}

	return 0;
}

char32_t* fstristr(
	const char32_t* src, 
	const char32_t* sub
)
{
	const char32_t* cp = src;

	if (!*sub)
		return (char32_t*)src;

	while (*cp)
	{
		const char32_t* s = cp;
		const char32_t* t = sub;

		while (*s && *t && (__tolower(*s) == __tolower(*t)))
			++s, ++t;

		if (*t == 0)
			return (char32_t*)cp;
		++cp;
	}

	return 0;
}

char* fstrrstr(
	const char* src,
	const char* sub
)
{
	if (!*sub)
		return (char*)src;

	const char* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const char* psc1 = --ps1;
		const char* sc2 = sub;

		for (;;)
		{
			if (*psc1++ != *sc2++)
				break;
			else if (!*sc2)
				return (char*)ps1;
		}
	}

	return nullptr;
}

wchar_t* fstrrstr(
	const wchar_t* src, 
	const wchar_t* sub
)
{
	if (!*sub)
		return (wchar_t*)src;

	const wchar_t* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const wchar_t* psc1 = --ps1;
		const wchar_t* sc2 = sub;

		for (;;)
		{
			if (*psc1++ != *sc2++)
				break;
			else if (!*sc2)
				return (wchar_t*)ps1;
		}
	}

	return nullptr;
}

char16_t* fstrrstr(
	const char16_t* src, 
	const char16_t* sub
)
{
	if (!*sub)
		return (char16_t*)src;

	const char16_t* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const char16_t* psc1 = --ps1;
		const char16_t* sc2 = sub;

		for (;;)
		{
			if (*psc1++ != *sc2++)
				break;
			else if (!*sc2)
				return (char16_t*)ps1;
		}
	}

	return nullptr;
}

char32_t* fstrrstr(
	const char32_t* src,
	const char32_t* sub
)
{
	if (!*sub)
		return (char32_t*)src;

	const char32_t* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const char32_t* psc1 = --ps1;
		const char32_t* sc2 = sub;

		for (;;)
		{
			if (*psc1++ != *sc2++)
				break;
			else if (!*sc2)
				return (char32_t*)ps1;
		}
	}

	return nullptr;
}

char* fstrirstr(
	const char* src, 
	const char* sub
)
{
	if (!*sub)
		return (char*)src;

	const char* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const char* psc1 = --ps1;
		const char* sc2 = sub;

		for (;;)
		{
			if (__tolower(*psc1++) != __tolower(*sc2++))
				break;
			else if (!*sc2)
				return (char*)ps1;
		}
	}
	return nullptr;
}

wchar_t* fstrirstr(
	const wchar_t* src,
	const wchar_t* sub
)
{
	if (!*sub)
		return (wchar_t*)src;

	const wchar_t* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const wchar_t* psc1 = --ps1;
		const wchar_t* sc2 = sub;

		for (;;)
		{
			if (__tolower(*psc1++) != __tolower(*sc2++))
				break;
			else if (!*sc2)
				return (wchar_t*)ps1;
		}
	}
	return nullptr;
}

char16_t* fstrirstr(
	const char16_t* src,
	const char16_t* sub
)
{
	if (!*sub)
		return (char16_t*)src;

	const char16_t* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const char16_t* psc1 = --ps1;
		const char16_t* sc2 = sub;

		for (;;)
		{
			if (__tolower(*psc1++) != __tolower(*sc2++))
				break;
			else if (!*sc2)
				return (char16_t*)ps1;
		}
	}
	return nullptr;
}

char32_t* fstrirstr(
	const char32_t* src, 
	const char32_t* sub
)
{
	if (!*sub)
		return (char32_t*)src;

	const char32_t* ps1 = src + fstrlen(src);

	while (ps1 != src)
	{
		const char32_t* psc1 = --ps1;
		const char32_t* sc2 = sub;

		for (;;)
		{
			if (__tolower(*psc1++) != __tolower(*sc2++))
				break;
			else if (!*sc2)
				return (char32_t*)ps1;
		}
	}
	return nullptr;
}

bool fstrstart(
	const char* src, 
	const char* prefix
)
{
	while (*prefix)
	{
		if (*src++ != *prefix++)
			return false;
	}

	return true;
}

bool fstrstart(
	const wchar_t* src, 
	const wchar_t* prefix
)
{
	while (*prefix)
	{
		if (*src++ != *prefix++)
			return false;
	}

	return true;
}

bool fstrstart(
	const char16_t* src, 
	const char16_t* prefix
)
{
	while (*prefix)
	{
		if (*src++ != *prefix++)
			return false;
	}

	return true;
}

bool fstrstart(
	const char32_t* src,
	const char32_t* prefix
)
{
	while (*prefix)
	{
		if (*src++ != *prefix++)
			return false;
	}

	return true;
}

bool fstristart(
	const char* src, 
	const char* prefix
)
{
	while (*prefix)
	{
		if (__tolower(*src++) != __tolower(*prefix++))
			return false;
	}

	return true;
}

bool fstristart(
	const wchar_t* src, 
	const wchar_t* prefix
)
{
	while (*prefix)
	{
		if (__tolower(*src++) != __tolower(*prefix++))
			return false;
	}

	return true;
}

bool fstristart(
	const char16_t* src,
	const char16_t* prefix
)
{
	while (*prefix)
	{
		if (__tolower(*src++) != __tolower(*prefix++))
			return false;
	}

	return true;
}

bool fstristart(
	const char32_t* src, 
	const char32_t* prefix
)
{
	while (*prefix)
	{
		if (__tolower(*src++) != __tolower(*prefix++))
			return false;
	}

	return true;
}

char* fstrend(const char* str)
{
	while (*str)
		++str;
	return (char*)str;
}

wchar_t* fstrend(const wchar_t* str)
{
	while (*str)
		++str;
	return (wchar_t*)str;
}

char16_t* fstrend(const char16_t* str)
{
	while (*str)
		++str;
	return (char16_t*)str;
}

char32_t* fstrend(const char32_t* str)
{
	while (*str)
		++str;
	return (char32_t*)str;
}

bool fstrend(
	const char* str, 
	const char* suffix,
	size_t strlen,
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return memcmp((str + strlen - suffixlen), suffix, suffixlen * sizeof(char)) == 0;

	return false;
}

bool fstrend(
	const wchar_t* str,
	const wchar_t* suffix,
	size_t strlen,
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return memcmp(str + strlen - suffixlen, suffix, suffixlen * sizeof(wchar_t)) == 0;

	return false;
}

bool fstrend(
	const char16_t* str, 
	const char16_t* suffix,
	size_t strlen,
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return memcmp(str + strlen - suffixlen, suffix, suffixlen * sizeof(char16_t)) == 0;

	return false;
}

bool fstrend(
	const char32_t* str,
	const char32_t* suffix,
	size_t strlen,
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return memcmp(str + strlen - suffixlen, suffix, suffixlen * sizeof(char32_t)) == 0;

	return false;
}

bool fstriend(
	const char* str,
	const char* suffix,
	size_t strlen,
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return fstricmp(str + strlen - suffixlen, suffix) == 0;

	return false;
}

bool fstriend(
	const wchar_t* str,
	const wchar_t* suffix, 
	size_t strlen,
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return fstricmp(str + strlen - suffixlen, suffix) == 0;

	return false;
}

bool fstriend(
	const char16_t* str,
	const char16_t* suffix,
	size_t strlen, 
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return fstricmp(str + strlen - suffixlen, suffix) == 0;

	return false;
}

bool fstriend(
	const char32_t* str,
	const char32_t* suffix, 
	size_t strlen, 
	size_t suffixlen
)
{
	if (strlen == SIZE_TYPE_UNSET)
		strlen = fstrlen(str);

	if (suffixlen == SIZE_TYPE_UNSET)
		suffixlen = fstrlen(suffix);

	if (strlen >= suffixlen)
		return fstricmp(str + strlen - suffixlen, suffix) == 0;

	return false;
}

char* fstrtok(
	char* str,
	const char* delim,
	char** context
)
{
	char* s = str;

	if (!s)
	{
		s = *context;
		if (!s)
			return nullptr;
	}

	// Compute bit hash based on lower 5 bits of delimiter characters
	const char* d = delim;
	int32_t hash = 0;
	uint32_t delimiterCount = 0;

	while (const char c = *d++)
	{
		hash |= (int32_t)(0x80000000 >> (c & 31));
		++delimiterCount;
	}

	// Skip delimiters
	for (;;)
	{
		const char c = *s;

		// If we hit the end of the string, it ends solely with delimiters
		// and there are no more tokens to get.
		if (!c)
		{
			*context = nullptr;
			return nullptr;
		}

		// Fast rejection against hash set
		if (int32_t(uint64_t(hash) << (c & 31)) >= 0)
			break;

		// brute-force search against delimiter list
		for (uint32_t i = 0; i < delimiterCount; ++i)
		{
			if (delim[i] == c)    // Is it a delimiter? ...
				goto still_delimiters;  // yes, continue the loop
		}

		// Not a token, so exit
		break;

	still_delimiters:
		++s;
	}

	// Mark beginning of token
	char* const pToken = s;

	// Search for end of token
	while (const char c = *s)
	{
		// Fast rejection against hash set
		if (int32_t(int64_t(hash) << (c & 31)) < 0)
		{
			// Brute-force search against delimiter list
			for (uint32_t i = 0; i < delimiterCount; ++i)
			{
				if (delim[i] == c)
				{
					// This token ends with a delimiter.
					*s = 0;                 // null-term substring
					*context = (s + 1);    // restart on next byte
					return pToken;          // return found token
				}
			}
		}

		++s;
	}

	// We found a token but it was at the end of the string, 
	// so we null out the context and return the last token.
	*context = nullptr;           // no more tokens
	return pToken;              // return found token
}

wchar_t* fstrtok(
	wchar_t* str, 
	const wchar_t* delim, 
	wchar_t** context
)
{
	wchar_t* s = str;

	if (!s)
	{
		s = *context;
		if (!s)
			return nullptr;
	}

	// Compute bit hash based on lower 5 bits of delimiter characters
	const wchar_t* d = delim;
	int32_t hash = 0;
	uint32_t delimiterCount = 0;

	while (const wchar_t c = *d++)
	{
		hash |= (int32_t)(0x80000000 >> (c & 31));
		++delimiterCount;
	}

	// Skip delimiters
	for (;;)
	{
		const wchar_t c = *s;

		// If we hit the end of the string, it ends solely with delimiters
		// and there are no more tokens to get.
		if (!c)
		{
			*context = nullptr;
			return nullptr;
		}

		// Fast rejection against hash set
		if (int32_t(uint64_t(hash) << (c & 31)) >= 0)
			break;

		// brute-force search against delimiter list
		for (uint32_t i = 0; i < delimiterCount; ++i)
		{
			if (delim[i] == c)    // Is it a delimiter? ...
				goto still_delimiters;  // yes, continue the loop
		}

		// Not a token, so exit
		break;

	still_delimiters:
		++s;
	}

	// Mark beginning of token
	wchar_t* const pToken = s;

	// Search for end of token
	while (const wchar_t c = *s)
	{
		// Fast rejection against hash set
		if (int32_t(int64_t(hash) << (c & 31)) < 0)
		{
			// Brute-force search against delimiter list
			for (uint32_t i = 0; i < delimiterCount; ++i)
			{
				if (delim[i] == c)
				{
					// This token ends with a delimiter.
					*s = 0;                 // null-term substring
					*context = (s + 1);    // restart on next byte
					return pToken;          // return found token
				}
			}
		}

		++s;
	}

	// We found a token but it was at the end of the string, 
	// so we null out the context and return the last token.
	*context = nullptr;           // no more tokens
	return pToken;              // return found token
}

char16_t* fstrtok(
	char16_t* str,
	const char16_t* delim, 
	char16_t** context
)
{
	char16_t* s = str;

	if (!s)
	{
		s = *context;
		if (!s)
			return nullptr;
	}

	// Compute bit hash based on lower 5 bits of delimiter characters
	const char16_t* d = delim;
	int32_t hash = 0;
	uint32_t delimiterCount = 0;

	while (const char16_t c = *d++)
	{
		hash |= (int32_t)(0x80000000 >> (c & 31));
		++delimiterCount;
	}

	// Skip delimiters
	for (;;)
	{
		const char16_t c = *s;

		// If we hit the end of the string, it ends solely with delimiters
		// and there are no more tokens to get.
		if (!c)
		{
			*context = nullptr;
			return nullptr;
		}

		// Fast rejection against hash set
		if (int32_t(uint64_t(hash) << (c & 31)) >= 0)
			break;

		// brute-force search against delimiter list
		for (uint32_t i = 0; i < delimiterCount; ++i)
		{
			if (delim[i] == c)    // Is it a delimiter? ...
				goto still_delimiters;  // yes, continue the loop
		}

		// Not a token, so exit
		break;

	still_delimiters:
		++s;
	}

	// Mark beginning of token
	char16_t* const pToken = s;

	// Search for end of token
	while (const char16_t c = *s)
	{
		// Fast rejection against hash set
		if (int32_t(int64_t(hash) << (c & 31)) < 0)
		{
			// Brute-force search against delimiter list
			for (uint32_t i = 0; i < delimiterCount; ++i)
			{
				if (delim[i] == c)
				{
					// This token ends with a delimiter.
					*s = 0;                 // null-term substring
					*context = (s + 1);    // restart on next byte
					return pToken;          // return found token
				}
			}
		}

		++s;
	}

	// We found a token but it was at the end of the string, 
	// so we null out the context and return the last token.
	*context = nullptr;           // no more tokens
	return pToken;              // return found token
}

char32_t* fstrtok(
	char32_t* str, 
	const char32_t* delim,
	char32_t** context
)
{
	char32_t* s = str;

	if (!s)
	{
		s = *context;
		if (!s)
			return nullptr;
	}

	// Compute bit hash based on lower 5 bits of delimiter characters
	const char32_t* d = delim;
	int32_t hash = 0;
	uint32_t delimiterCount = 0;

	while (const char32_t c = *d++)
	{
		hash |= (int32_t)(0x80000000 >> (c & 31));
		++delimiterCount;
	}

	// Skip delimiters
	for (;;)
	{
		const char32_t c = *s;

		// If we hit the end of the string, it ends solely with delimiters
		// and there are no more tokens to get.
		if (!c)
		{
			*context = nullptr;
			return nullptr;
		}

		// Fast rejection against hash set
		if (int32_t(uint64_t(hash) << (c & 31)) >= 0)
			break;

		// brute-force search against delimiter list
		for (uint32_t i = 0; i < delimiterCount; ++i)
		{
			if (delim[i] == c)    // Is it a delimiter? ...
				goto still_delimiters;  // yes, continue the loop
		}

		// Not a token, so exit
		break;

	still_delimiters:
		++s;
	}

	// Mark beginning of token
	char32_t* const pToken = s;

	// Search for end of token
	while (const char32_t c = *s)
	{
		// Fast rejection against hash set
		if (int32_t(int64_t(hash) << (c & 31)) < 0)
		{
			// Brute-force search against delimiter list
			for (uint32_t i = 0; i < delimiterCount; ++i)
			{
				if (delim[i] == c)
				{
					// This token ends with a delimiter.
					*s = 0;                 // null-term substring
					*context = (s + 1);    // restart on next byte
					return pToken;          // return found token
				}
			}
		}

		++s;
	}

	// We found a token but it was at the end of the string, 
	// so we null out the context and return the last token.
	*context = nullptr;           // no more tokens
	return pToken;              // return found token
}

char* fstrrev2(char* str)
{
	for (char* p1 = str, *p2 = (str + fstrlen(str)) - 1; p1 < p2; ++p1, --p2)
	{
		char c = *p2;
		*p2 = *p1;
		*p1 = c;
	}

	return str;
}

wchar_t* fstrrev2(wchar_t* str)
{
	for (wchar_t* p1 = str, *p2 = (str + fstrlen(str)) - 1; p1 < p2; ++p1, --p2)
	{
		wchar_t c = *p2;
		*p2 = *p1;
		*p1 = c;
	}

	return str;
}

char16_t* fstrrev2(char16_t* str)
{
	for (char16_t* p1 = str, *p2 = (str + fstrlen(str)) - 1; p1 < p2; ++p1, --p2)
	{
		char16_t c = *p2;
		*p2 = *p1;
		*p1 = c;
	}

	return str;
}

char32_t* fstrrev2(char32_t* str)
{
	for (char32_t* p1 = str, *p2 = (str + fstrlen(str)) - 1; p1 < p2; ++p1, --p2)
	{
		char32_t c = *p2;
		*p2 = *p1;
		*p1 = c;
	}

	return str;
}

char* fstrstrip(char* str)
{
	while (_isspace(*str)) // Isspace returns false for *pString == '\0'.
		++str;

	if (*str)
	{
		// Walk backward from the end and find the last whitespace.
		size_t length = fstrlen(str);
		char* end = (str + length) - 1;

		while ((end > str) && _isspace(*end))
			end--;

		end[1] = '\0';
	}

	return str;
}

wchar_t* fstrstrip(wchar_t* str)
{
	while (_isspace(*str)) // Isspace returns false for *pString == '\0'.
		++str;

	if (*str)
	{
		// Walk backward from the end and find the last whitespace.
		size_t length = fstrlen(str);
		wchar_t* end = (str + length) - 1;

		while ((end > str) && _isspace(*end))
			end--;

		end[1] = '\0';
	}

	return str;
}

char16_t* fstrstrip(char16_t* str)
{
	while (_isspace(*str)) // Isspace returns false for *pString == '\0'.
		++str;

	if (*str)
	{
		// Walk backward from the end and find the last whitespace.
		size_t length = fstrlen(str);
		char16_t* end = (str + length) - 1;

		while ((end > str) && _isspace(*end))
			end--;

		end[1] = '\0';
	}

	return str;
}

char32_t* fstrstrip(char32_t* str)
{
	while (_isspace(*str)) // Isspace returns false for *pString == '\0'.
		++str;

	if (*str)
	{
		// Walk backward from the end and find the last whitespace.
		size_t length = fstrlen(str);
		char32_t* end = (str + length) - 1;

		while ((end > str) && _isspace(*end))
			end--;

		end[1] = '\0';
	}

	return str;
}

int fstrcmp(
	const char* str1, 
	const char* str2
)
{
	char c1, c2;

	while ((c1 = *str1++) == (c2 = *str2++))
	{
		if (c1 == 0)
			return 0;
	}

	return ((uint8_t)c1 - (uint8_t)c2);
}

int fstrcmp(
	const wchar_t* str1, 
	const wchar_t* str2
)
{
	wchar_t c1, c2;

	while ((c1 = *str1++) == (c2 = *str2++))
	{
		if (c1 == 0) // If we've reached the end of the string with no difference...
			return 0;
	}

	return ((wchar_t)c1 - (wchar_t)c2);
}

int fstrcmp(
	const char16_t* str1, 
	const char16_t* str2
)
{
	char16_t c1, c2;

	while ((c1 = *str1++) == (c2 = *str2++))
	{
		if (c1 == 0) // If we've reached the end of the string with no difference...
			return 0;
	}

	return ((char16_t)c1 - (char16_t)c2);
}

int fstrcmp(
	const char32_t* str1,
	const char32_t* str2
)
{
	char32_t c1, c2;

	while ((c1 = *str1++) == (c2 = *str2++))
	{
		if (c1 == 0) // If we've reached the end of the string with no difference...
			return 0;
	}

	// We can't just return c1 - c2, because the difference might be greater than INT_MAX.
	return ((uint32_t)c1 > (uint32_t)c2) ? 1 : -1;
}

int fstrncmp(
	const char* str1, 
	const char* str2, 
	size_t n
)
{
	return strncmp(str1, str2, n);
}

int fstrncmp(
	const wchar_t* str1, 
	const wchar_t* str2, 
	size_t n
)
{
	wchar_t c1, c2;

	// Code below which uses (c1 - c2) assumes this.
	assert(sizeof(int) > sizeof(wchar_t));

	++n;
	while (--n)
	{
		if ((c1 = *str1++) != (c2 = *str2++))
			return ((wchar_t)c1 - (wchar_t)c2);
		else if (c1 == 0)
			break;
	}

	return 0;
}

int fstrncmp(
	const char16_t* str1, 
	const char16_t* str2, 
	size_t n
)
{
	char16_t c1, c2;

	// Code below which uses (c1 - c2) assumes this.
	assert(sizeof(int) > sizeof(char16_t));

	++n;
	while (--n)
	{
		if ((c1 = *str1++) != (c2 = *str2++))
			return ((uint16_t)c1 - (uint16_t)c2);
		else if (c1 == 0)
			break;
	}

	return 0;
}

int fstrncmp(
	const char32_t* str1,
	const char32_t* str2, 
	size_t n
)
{
	char32_t c1, c2;

	++n;
	while (--n)
	{
		if ((c1 = *str1++) != (c2 = *str2++))
		{
			// We can't just return c1 - c2, because the difference might be greater than INT_MAX.
			return ((uint32_t)c1 > (uint32_t)c2) ? 1 : -1;
		}
		else if (c1 == 0)
			break;
	}

	return 0;
}

int fstricmp(
	const char* str1,
	const char* str2
)
{
	char c1, c2;

	while ((c1 = __tolower(*str1++)) == (c2 = __tolower(*str2++)))
	{
		if (c1 == 0)
			return 0;
	}

	return ((uint8_t)c1 - (uint8_t)c2);
}

int fstricmp(
	const wchar_t* str1,
	const wchar_t* str2
)
{
	char16_t c1, c2;

	while ((c1 = __tolower(*str1++)) == (c2 = __tolower(*str2++)))
	{
		if (c1 == 0)
			return 0;
	}

	// Code below which uses (c1 - c2) assumes this.
	assert(sizeof(int) > sizeof(uint16_t));
	return ((uint16_t)c1 - (uint16_t)c2);
}

int fstricmp(
	const char16_t* str1, 
	const char16_t* str2
)
{
	char16_t c1, c2;

	while ((c1 = __tolower(*str1++)) == (c2 = __tolower(*str2++)))
	{
		if (c1 == 0)
			return 0;
	}

	// Code below which uses (c1 - c2) assumes this.
	assert(sizeof(int) > sizeof(uint16_t));
	return ((uint16_t)c1 - (uint16_t)c2);
}

int fstricmp(
	const char32_t* str1, 
	const char32_t* str2
)
{
	char32_t c1, c2;

	while ((c1 = __tolower(*str1++)) == (c2 = __tolower(*str2++)))
	{
		if (c1 == 0)
			return 0;
	}

	// We can't just return c1 - c2, because the difference might be greater than INT_MAX.
	return ((uint32_t)c1 > (uint32_t)c2) ? 1 : -1;
}

int fstrincmp(
	const char* str1, 
	const char* str2, 
	size_t n
)
{
	char c1, c2;

	++n;
	while (--n)
	{
		if ((c1 = __tolower(*str1++)) != (c2 = __tolower(*str2++)))
			return ((uint8_t)c1 - (uint8_t)c2);
		else if (c1 == 0)
			break;
	}

	return 0;
}

int fstrincmp(
	const wchar_t* str1, 
	const wchar_t* str2, 
	size_t n
)
{
	char16_t c1, c2;

	// Code below which uses (c1 - c2) assumes this.
	assert(sizeof(int) > sizeof(uint16_t));

	++n;
	while (--n)
	{
		if ((c1 = __tolower(*str1++)) != (c2 = __tolower(*str2++)))
			return ((uint16_t)c1 - (uint16_t)c2);
		else if (c1 == 0)
			break;
	}

	return 0;
}

int fstrincmp(
	const char16_t* str1,
	const char16_t* str2, 
	size_t n
)
{
	char16_t c1, c2;

	// Code below which uses (c1 - c2) assumes this.
	assert(sizeof(int) > sizeof(uint16_t));

	++n;
	while (--n)
	{
		if ((c1 = __tolower(*str1++)) != (c2 = __tolower(*str2++)))
			return ((uint16_t)c1 - (uint16_t)c2);
		else if (c1 == 0)
			break;
	}

	return 0;
}

int fstrincmp(
	const char32_t* str1, 
	const char32_t* str2, 
	size_t n
)
{
	char32_t c1, c2;

	++n;
	while (--n)
	{
		if ((c1 = _tolower(*str1++)) != (c2 = _tolower(*str2++)))
		{
			// We can't just return c1 - c2, because the difference might be greater than INT_MAX.
			return ((uint32_t)c1 > (uint32_t)c2) ? 1 : -1;
		}
		else if (c1 == 0)
			break;
	}

	return 0;
}
