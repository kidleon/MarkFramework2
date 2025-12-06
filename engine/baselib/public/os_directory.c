#include "pch.h"
#include "os_directory.h"
#include "strings.h"


static BOOL build_search_pattern(const char* dirname, char* out, size_t outSize)
{
    if (!dirname || !out || outSize == 0)
        return FALSE;

    size_t len = strlen(dirname);
    if (len + 3 >= outSize)  // "\*" 포함 여유
        return FALSE;

    // 마지막 문자가 '\\' 또는 '/' 인지 확인
    if (len > 0 && (dirname[len - 1] == '\\' || dirname[len - 1] == '/')) {
        // 그대로 사용
        if (snprintf(out, outSize, "%s*", dirname) < 0)
            return FALSE;
    }
    else {
        if (snprintf(out, outSize, "%s\\*", dirname) < 0)
            return FALSE;
    }

    return TRUE;
}

// "." 또는 ".." 디렉토리인지 확인
static BOOL is_dot_or_dotdot(const char* name)
{
    return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
}

// 디렉토리 생성
BOOL create_dir(
    const char* dirname
)
{
    if (!dirname || !*dirname)
        return FALSE;

    if (CreateDirectoryA(dirname, NULL)) 
    {
        return TRUE;
    }
    else 
    {
        DWORD err = GetLastError();

        // 이미 존재하면 "성공"으로 간주할지 여부는 정책에 따라 다를 수 있음

        if (err == ERROR_ALREADY_EXISTS) 
        {
            // 이미 디렉토리가 존재하는 경우에도 성공으로 처리
            DWORD attr = GetFileAttributesA(dirname);
            if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                return TRUE;
            }
        }

        return FALSE;
    }
}

// 디렉토리 존재 여부 확인
BOOL exist_dir(
    const char* dirname
)
{
    if (!dirname || !*dirname)
        return FALSE;

    DWORD attr = GetFileAttributesA(dirname);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return FALSE;

    return (attr & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
}

// 디렉토리 내 서브디렉토리 개수 조회
size_t get_subdir_count(
    const char* dirname
)
{
    if (!dirname || !*dirname)
        return 0;

    char searchPattern[MAX_PATH];
    if (!build_search_pattern(dirname, searchPattern, sizeof(searchPattern)))
        return 0;

    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA(searchPattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        return 0;

    size_t count = 0;

    do 
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            if (!is_dot_or_dotdot(ffd.cFileName)) 
            {
                ++count;
            }
        }
    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);

    return count;
}

// 디렉토리 내 파일 개수 조회 (일반 파일만)
size_t get_file_count(
    const char* dirname
)
{
    if (!dirname || !*dirname)
        return 0;

    char searchPattern[MAX_PATH];
    if (!build_search_pattern(dirname, searchPattern, sizeof(searchPattern)))
        return 0;

    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA(searchPattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        return 0;

    size_t count = 0;

    do 
    {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
        {
            ++count;
        }
    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);
    return count;
}

static BOOL get_dir_contents_count(
    const char* dirname, 
    size_t* num_subdir, 
    size_t* num_files
)
{
    if (!dirname || !*dirname || !num_subdir || !num_files)
        return FALSE;

    char searchPattern[MAX_PATH];
    if (!build_search_pattern(dirname, searchPattern, sizeof(searchPattern)))
        return 0;

    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA(searchPattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        return 0;

    *num_subdir = 0;
    *num_files = 0;

    do
    {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            ++(*num_files);
        }
        else
        {
            if (!is_dot_or_dotdot(ffd.cFileName))
            {
                ++(*num_subdir);
            }
        }

    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);

    return TRUE;
}

// 내부용: 디렉토리 전체 삭제 (재귀) delete_files / delete_subdirs 정책을 반영
static BOOL delete_dir_internal(
    const char* dirname,
    BOOL delete_files,
    BOOL delete_subdirs
)
{
    char searchPattern[MAX_PATH];
    if (!build_search_pattern(dirname, searchPattern, sizeof(searchPattern)))
        return FALSE;

    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA(searchPattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        // 비어있는 폴더이거나, 접근불가/존재X
        // 존재하지 않으면 exist_dir에서 이미 필터링하는 것이 좋음
        // 여기서는 "없는 디렉토리"는 실패로 취급
        return exist_dir(dirname) ? FALSE : FALSE;
    }

    BOOL result = TRUE;

    do 
    {
        const char* name = ffd.cFileName;
        if (is_dot_or_dotdot(name))
            continue;

        char path[MAX_PATH];
        int n = snprintf(path, sizeof(path), "%s\\%s", dirname, name);
        if (n < 0 || n >= (int)sizeof(path)) 
        {
            result = FALSE;
            break;
        }

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            // 서브 디렉토리
            if (!delete_subdirs) 
            {
                result = FALSE;
                break;
            }

            if (!delete_dir_internal(path, delete_files, delete_subdirs)) 
            {
                result = FALSE;
                break;
            }

            // 서브 디렉토리 삭제
            if (!RemoveDirectoryA(path)) 
            {
                result = FALSE;
                break;
            }
        }
        else
        {
            // 파일
            if (!delete_files) 
            {
                result = FALSE;
                break;
            }

            if (!DeleteFileA(path)) 
            {
                result = FALSE;
                break;
            }
        }

    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);

    return result;
}

