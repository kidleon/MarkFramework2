#ifndef __I_DATA_STREAM_H__
#define __I_DATA_STREAM_H__

#include "IUnknownImpl.h"


interface IDataStream : public IUNKNOWN_IMPL
{
public:
	virtual size_t Read(void* pBuffer, size_t BufferSize) = 0;

	virtual size_t Write(const void* pBuffer, size_t BufferSize) = 0;

	virtual bool Seek(size_t Position) = 0;

	virtual size_t Tell() const = 0;

	virtual size_t GetSize() const = 0;

};

#endif // __I_DATA_STREAM_H__
