#ifndef __OS_FILE_SYSTEM_H__
#define __OS_FILE_SYSTEM_H__

#include "IFileSystem.h"


class OSFileSystem : public IFileSystem
{
	DECLARATION_IUNKNOWN_INTERFACE(OSFileSystem);

public:
	OSFileSystem(const char* szRootPath);

	virtual HANDLE OpenFile(
		const char* szFilePath,
		BOOL ReadOnly
	) override;

	virtual void CloseFile(
		HANDLE hFile
	) override;

	virtual size_t ReadFile(
		HANDLE hFile,
		void* pBuffer,
		size_t BufferSize
	) override;

	virtual size_t WriteFile(
		HANDLE hFile,
		const void* pBuffer,
		size_t BufferSize
	) override;

	virtual size_t SeekFile(
		HANDLE hFile,
		size_t Position,
		uint32 SeekType
	) noexcept override;

	virtual size_t GetSeekPos(HANDLE Handle) noexcept override;

	virtual size_t GetFileSize(HANDLE Handle) noexcept override;

private:
	char m_szRootPath[MAX_FILE_LENGTH];

};


#endif // __OS_FILE_SYSTEM_H__



