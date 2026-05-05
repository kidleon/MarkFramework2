#include "pch.h"
#include "file_system.h"
#include <filesystem>
#include <stdio.h>
#include "core_heap.h"



namespace mark
{
	struct file_system_info
	{
		char root_path[_MAX_PATH];
		bool initialized;
	};

	struct file_handle_info
	{
		FILE* fp;
		size_t read_buffer_size;
	};

	static file_system_info s_file_system_info = { 0 };

	// 경로 구분자를 '\'에서 '/'로 변경하는 유틸리티 함수
	inline static void change_slash_to_backslash(char* path, size_t size)
	{
		for (size_t i = 0; i < size && path[i] != '\0'; ++i)
		{
			if (path[i] == '\\')
				path[i] = '/';
		}
	}

	bool file_system::initialize(const char* root_path)
	{
		if (s_file_system_info.initialized)
			return false;

		size_t copy_size = safe_strcpy(
			s_file_system_info.root_path,
			sizeof(s_file_system_info.root_path),
			root_path
		);

		if (SIZE_MAX == copy_size)
			return false;

#if defined(__TARGET_OS_WINDOWS)
		// Windows에서는 경로 구분자로 '\'를 사용하지만, 내부적으로는 '/'로 통일하여 처리하기 위해 변경
		change_slash_to_backslash(s_file_system_info.root_path, sizeof(s_file_system_info.root_path));
#endif // #if defined(__TARGET_OS_WINDOWS)

		size_t root_path_size = strnlen(s_file_system_info.root_path, sizeof(s_file_system_info.root_path));

		// root_path 끝에 '/'가 없으면 추가
		if (strlen(s_file_system_info.root_path) > 0 && s_file_system_info.root_path[strlen(s_file_system_info.root_path) - 1] != '/')
		{
			// '/'를 추가하기 전에 root_path가 너무 길어서 추가할 공간이 있는지 확인
			if (strlen(s_file_system_info.root_path) < sizeof(s_file_system_info.root_path) - 1)
			{
				s_file_system_info.root_path[strlen(s_file_system_info.root_path)] = '/';
				s_file_system_info.root_path[strlen(s_file_system_info.root_path) + 1] = '\0';
			}
			else
			{
				return false; // root path가 너무 길어서 '/'를 추가할 공간이 없음
			}
		}

		s_file_system_info.initialized = true;

		return true;
	};

	void file_system::destroy()
	{
		s_file_system_info.initialized = false;
		s_file_system_info.root_path[0] = '\0';
	}

	bool file_system::create_directory(const char* path)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return false;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		std::error_code ec;
		bool result = std::filesystem::create_directories(full_path, ec);

