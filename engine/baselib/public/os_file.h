#ifndef __OS_FILE_H__
#define __OS_FILE_H__


typedef enum
{
	FILE_MODE_READ = 0,
	FILE_MODE_WRITE,
	FILE_MODE_APPEND,
	FILE_MODE_READ_BINARY,
	FILE_MODE_WRITE_BINARY,
	FILE_MODE_APPEND_BINARY
} FILE_MODE;

typedef enum 
{
	FILE_SEEK_SET = 0,
	FILE_SEEK_CUR,
	FILE_SEEK_END
} FILE_SEEK;

/**
* open_file
* @brief 파일을 엽니다.
* @param filename 열 파일 이름
* @param mode 오픈 파일 모드
* @return 성공시 FILE 핸들, 실패시 NULL
*/
MARK_BASELIB_C_API HANDLE open_file(
	const char* filename,
	FILE_MODE mode
);

/**
* close_file
* @brief 파일을 닫습니다.
* @param file 닫을 파일 핸들
* @return 없음
*/
MARK_BASELIB_C_API void close_file(
	HANDLE file
);

/**
* read_file
* @brief 파일에서 데이터를 읽습니다.
* @param file 읽을 파일 핸들
* @param buffer 읽은 데이터를 저장할 버퍼
* @param size 읽을 데이터 크기
* @return 성공시 읽은 데이터 크기, 실패시 0
*/
MARK_BASELIB_C_API size_t read_file(
	HANDLE file,
	void* buffer,
	size_t size
);

/**
* write_file
* @brief 파일에 데이터를 씁니다.
* @param file 쓸 파일 핸들
* @param buffer 쓸 데이터 버퍼
* @param size 쓸 데이터 크기
* @return 성공시 쓴 데이터 크기, 실패시 0
*/
MARK_BASELIB_C_API size_t write_file(
	HANDLE file,
	const void* buffer,
	size_t size
);

/**
* tell_file
* @brief 파일 포인터의 현재 위치를 반환합니다.
* @param file 파일 핸들
* @return 현재 위치
*/
MARK_BASELIB_C_API size_t tell_file(
	HANDLE file
);

/**
* seek_file
* @brief 파일 포인터를 이동합니다.
* @param file 파일 핸들
* @param offset 이동할 오프셋
* @param origin 이동 기준
* @return 없음
*/
MARK_BASELIB_C_API void seek_file(
	HANDLE file,
	size_t offset,
	FILE_SEEK origin
);

MARK_BASELIB_C_API BOOL exist_file(
	const char* filename
);

#endif // __OS_FILE_H__