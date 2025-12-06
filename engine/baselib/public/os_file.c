#include "pch.h"
#include "os_file.h"
#include "strings.h"


/* 컴파일러별 헤더 및 함수 정의 */
#if defined(__TARGET_COMPILER_MSC)
	/* Microsoft Visual C++ */
#	include <sys/stat.h>
#	define STAT_STRUCT struct _stat64
#	define STAT_FUNC _stat64
#	include <io.h>     // _access()
#	define ACCESS _access
#	define F_OK 0
#elif defined(__TARGET_COMPLIER_GCC) || defined(__TARGET_COMPLIER_CLANG)
	/* GCC 또는 Clang */
#	include <sys/stat.h>
#	define STAT_STRUCT struct stat
#	define STAT_FUNC stat
#	include <unistd.h> // access()
#	define ACCESS access
#endif // 컴파일러별 헤더 및 함수 정의

struct OS_FILE
{
	FILE* file;
	char filename[MAX_FILE_LENGTH];
};


HANDLE open_file(
	const char* filename,
	FILE_MODE mode
)
{
	HANDLE file = (HANDLE)malloc(sizeof(struct OS_FILE));
	if (!file)
		return NULL;

	struct OS_FILE* os_file = (struct OS_FILE*)file;
	fstrlcpy(os_file->filename, filename, sizeof(os_file->filename) - 1);
	os_file->filename[sizeof(os_file->filename) - 1] = '\0';

	const char* mode_str = NULL;
	switch (mode)
	{
	case FILE_MODE_READ:
		mode_str = "rb";
		break;
	case FILE_MODE_WRITE:
		mode_str = "wb";
		break;
	case FILE_MODE_APPEND:
		mode_str = "ab";
		break;
	default:
		free(file);
		return NULL;
	}

#if defined(__TARGET_COMPILER_MSC)
	errno_t res = fopen_s(&os_file->file, os_file->filename, mode_str);
	if(res)
	{
		free(file);
		return NULL;
	}
#else
	os_file->file = fopen(os_file->filename, mode_str);
	if (!os_file->file)
	{
		free(file);
		return NULL;
	}
#endif // __TARGET_COMPILER_MSC

	return file;
}

void close_file(
	HANDLE file
) 
{
	struct OS_FILE* os_file = (struct OS_FILE*)file;
	if (os_file)
	{
		fclose(os_file->file);
		free(file);
	}
}

/**
* read_file
* @brief 파일에서 데이터를 읽습니다.
* @param file 읽을 파일 핸들
* @param buffer 읽은 데이터를 저장할 버퍼
* @param size 읽을 데이터 크기
* @return 성공시 읽은 데이터 크기, 실패시 0
*/
size_t read_file(
	HANDLE file,
	void* buffer,
	size_t size
)
{
	struct OS_FILE* os_file = (struct OS_FILE*)file;
	if (!os_file || !os_file->file || !buffer || size == 0)
		return 0;

	size_t bytes_read = fread(buffer, 1, size, os_file->file);
	if (bytes_read == 0 && ferror(os_file->file))
		return 0;

	return bytes_read;
}

/**
* write_file
* @brief 파일에 데이터를 씁니다.
* @param file 쓸 파일 핸들
* @param buffer 쓸 데이터 버퍼
* @param size 쓸 데이터 크기
* @return 성공시 쓴 데이터 크기, 실패시 0
*/
size_t write_file(
	HANDLE file,
	const void* buffer,
	size_t size
)
{
	struct OS_FILE* os_file = (struct OS_FILE*)file;
	if (!os_file || !os_file->file || !buffer || size == 0)
		return 0;

	size_t bytes_written = fwrite(buffer, 1, size, os_file->file);
	if (bytes_written != size)
		return 0;

	return bytes_written;
}

/**
* tell_file
* @brief 파일 포인터의 현재 위치를 반환합니다.
* @param file 파일 핸들
* @return 현재 위치
*/
size_t tell_file(
	HANDLE file
)
{
	struct OS_FILE* os_file = (struct OS_FILE*)file;
	if (!os_file || !os_file->file)
		return 0;

	long pos = ftell(os_file->file);
	if (-1 == pos)
		return 0;

	return (size_t)pos;
}

/**
* seek_file
* @brief 파일 포인터를 이동합니다.
* @param file 파일 핸들
* @param offset 이동할 오프셋
* @param origin 이동 기준
* @return 없음
*/
void seek_file(
	HANDLE file,
	size_t offset,
	FILE_SEEK origin
)
{
	struct OS_FILE* os_file = (struct OS_FILE*)file;
	if (!os_file || !os_file->file)
		return;

	int whence = SEEK_SET;
	switch (origin)
	{
	case FILE_SEEK_SET:
		whence = SEEK_SET;
		break;
	case FILE_SEEK_CUR:
		whence = SEEK_CUR;
		break;
	case FILE_SEEK_END:
		whence = SEEK_END;
		break;
	default:
		return;
	}

	fseek(os_file->file, (long)offset, whence);
}

BOOL exist_file(
	const char* filename
)
{
	return ACCESS(filename, F_OK) == 0;
}

size_t get_file_size(
	const char* filename
)
{
	if (!filename)
		return 0;

	STAT_STRUCT file_stat;

	if (STAT_FUNC(filename, &file_stat) != 0) 
		return 0;

	return (size_t)file_stat.st_size;
}

size_t get_file_size_by_handle(
	HANDLE file
)
{
	struct OS_FILE* os_file = (struct OS_FILE*)file;
	if (!os_file || !os_file->file)
		return 0;

	// 현재 위치 저장
	long current_pos = ftell(os_file->file);
	if (current_pos == -1)
		return 0;
	// 파일 끝으로 이동
	if (fseek(os_file->file, 0, SEEK_END) != 0)
		return 0;
	// 파일 크기 가져오기
	long file_size = ftell(os_file->file);

	fseek(os_file->file, current_pos, SEEK_SET);

	return (size_t)file_size;
}

BOOL delete_file(
	const char* filename
)
{
	if (!filename)
		return FALSE;

#ifdef __TARGET_COMPILER_MSC
	return DeleteFileA(filename);
#else
	return (unlink(filename) == 0) ? TRUE : FALSE;
#endif // __TARGET_COMPILER_MSC
}