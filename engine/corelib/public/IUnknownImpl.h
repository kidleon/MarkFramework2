#ifndef __I_UNKNOWN_IMPL_H__
#define __I_UNKNOWN_IMPL_H__

#include "hash_table.h"
#include "linked_list.h"


struct IUNKNOWN_IMPL : public IUNKNOWN
{
	IUNKNOWN_IMPL() = default;

	virtual long AddRef() final
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() final
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				OnDestroy();
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

protected:
	virtual ~IUNKNOWN_IMPL() noexcept = default;
	IUNKNOWN_IMPL(const IUNKNOWN_IMPL&) = delete;
	IUNKNOWN_IMPL(IUNKNOWN_IMPL&&) = delete;
	IUNKNOWN_IMPL& operator=(const IUNKNOWN_IMPL&) = delete;
	IUNKNOWN_IMPL& operator=(IUNKNOWN_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

private:
	volatile long m_RefCnt = 1;
#ifdef __TARGET_OS_WINDOWS
	unsigned PADDING_OR_RESERVED = 0;
#endif // __TARGET_OS_WINDOWS

};

//----------------------------------------------------------------
struct IUNKNOWN_HASH_IMPL : public IUNKNOWN
{
	IUNKNOWN_HASH_IMPL() = default;

	virtual long AddRef() final
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() final
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				OnDestroy();
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

	__FORCEINLINE HASH_NODE* INL_GetHashNode() noexcept { return &m_HashNode; }

protected:
	virtual ~IUNKNOWN_HASH_IMPL() noexcept = default;
	IUNKNOWN_HASH_IMPL(const IUNKNOWN_HASH_IMPL&) = delete;
	IUNKNOWN_HASH_IMPL(IUNKNOWN_HASH_IMPL&&) = delete;
	IUNKNOWN_HASH_IMPL& operator=(const IUNKNOWN_HASH_IMPL&) = delete;
	IUNKNOWN_HASH_IMPL& operator=(IUNKNOWN_HASH_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

private:
	volatile long m_RefCnt = 1;
#ifdef __TARGET_OS_WINDOWS
	unsigned PADDING_OR_RESERVED = 0;
#endif // __TARGET_OS_WINDOWS

	HASH_NODE m_HashNode;

};

//----------------------------------------------------------------
struct IUNKNOWN_LIST_IMPL : public IUNKNOWN
{
	IUNKNOWN_LIST_IMPL() = default;

	virtual long AddRef() final
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() final
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				OnDestroy();
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() final
	{
		return m_RefCnt;
	}

	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }

protected:
	virtual ~IUNKNOWN_LIST_IMPL() noexcept = default;
	IUNKNOWN_LIST_IMPL(const IUNKNOWN_LIST_IMPL&) = delete;
	IUNKNOWN_LIST_IMPL(IUNKNOWN_LIST_IMPL&&) = delete;
	IUNKNOWN_LIST_IMPL& operator=(const IUNKNOWN_LIST_IMPL&) = delete;
	IUNKNOWN_LIST_IMPL& operator=(IUNKNOWN_LIST_IMPL&&) = delete;
	virtual void OnDestroy() = 0;

private:
	volatile long m_RefCnt = 1;
#ifdef __TARGET_OS_WINDOWS
	unsigned PADDING_OR_RESERVED = 0;
#endif // __TARGET_OS_WINDOWS

	LINK_NODE m_LinkNode;

};

#endif // __I_UNKNOWN_IMPL_H__