// 디렉토리 삭제 (옵션에 따라 파일/서브디렉토리 삭제)
BOOL delete_dir(
    const char* dirname,
    BOOL delete_files,
    BOOL delete_subdirs
)
{
    if (!dirname || !*dirname)
        return FALSE;

    if (!exist_dir(dirname))
        return FALSE;

    // 내부에서 재귀적으로 내용 삭제
    if (!delete_dir_internal(dirname, delete_files, delete_subdirs)) 
    {
        return FALSE;
    }

    // 마지막으로 자기 자신 디렉토리 삭제
    if (!RemoveDirectoryA(dirname)) 
    {
        return FALSE;
    }

    return TRUE;
}

// 디렉토리 내 파일 및 서브디렉토리 목록 열람
//  - file_list[i] / dir_list[i] 는 "충분한 크기의 버퍼"가 미리 할당돼 있다고 가정
//  - 호출 전 get_file_count(), get_subdir_count() 로 개수 파악 후
//    그 개수만큼의 배열/버퍼를 준비하는 패턴 사용을 권장
BOOL get_dir_contents(
    const char* dirname,
    struct directory_t* dir_info,
    void* (*alloc_func)(size_t num_elem, size_t count),
    void (*free_func)(void* p)
)
{
	if (!dirname || !*dirname || !alloc_func || !free_func || !dir_info)
        return FALSE;

    char searchPattern[MAX_PATH];
    if (!build_search_pattern(dirname, searchPattern, sizeof(searchPattern)))
        return FALSE;

    if (!get_dir_contents_count(dirname, &dir_info->num_subdirs, &dir_info->num_files))
        return FALSE;

    if (dir_info->num_files > 0)
    {
        dir_info->file_list = (char**)alloc_func(MAX_FILE_LENGTH, dir_info->num_files);
        if (!dir_info->file_list)
            return FALSE;
    }
    else
    {
        dir_info->file_list = NULL;
    }
	
    if (dir_info->num_subdirs > 0)
    {
        dir_info->subdir_list = (char**)alloc_func(MAX_FILE_LENGTH, dir_info->num_subdirs);
        if (!dir_info->subdir_list)
        {
            free_func(dir_info->file_list);
            return FALSE;
        }
    }
    else
    {
        dir_info->subdir_list = NULL;
    }

    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA(searchPattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        return FALSE;

    size_t file_index = 0;
    size_t dir_index = 0;

    do
    {
        const char* name = ffd.cFileName;
        if (is_dot_or_dotdot(name))
            continue;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            if (dir_info->subdir_list && dir_info->subdir_list[dir_index])
            {
                // MAX_PATH 길이의 버퍼라고 가정
                fstrlcpy(dir_info->subdir_list[dir_index], name, MAX_FILE_LENGTH - 1);

				size_t len = strlen(dir_info->subdir_list[dir_index]);
				if (len > 0)
                    dir_info->subdir_list[dir_index][len + 1] = '\0';
                ++dir_index;
            }
        }
        else 
        {
            if (dir_info->file_list && dir_info->file_list[file_index])
            {
                fstrlcpy(dir_info->file_list[file_index], name, MAX_FILE_LENGTH - 1);

				size_t len = strlen(dir_info->file_list[file_index]);
                if (len > 0)
					dir_info->file_list[file_index][len + 1] = '\0';

                ++file_index;
            }
        }

    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);

    return TRUE;
}