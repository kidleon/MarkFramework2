#ifndef __OS_FILE_DATA_STREAM_H__
#define __OS_FILE_DATA_STREAM_H__

#include "IDataStream.h"

namespace mark
{
	class OSFileDataStream : public IDataStream
	{
		DECLARATION_IUNKNOWN_INTERFACE(OSFileDataStream);

	public:
		OSFileDataStream(HANDLE hFile);

		virtual size_t Read(void* buffer, size_t size) override;

		virtual size_t Write(const void* buffer, size_t size) override;

		virtual bool Seek(size_t position) override;

		virtual size_t Tell() const override;

		virtual size_t GetSize() const override;

	private:
		HANDLE m_hFile;

	};

}


#endif // __OS_FILE_DATA_STREAM_H__
