#ifndef __NAME_HASH_H__
#define __NAME_HASH_H__

#include "fnv.h"
#include "strings.h"


class MARKENGINE_API NameHash
{
public:
	NameHash() = default;
	NameHash(const char* szName);

	explicit NameHash(uint32 Hash)
		: m_Hash(Hash)
	{
	}

	NameHash(const NameHash& Other)
		: m_Hash(Other.m_Hash)
	{
	}

	NameHash(NameHash&& Other) noexcept
	{
		m_Hash = Other.m_Hash;
	}

	inline NameHash& operator=(const char* Other) noexcept
	{
		if (Other)
			m_Hash = fnv1_cstr_c(Other, C_ANY);
		return *this;
	}

	inline NameHash& operator=(const NameHash& Other) noexcept
	{
		m_Hash = Other.m_Hash;
		return *this;
	}

	inline NameHash& operator=(NameHash&& Other) noexcept
	{
		m_Hash = Other.m_Hash;
		return *this;
	}

	inline bool operator ==(const NameHash& Other) const noexcept
	{
		return m_Hash == Other.m_Hash;
	}

	inline bool operator ==(uint32 Other) const noexcept
	{
		return m_Hash == Other;
	}

	inline bool operator ==(const char* Other) const noexcept
	{
		uint32 OtherHash = 0;
		if (Other)
			OtherHash = fnv1_cstr_c(Other, C_ANY);
		return m_Hash == OtherHash;
	}

	inline bool operator !=(const NameHash& Other) const noexcept
	{
		return m_Hash != Other.m_Hash;
	}

	inline bool operator !=(uint32 Other) const noexcept
	{
		return m_Hash != Other;
	}

	inline bool operator !=(const char* Other) const noexcept
	{
		uint32 OtherHash = 0;
		if (Other)
			OtherHash = fnv1_cstr_c(Other, C_ANY);
		return m_Hash != OtherHash;
	}

	inline bool operator >(const NameHash& Other) const noexcept
	{
		return m_Hash > Other.m_Hash;
	}

	inline bool operator >(uint32 Other) const noexcept
	{
		return m_Hash > Other;
	}

	inline bool operator <(const NameHash& Other) const noexcept
	{
		return m_Hash < Other.m_Hash;
	}

	inline bool operator <(uint32 Other) const noexcept
	{
		return m_Hash < Other;
	}

	inline operator uint32() noexcept
	{
		return m_Hash;
	}

	inline operator const uint32() const noexcept
	{
		return m_Hash;
	}

	inline bool empty() const noexcept
	{
		return !m_Hash ? true : false;
	}

	inline uint32 value() const noexcept
	{
		return m_Hash;
	}

private:
	uint32 m_Hash = 0;

};

#endif // __NAME_HASH_H__
