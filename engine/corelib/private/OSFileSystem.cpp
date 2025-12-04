#include "pch.h"
#include "OSFileSystem.h"
#include "interlock_atomic.h"
#include "strings.h"


IMPLEMENTATION_IUNKNOWN_INTERFACE(OSFileSystem);

OSFileSystem::OSFileSystem(const char* szRootPath)
{
	fstrlcpy(m_szRootPath, szRootPath, MAX_FILE_LENGTH - 1);
}

OSFileSystem::~OSFileSystem() noexcept
{

}

void OSFileSystem::OnDestroy()
{

}

HANDLE OSFileSystem::OpenFile(
	const char* szFilePath,
	BOOL ReadOnly
)
{
	return NULL;
}

void OSFileSystem::CloseFile(
	HANDLE hFile
)
{
}

size_t OSFileSystem::ReadFile(
	HANDLE hFile,
	void* pBuffer,
	size_t BufferSize
)
{
	return 0;
}

size_t OSFileSystem::WriteFile(
	HANDLE hFile,
	const void* pBuffer,
	size_t BufferSize
)
{
	return 0;
}

size_t OSFileSystem::SeekFile(
	HANDLE hFile,
	size_t Position,
	uint32 SeekType
) noexcept
{
	return 0;
}

size_t OSFileSystem::GetSeekPos(HANDLE Handle) noexcept
{
	return 0;
}

size_t OSFileSystem::GetFileSize(HANDLE Handle) noexcept
{
	return 0;
}