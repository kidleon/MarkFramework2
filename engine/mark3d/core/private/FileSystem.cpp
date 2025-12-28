#include "pch.h"
#include "OSFileSystem.h"


BOOL CreateOSFileSystem(
	const char* szRootPath,
	IFileSystem** ppFileSystem
)
{
	OSFileSystem* pFileSys = CORE_NEW(OSFileSystem)(szRootPath);
	(*ppFileSystem) = static_cast<IFileSystem*>(pFileSys);

	return TRUE;
}