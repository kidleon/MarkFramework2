#ifndef __T_UNKNOWN_H__
#define __T_UNKNOWN_H__


template<typename _T>
struct T_UNKNOWN
{
	long AddRef() override
	{
		if (this)
		{
			++m_RefCount;
		}
	}

	long Release() override
	{
		if (this)
		{
			if (--m_RefCount == 0)
			{
				OnDestroy();
			}
		}
	}

	long RefCnt() override
	{
		return m_RefCount;
	}

protected:
	virtual ~_T() noexcept;
	_T(const _T&) = delete;
	_T(_T&&) = delete;
	_T& operator=(const _T&) = delete;
	_T& operator=(_T&&) = delete;

	void OnDestroy();

private:
	volatile long m_RefCnt = 1;
	unsigned PADDING_OR_RESERVED = 0;

}


#endif // __T_UNKNOWN_H__
