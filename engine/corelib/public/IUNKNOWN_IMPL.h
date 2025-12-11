#ifndef __IUNKNOWN_IMPL_H__
#define __IUNKNOWN_IMPL_H__


struct IUNKNOWN_IMPL : public IUNKNOWN
{
	virtual long AddRef() override
	{
		if (this)
		{
			return ++m_RefCnt;
		}
		return 0;
	}

	virtual long Release() override
	{
		if (this)
		{
			long NewRefCnt = --m_RefCnt;
			if (NewRefCnt == 0)
			{
				delete this;
			}
			return NewRefCnt;
		}
		return 0;
	}

	virtual long RefCnt() override
	{
		return m_RefCnt;
	}

protected:
	virtual ~IUNKNOWN_IMPL() noexcept = default;
	IUNKNOWN_IMPL(const IUNKNOWN_IMPL&) = delete;
	IUNKNOWN_IMPL(IUNKNOWN_IMPL&&) = delete;
	IUNKNOWN_IMPL& operator=(const IUNKNOWN_IMPL&) = delete;
	IUNKNOWN_IMPL& operator=(IUNKNOWN_IMPL&&) = delete;

private:
	volatile long m_RefCnt = 1;
	unsigned PADDING_OR_RESERVED = 0;

};

#endif // __IUNKNOWN_IMPL_H__