		return result && !ec;
	}

	bool file_system::remove_directory(const char* path)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return false;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		std::error_code ec;
		bool result = std::filesystem::remove_all(full_path, ec) > 0;

		return result && !ec;
	}

	bool file_system::rename_directory(const char* old_path, const char* new_path)
	{
		if (!s_file_system_info.initialized || !old_path || old_path[0] == '\0' || !new_path || new_path[0] == '\0')
			return false;

		std::filesystem::path full_old_path = std::filesystem::path(s_file_system_info.root_path) / old_path;
		std::filesystem::path full_new_path = std::filesystem::path(s_file_system_info.root_path) / new_path;

		std::error_code ec;
		std::filesystem::rename(full_old_path, full_new_path, ec);

		return !ec;
	}

	bool file_system::exist_directory(const char* path)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return false;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		return std::filesystem::exists(full_path) && std::filesystem::is_directory(full_path);
	}

	bool file_system::create_file(const char* path)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return false;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		// 이미 파일이 존재하면 실패
		if (std::filesystem::exists(full_path))
			return false;

		FILE* fp = std::fopen(full_path.string().c_str(), "wb");
		if (!fp)
			return false;

		fclose(fp);

		return true;
	}

	bool file_system::remove_file(const char* path)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return false;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		// 파일이 존재하지 않거나 일반 파일이 아니면 실패
		if (!std::filesystem::exists(full_path) || !std::filesystem::is_regular_file(full_path))
			return false;

		std::error_code ec;
		bool result = std::filesystem::remove(full_path, ec);

		return result && !ec;
	}

	bool file_system::exist_file(const char* path)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return false;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		return std::filesystem::exists(full_path) && std::filesystem::is_regular_file(full_path);
	}

	HANDLE file_system::open_file(const char* path, ACESS_MODE access_mode)
	{
		if (!s_file_system_info.initialized || !path || path[0] == '\0')
			return nullptr;

		std::filesystem::path full_path = std::filesystem::path(s_file_system_info.root_path) / path;

		const char* mode = nullptr;
		switch (access_mode)
		{
		case ACESS_MODE::READ:
			mode = "rb";
			break;

		case ACESS_MODE::WRITE:
			mode = "wb";
			break;

		case ACESS_MODE::READ_WRITE:
			mode = "r+b";
			break;

		default:
			return nullptr; // 지원하지 않는 플래그
		}
			
		FILE* fp = fopen(full_path.string().c_str(), mode);
		if (!fp)
			return nullptr;

		file_handle_info* handle = (file_handle_info*)sys_alloc(sizeof(file_handle_info), DEFAULT_ALIGNMENT);

		handle->fp = fp;
		handle->read_buffer_size = 0;

		return handle;
	}

	void file_system::close_file(HANDLE file_handle)
	{
		if (!file_handle)
			return;

		file_handle_info* handle = (file_handle_info*)file_handle;

		if (handle->fp)
			std::fclose(handle->fp);

		sys_free(handle);
	}

	bool file_system::rename_file(const char* old_path, const char* new_path)
	{
		if (!s_file_system_info.initialized || !old_path || old_path[0] == '\0' || !new_path || new_path[0] == '\0')
			return false;

		std::filesystem::path full_old_path = std::filesystem::path(s_file_system_info.root_path) / old_path;
		std::filesystem::path full_new_path = std::filesystem::path(s_file_system_info.root_path) / new_path;

		std::error_code ec;
		std::filesystem::rename(full_old_path, full_new_path, ec);

		return !ec;
	}

	size_t file_system::read_file(HANDLE file_handle, void* buffer, size_t bytes_to_read)
	{
		if (!file_handle || !buffer || 0 == bytes_to_read)
			return 0;

		file_handle_info* handle = (file_handle_info*)file_handle;

		size_t read_bytes = fread(buffer, 1, bytes_to_read, handle->fp);

		return read_bytes;
	}

	size_t file_system::write_file(HANDLE file_handle, const void* buffer, size_t bytes_to_write)
	{
		if (!file_handle || !buffer || 0 == bytes_to_write)
			return 0;

		file_handle_info* handle = (file_handle_info*)file_handle;

		size_t written_bytes = fwrite(buffer, 1, bytes_to_write, handle->fp);

		return written_bytes;
	}

	bool file_system::seek_file(HANDLE file_handle, int64_t offset, uint32_t origin)
	{
		if (!file_handle)
			return false;

		file_handle_info* handle = (file_handle_info*)file_handle;

		int origin_flag;
		switch (origin)
		{
		case 0: // SEEK_SET
			origin_flag = SEEK_SET;
			break;
		case 1: // SEEK_CUR
			origin_flag = SEEK_CUR;
			break;
		case 2: // SEEK_END
			origin_flag = SEEK_END;
			break;
		default:
			return false; // 지원하지 않는 origin
		}

		if (fseek(handle->fp, (long)offset, origin_flag) != 0)
			return false;

		return true;
	}

	size_t file_system::tell_file(HANDLE file_handle)
	{
		if (!file_handle)
			return 0;

		file_handle_info* handle = (file_handle_info*)file_handle;

		long pos = ftell(handle->fp);
		if (pos < 0)
			return 0;

		return static_cast<size_t>(pos);
	}

	void* file_system::read_all(HANDLE temppool_handle, HANDLE file_handle, size_t& read_size)
	{
		read_size = 0;

		if (!file_handle)
			return nullptr;

		file_handle_info* handle = (file_handle_info*)file_handle;

		// 현재 파일 위치 저장
		long current_pos = ftell(handle->fp);
		if (current_pos < 0)
			return nullptr;

		if (fseek(handle->fp, 0, SEEK_END) != 0)
		{
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		long file_size = ftell(handle->fp); // 파일 크기 계산
		if (file_size < 0)
		{
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		// 파일 위치를 처음으로 되돌림
		if (fseek(handle->fp, 0, SEEK_SET) != 0)
		{
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		void* buffer = temppool_alloc(temppool_handle, file_size, sizeof(uintptr_t));

		size_t read_bytes = fread(buffer, 1, file_size, handle->fp);
		if (read_bytes != static_cast<size_t>(file_size))
		{
			handle->read_buffer_size = 0;
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		read_size = read_bytes;

		return buffer;
	}

	void* file_system::read_all(HANDLE file_handle, size_t& read_size)
	{
		read_size = 0;

		if (!file_handle)
			return nullptr;

		file_handle_info* handle = (file_handle_info*)file_handle;

		// 현재 파일 위치 저장
		long current_pos = ftell(handle->fp);
		if (current_pos < 0)
			return nullptr;

		// 파일 크기 계산
		if (fseek(handle->fp, 0, SEEK_END) != 0)
		{
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		long file_size = ftell(handle->fp);
		if (file_size < 0)
		{
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		// 파일 위치를 처음으로 되돌림
		if (fseek(handle->fp, 0, SEEK_SET) != 0)
		{
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		// 읽기 버퍼 할당
		char* read_buffer = (char*)sys_alloc(file_size, alignof(file_handle_info));
		if (!read_buffer)
		{
			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);
			return nullptr;
		}

		handle->read_buffer_size = file_size;
		size_t read_bytes = fread(read_buffer, 1, file_size, handle->fp);

		if (read_bytes != static_cast<size_t>(file_size))
		{
			sys_free(read_buffer);
			handle->read_buffer_size = 0;

			// 파일 위치를 원래대로 되돌림
			fseek(handle->fp, current_pos, SEEK_SET);

			return nullptr;
		}

		read_size = read_bytes;

		return read_buffer;
	}

	void file_system::get_file_extension(
		const char* path,
		char* out_extension,
		size_t out_size
	)
	{
		if (!path || path[0] == '\0' || !out_extension || out_size == 0)
		{
			if (out_extension && out_size > 0)
				out_extension[0] = '\0';
			return;
		}

		// 일단 빈 문자열로 초기화 (확장자 없을 때의 기본값)
		out_extension[0] = '\0';

		// 경로에서 파일명 시작 위치를 찾습니다.
		// '/' 또는 '\\' 이후의 마지막 세그먼트가 파일명입니다.
		const char* filename = path;
		for (const char* p = path; *p != '\0'; ++p)
		{
			if (*p == '/' || *p == '\\')
				filename = p + 1; // 구분자 다음 문자부터 파일명 시작
		}

		// 파일명에서 마지막 '.'의 위치를 찾습니다.
		const char* dot = nullptr;
		for (const char* p = filename; *p != '\0'; ++p)
		{
			if (*p == '.')
				dot = p; // 마지막 '.'의 위치를 계속 갱신
		}

		// 확장자가 없는 경우들을 걸러냅니다:
		//  1. '.'이 아예 없는 경우          (예: "Makefile")
		//  2. 숨김 파일인 경우               (예: ".gitignore" → dot == filename)
		//  3. '.'이 파일명 맨 끝인 경우      (예: "file.")
		if (dot == nullptr || dot == filename || dot[1] == '\0')
			return;

		// 확장자를 out_extension 버퍼에 복사합니다. (널 문자 포함 out_size 만큼만)
		// C++20의 strncpy_s 대신 범용성을 위해 직접 복사합니다.
		size_t i = 0;
		while (dot[i] != '\0' && i < out_size - 1) // 마지막 자리는 널 문자용으로 예약
		{
			out_extension[i] = dot[i];
			++i;
		}

		out_extension[i] = '\0'; // 문자열 종료
	}

	void file_system::change_extension(
		const char* path,
		const char* new_extension,
		char* out_filename,
		size_t out_size
	)
	{
		if (!path || path[0] == '\0' || !new_extension || new_extension[0] == '\0' || !out_filename || out_size == 0)
		{
			if (out_filename && out_size > 0)
				out_filename[0] = '\0';
			return;
		}
		std::filesystem::path p(path);
		std::filesystem::path new_p = p.replace_extension(new_extension);
		safe_strcpy(out_filename, out_size, new_p.string().c_str());
	}

	void file_system::get_filename_without_extension(
		const char* path,
		char* out_filename,
		size_t out_size
	)
	{
		if (!path || path[0] == '\0' || !out_filename || out_size == 0)
		{
			if (out_filename && out_size > 0)
				out_filename[0] = '\0';
			return;
		}

		const char* filename = path;
		for (const char* p = path; *p != '\0'; ++p)
		{
			if (*p == '/' || *p == '\\')
				filename = p + 1; // 구분자 다음 문자부터 파일명 시작
		}

		const char* dot = nullptr;
		for (const char* p = filename; *p != '\0'; ++p)
		{
			if (*p == '.')
				dot = p; // 마지막 '.'의 위치를 계속 갱신
		}

		const bool has_valid_extension =
			(dot != nullptr) &&  // '.'이 존재
			(dot != filename) && // 숨김 파일 아님 (예: ".gitignore")
			(dot[1] != '\0');    // '.'이 맨 끝이 아님 (예: "file.")

		// 복사 끝 위치: 확장자가 있으면 dot 앞까지, 없으면 문자열 끝까지
		const char* end = has_valid_extension ? dot : (filename + strlen(filename));

		size_t i = 0;
		while ((filename + i) < end && i < out_size - 1)
		{
			out_filename[i] = filename[i];
			++i;
		}

		out_filename[i] = '\0'; // 문자열 종료
	}

	void file_system::get_filename(
		const char* path,
		char* out_filename,
		size_t out_size
	)
	{
		if (!path || path[0] == '\0' || !out_filename || out_size == 0)
		{
			if (out_filename && out_size > 0)
				out_filename[0] = '\0';
			return;
		}

		const char* filename = path;
		for (const char* p = path; *p != '\0'; ++p)
		{
			if (*p == '/' || *p == '\\')
				filename = p + 1; // 구분자 다음 문자부터 파일명 시작
		}

		safe_strcpy(out_filename, out_size, filename);
	}

	void file_system::get_directory(
		const char* path,
		char* out_directory,
		size_t out_size,
		bool without_root
	)
	{
		if (!path || path[0] == '\0' || !out_directory || out_size == 0)
		{
			if (out_directory && out_size > 0)
				out_directory[0] = '\0';
			return;
		}

		const char* last_slash = nullptr;
		for (const char* p = path; *p != '\0'; ++p)
		{
			if (*p == '/' || *p == '\\')
				last_slash = p; // 마지막 구분자의 위치를 계속 갱신
		}

		if (!last_slash)
		{
			out_directory[0] = '\0'; // 구분자가 없으면 디렉토리 부분은 빈 문자열
			return;
		}

		size_t dir_length = last_slash - path + 1; // 구분자까지 포함한 길이
		if (dir_length >= out_size)
			dir_length = out_size - 1; // 버퍼 크기에 맞게 조정

		strncpy(out_directory, path, dir_length);

		out_directory[dir_length] = '\0'; // 문자열 종료

		if (without_root && s_file_system_info.initialized)
		{
			size_t root_length = strlen(s_file_system_info.root_path);

			if (strncmp(out_directory, s_file_system_info.root_path, root_length) == 0)
			{
				size_t remaining_length = dir_length - root_length;

				if (remaining_length < out_size)
				{
					memmove(out_directory, out_directory + root_length, remaining_length);
					out_directory[remaining_length] = '\0';
				}
				else
				{
					out_directory[0] = '\0'; // 루트 경로가 버퍼 크기보다 크면 빈 문자열로 처리
				}
			}
		}
	}

	bool file_system::combine_path(
		const char* path1,
		const char* path2,
		char* out_path,
		size_t out_size
	)
	{
		if (!path1 || path1[0] == '\0' || !path2 || path2[0] == '\0' || !out_path || out_size == 0)
		{
			return false;
		}

		std::filesystem::path p1(path1);
		std::filesystem::path p2(path2);
		std::filesystem::path combined = p1 / p2;
		safe_strcpy(out_path, out_size, combined.string().c_str());

		return true;
	}


}
