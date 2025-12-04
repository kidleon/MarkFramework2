#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "IFileSystem.h"


MARKENGINE_C_API BOOL __stdcall CreateOSFileSystem(
	const char* szRootPath,
	IFileSystem** ppFileSystem
);



#endif // __FILE_SYSTEM_H__
