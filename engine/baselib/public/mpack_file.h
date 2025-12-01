#ifndef __MPACK_FILE_H__
#define __MPACK_FILE_H__




MARK_BASELIB_API HANDLE mpack_file_create_or_open(
	const char* filepath,
);

MARK_BASELIB_API BOOL mpack_file_close(
	HANDLE hMPackFile
);

#endif // __MPACK_FILE_H__
