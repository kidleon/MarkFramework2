#ifndef __I_DATA_STREAM_H__
#define __I_DATA_STREAM_H__


class IDataStream : public IUNKNOWN
{
public:
	virtual size_t Read(void* buffer, size_t size) = 0;

	virtual size_t Write(const void* buffer, size_t size) = 0;

	virtual bool Seek(size_t position) = 0;

	virtual size_t Tell() const = 0;

	virtual size_t GetSize() const = 0;

};


#endif // __I_DATA_STREAM_H__
