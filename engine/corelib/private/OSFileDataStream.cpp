#include "pch.h"
#include "OSFileDataStream.h"
#include "interlock_atomic.h"
#include "os_file.h"
#include "Heap.h"


IMPLEMENTATION_IUNKNOWN_INTERFACE(OSFileDataStream);

OSFileDataStream::OSFileDataStream(HANDLE hFile)
	: m_hFile(hFile)
{
}

OSFileDataStream::~OSFileDataStream()
{
	if (m_hFile)
	{
		close_file(m_hFile);
		m_hFile = nullptr;
	}
}

void OSFileDataStream::OnDestroy()
{
	MARK_DELETE(this, OSFileDataStream);
}

size_t OSFileDataStream::Read(void* buffer, size_t size)
{
	if (!m_hFile || !buffer || size == 0)
		return 0;

	return read_file(m_hFile, buffer, size);
}

size_t OSFileDataStream::Write(const void* buffer, size_t size)
{
	if (!m_hFile || !buffer || size == 0)
		return 0;
	return write_file(m_hFile, buffer, size);
}

bool OSFileDataStream::Seek(size_t position)
{
	if (!m_hFile)
		return false;

	seek_file(m_hFile, position, FILE_SEEK_SET);

	return true;
}

size_t OSFileDataStream::Tell() const
{
	if (!m_hFile)
		return 0;
	return tell_file(m_hFile);
}

size_t OSFileDataStream::GetSize() const
{
	if (!m_hFile)
		return 0;

	return get_file_size_by_handle(m_hFile);
}
