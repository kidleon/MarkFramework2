#ifndef __OS_FILE_SYSTEM_H__
#define __OS_FILE_SYSTEM_H__

#include "IFileSystem.h"


class OSFileSystem : public IFileSystem
{
	DECLARATION_IUNKNOWN_INTERFACE(OSFileSystem);

public:
	OSFileSystem(const char* szRootPath);

	virtual IDataStream* OpenFile(
		const char* szRelativePath,
		BOOL ReadOnly
	) override;

#ifdef CreateFile
#undef CreateFile
#endif // CreateFile
	virtual IDataStream* CreateFile(
		const char* szRelativePath,
		BOOL OverwriteIfExists
	) override;

#ifdef DeleteFile
#undef DeleteFile
#endif // DeleteFile
	virtual void DeleteFile(
		const char* szRelativePath
	) override;

	virtual size_t GetFileSize(
		const char* szRelativePath
	) noexcept override;

	virtual BOOL ExistFile(
		const char* szRelativePath
	) noexcept override;

private:
	char m_szRootPath[MAX_FILE_LENGTH];

};


#endif // __OS_FILE_SYSTEM_H__



