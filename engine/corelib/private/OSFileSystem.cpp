#include "pch.h"
#include "OSFileSystem.h"
#include "interlock_atomic.h"
#include "strings.h"
#include "os_file.h"
#include "Heap.h"
#include "Log.h"
#include "OSFileDataStream.h"


IMPLEMENTATION_IUNKNOWN_INTERFACE(OSFileSystem);

OSFileSystem::OSFileSystem(const char* szRootPath)
{
	fstrlcpy(m_szRootPath, szRootPath, MAX_FILE_LENGTH - 1);

	size_t root_path_len = fstrlen(m_szRootPath);
	if (0 < root_path_len)
	{
		char last_char = m_szRootPath[root_path_len - 1];
		if (last_char == '/' || last_char == '\\')
		{
			// 경로 끝에 구분자 삭제
			m_szRootPath[root_path_len] = '\0';
		}
	}
}

OSFileSystem::~OSFileSystem() noexcept
{

}

void OSFileSystem::OnDestroy()
{
	MARK_DELETE(this, OSFileSystem);
}

inline void GenFullPath(
	const char* szRootPath, 
	const char* szRelativePath, 
	char* szFullPath, 
	size_t FullPathSize
)
{
	fstrlcpy(szFullPath, szRootPath, FullPathSize - 1);
	if (szRelativePath[0] != '/' && szRelativePath[0] != '\\')
		fstrlcat(szFullPath, "/", FullPathSize - 1);
	fstrlcat(szFullPath, szRelativePath, FullPathSize - 1);
}

IDataStream* OSFileSystem::OpenFile(
	const char* szRelativePath,
	BOOL ReadOnly
)
{
	if (!szRelativePath)
	{
		SYS_LOG_E("OSFileSystem::OpenFile: szRelativePath is NULL");
		return NULL;
	}

	// 전체 경로 생성
	char szFullPath[MAX_FILE_LENGTH];
	GenFullPath(m_szRootPath, szRelativePath, szFullPath, MAX_FILE_LENGTH - 1);
		
	HANDLE file_handle = open_file(szFullPath, ReadOnly ? FILE_MODE_READ_BINARY : FILE_MODE_WRITE_BINARY);
	if (!file_handle)
	{
		SYS_LOG_E("OSFileSystem::OpenFile: Failed to open file: %s", szFullPath);
		return NULL;
	}

	OSFileDataStream* pFileStream = MARK_NEW(OSFileDataStream)(file_handle);

	return static_cast<IDataStream*>(pFileStream);
}

IDataStream* OSFileSystem::CreateFile(
	const char* szRelativePath,
	BOOL OverwriteIfExists
)
{
	if (!szRelativePath)
	{
		SYS_LOG_E("OSFileSystem::CreateFile: szRelativePath is NULL");
		return NULL;
	}

	// 전체 경로 생성
	char szFullPath[MAX_FILE_LENGTH];
	GenFullPath(m_szRootPath, szRelativePath, szFullPath, MAX_FILE_LENGTH - 1);

	// 파일이 이미 존재하는지 확인
	if (!OverwriteIfExists)
	{
		if (exist_file(szFullPath))
		{
			SYS_LOG_E("OSFileSystem::CreateFile: File already exists: %s", szFullPath);
			return NULL;
		}
	}
	else
	{
		// 덮어쓰기 모드인 경우 기존 파일이 있으면 삭제
		if (exist_file(szFullPath))
		{
			DeleteFile(szRelativePath);
		}
	}

	HANDLE file_handle = open_file(szFullPath, FILE_MODE_WRITE_BINARY);
	if (!file_handle)
	{
		SYS_LOG_E("OSFileSystem::CreateFile: Failed to create file: %s", szFullPath);
		return NULL;
	}

	OSFileDataStream* pFileStream = MARK_NEW(OSFileDataStream)(file_handle);
	return static_cast<IDataStream*>(pFileStream);
}

void OSFileSystem::DeleteFile(
	const char* szRelativePath
)
{
	if (!szRelativePath)
	{
		SYS_LOG_E("OSFileSystem::DeleteFile: szRelativePath is NULL");
		return;
	}

	// 전체 경로 생성
	char szFullPath[MAX_FILE_LENGTH];
	GenFullPath(m_szRootPath, szRelativePath, szFullPath, MAX_FILE_LENGTH - 1);

	if (exist_file(szFullPath))
	{
		int res = remove(szFullPath);
		if (res != 0)
		{
			SYS_LOG_E("OSFileSystem::DeleteFile: Failed to delete file: %s", szFullPath);
		}
	}
}

size_t OSFileSystem::GetFileSize(
	const char* szRelativePath
) noexcept
{
	return get_file_size(szRelativePath);
}

BOOL OSFileSystem::ExistFile(
	const char* szRelativePath
) noexcept
{
	return exist_file(szRelativePath);
}
