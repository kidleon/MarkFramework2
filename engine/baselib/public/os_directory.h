#ifndef __OS_DIRECTORY_H__
#define __OS_DIRECTORY_H__


struct MARK_BASELIB_API directory_t
{
	char name[MAX_FILE_LENGTH];
	size_t num_files;
	size_t num_subdirs;
	char** file_list;
	char** subdir_list;
};

// 디렉토리 생성
MARK_BASELIB_C_API BOOL create_dir(
	const char* dirname
);

// 디렉토리 삭제
MARK_BASELIB_C_API BOOL delete_dir(
	const char* dirname,
	BOOL delete_files,
	BOOL delete_subdirs
);

// 디렉토리 존재 여부 확인
MARK_BASELIB_C_API BOOL exist_dir(
	const char* dirname
);

// 디렉토리 내 서브디렉토리 개수 조회
MARK_BASELIB_C_API size_t get_subdir_count(
	const char* dirname
);

// 디렉토리 내 파일 개수 조회
MARK_BASELIB_C_API size_t get_file_count(
	const char* dirname
);

// 디렉토리 내 파일 및 서브디렉토리 목록 열람
MARK_BASELIB_C_API BOOL get_dir_contents(
	const char* dirname,
	struct directory_t* dir_info,
	void* (*alloc_func)(size_t num_elem, size_t count),
	void (*free_func)(void* p)
);
	


#endif // __OS_DIRECTORY_H__
