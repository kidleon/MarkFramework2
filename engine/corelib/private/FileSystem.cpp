#include "pch.h"
#include "IFileSystem.h"
#include "Heap.h"
#include "OSFileSystem.h"


BOOL CreateOSFileSystem(
	const char* szRootPath,
	IFileSystem** ppFileSystem
)
{
	OSFileSystem* pFileSys = MARK_NEW(OSFileSystem)(szRootPath);
	(*ppFileSystem) = static_cast<IFileSystem*>(pFileSys);

	return TRUE;
}