#pragma once


namespace mark
{
	/**
	* @brief 파일 시스템을 관리하는 클래스입니다. 이 클래스는 파일과 디렉토리의 생성, 제거, 이름 변경,
	* 존재 여부 확인, 파일 읽기/쓰기 등의 기능을 제공합니다.
	*/
	class MARKENGINE_API file_system
	{
	public:
		/**
		* @brief  파일 시스템을 초기화합니다.
		* root_path 는 파일 시스템의 루트 디렉토리를 지정하며,
		* 이후 모든 경로는 이 루트 디렉토리를 기준으로 해석됩니다.
		* @param root_path 파일 시스템의 루트 디렉토리 경로
		* @return 초기화 성공 여부. 성공하면 true, 실패하면 false를 반환합니다.
		*/
		static bool initialize(const char* root_path);

		/**
		* @brief  파일 시스템을 종료합니다. 이 함수는 파일 시스템과 관련된 모든 리소스를 해제하고 정리합니다.
		* @return 없음
		*/
		static void destroy();

		/**
		* @brief  디렉토리를 생성
		* @param path 생성할 디렉토리의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 디렉토리 생성 성공 여부. 성공하면 true, 실패하면 false를 반환합니다.
		*/
		static bool create_directory(const char* path);

		/**
		* @brief  디렉토리를 제거
		* @param path 제거할 디렉토리의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 디렉토리 제거 성공 여부. 성공하면 true, 실패하면 false를 반환합니다.
				* 디렉토리가 비어있지 않으면 제거에 실패할 수 있습니다.
		*/
		static bool remove_directory(const char* path);

		/**
		* @brief  디렉토리를 이름 변경
		* @param old_path 변경할 디렉토리의 현재 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @param new_path 변경할 디렉토리의 새 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 디렉토리 이름 변경 성공 여부. 성공하면 true, 실패하면 false를 반환합니다.
		*/
		static bool rename_directory(const char* old_path, const char* new_path);

		/**
		* @brief  디렉토리가 존재하는지 확인
		* @param path 확인할 디렉토리의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 디렉토리 존재 여부. 존재하면 true, 존재하지 않으면 false를 반환합니다.
		*/
		[[nodiscard]] static bool exist_directory(const char* path);

		/**
		* @brief  파일을 생성
		* @param path 생성할 파일의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 파일 생성 성공 여부. 성공하면 true, 실패하면 false를 반환します.
			* 이미 존재하는 파일을 생성하려고 하면 실패할 수 있습니다.
		*/
		static bool create_file(const char* path);

		/**
		* @brief  파일을 제거
		* @param path 제거할 파일의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 파일 제거 성공 여부. 성공하면 true, 실패하면 false를 반환します.
			* 파일이 존재하지 않으면 제거에 실패할 수 있습니다.
		*/
		static bool remove_file(const char* path);

		/**
		* @brief  파일이 존재하는지 체크
		* @param path 체크할 파일의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @return 파일 존재 여부. 존재하면 true, 존재하지 않으면 false를 반환します.
		*/
		[[nodiscard]] static bool exist_file(const char* path);


		/**
		* @brief  파일을 열고 핸들을 반환
		* @param path 열 파일의 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다.
		* @param flags 파일 열기 옵션을 나타내는 플래그입니다. SEEK_SET, SEEK_CUR, SEEK_END 등의 값을 사용할 수 있습니다.
		* @return 파일 핸들. 파일을 성공적으로 열면 유효한 핸들을 반환하고, 실패하면 INVALID_HANDLE_VALUE 또는 nullptr을 반환할 수 있습니다.
			* 반환된 핸들은 사용 후 반드시 close_file 함수를 통해 닫아야 합니다.
		*/
		[[nodiscard]] static HANDLE open_file(const char* path, uint32_t flags);

		/**
		* @brief  파일 핸들을 닫습니다. open_file 함수를 통해 얻은 핸들을 사용한 후에는 반드시 이 함수를 호출하여 리소스를 해제해야 합니다.
		* @param file_handle 닫을 파일 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다.
		* @return 없음. 
		*/
		static void close_file(HANDLE file_handle);

		/**
		* @brief  파일의 이름을 변경합니다. old_path에 해당하는 파일이 존재해야 하며, new_path에 해당하는 파일이 존재하지 않아야 합니다.
		* @param old_path 변경할 파일의 현재 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다。
		* @param new_path 변경할 파일의 새 경로. 루트 디렉토리를 기준으로 한 상대 경로입니다。
		* @return 파일 이름 변경 성공 여부. 성공하면 true, 실패하면 false를 반환します。
		*/
		static bool rename_file(const char* old_path, const char* new_path);

		/**
		* @brief  파일에서 데이터를 읽습니다.
		* @param file_handle 읽을 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다.
		* @param buffer 데이터를 읽어올 버퍼입니다. 충분한 크기의 버퍼를 전달해야 합니다.
		* @param bytes_to_read 읽을 바이트 수입니다. buffer가 이 크기만큼 데이터를 저장할 수 있어야 합니다.
		* @return 실제로 읽은 바이트 수입니다. 읽기에 실패하면 0을 반환할 수 있습니다.
		*/
		static size_t read_file(HANDLE file_handle, void* buffer, size_t bytes_to_read);

		/**
		* @brief  파일에 데이터를 씁니다.
		* @param file_handle 쓸 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다.
		* @param buffer 쓸 데이터가 저장된 버퍼입니다. bytes_to_write 크기만큼 데이터를 제공해야 합니다.
		* @param bytes_to_write 쓸 바이트 수입니다. buffer가 이 크기만큼 데이터를 제공해야 합니다.
		* @return 실제로 쓴 바이트 수입니다. 쓰기에 실패하면 0을 반환할 수 있습니다.
			 * 파일이 쓰기 가능한 모드로 열려 있어야 하며, 그렇지 않으면 쓰기에 실패할 수 있습니다.
		*/
		static size_t write_file(HANDLE file_handle, const void* buffer, size_t bytes_to_write);

		/**
		* @brief  파일 포인터를 이동합니다. offset은 origin에서부터의 바이트 오프셋을 나타냅니다.
		* origin은 SEEK_SET, SEEK_CUR, SEEK_END 중 하나일 수 있습니다.
		* @param file_handle 이동할 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다.
		* @param offset 이동할 바이트 오프셋입니다. origin에서부터의 상대적인 위치를 나타냅니다.
		* @param origin 오프셋의 기준이 되는 위치입니다. SEEK_SET(파일의 시작), SEEK_CUR(현재 위치), SEEK_END(파일의 끝) 중 하나를 사용할 수 있습니다。
		* @return 파일 포인터 이동 성공 여부. 성공하면 true, 실패하면 false를 반환합니다. 이동에 실패하면 파일 포인터의 위치는 변경되지 않습니다。
		 * 파일이 열려 있어야 하며, 그렇지 않으면 이동에 실패할 수 있습니다。
		*/
		static bool seek_file(HANDLE file_handle, int64_t offset, uint32_t origin);

		/**
		* @brief  현재 파일 포인터의 위치를 반환합니다. 파일의 시작부터 현재 위치까지의 바이트 오프셋을 나타냅니다。
		* @param file_handle 위치를 확인할 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다。
		* @return 현재 파일 포인터의 위치입니다.
		*/
		[[nodiscard]] static size_t tell_file(HANDLE file_handle);

		/**
		* @brief  파일의 크기를 반환합니다
		* @@param file_handle 크기를 확인할 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다。
		* @return 파일의 크기입니다. 파일 크기를 가져오는 데 실패하면 0을 반환할 수 있습니다。
		*/
		[[nodiscard]] static size_t get_file_size(HANDLE file_handle);

		/**
		* @brief  파일의 모든 내용을 읽어서 버퍼에 저장합니다. 이 함수는 파일의 크기만큼 버퍼를 자동으로 할당하고, 파일의 내용을 그 버퍼에 읽어옵니다。
		* @param file_handle 읽을 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다。
		* @return 파일의 모든 내용을 읽는 데 성공하면 true를 반환하고, 실패하면 false를 반환합니다。
		*/
		static bool read_all(HANDLE file_handle);

		/**
		* @brief  read_all 함수를 통해 읽은 파일의 모든 내용을 저장한 버퍼를 반환합니다。
		* 이 버퍼는 read_all 함수가 성공적으로 파일을 읽은 후에만 유효합니다。
		* @param file_handle 버퍼를 가져올 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다。
		* @return read_all 함수를 통해 읽은 파일의 모든 내용을 저장한 버퍼입니다。
		*/
		[[nodiscard]] static void* get_read_all_buffer(HANDLE file_handle);

		/**
		* @brief  read_all 함수를 통해 읽은 파일의 모든 내용의 크기를 반환합니다。
		* @param file_handle 크기를 가져올 파일의 핸들입니다. open_file 함수를 통해 얻은 유효한 핸들을 전달해야 합니다。
		* @return read_all 함수를 통해 읽은 파일의 모든 내용의 크기입니다。
		*/
		[[nodiscard]] static size_t get_read_all_size(HANDLE file_handle);

		/**
		* @brief  파일 경로에서 확장자를 추출하여 out_extension 버퍼에 저장
		* @param path 확장자를 추출할 파일 경로. 루트 디렉토리 기준 상대경로
		* @param out_extension 추출된 확장자를 저장할 버퍼
		* @param out_size out_extension 버퍼의 크기
		* @return 없음
		*/
		static void get_file_extension(
			const char* path,
			char* out_extension,
			size_t out_size
		);

		/**
		* @brief  파일 경로에서 확장자를 변경하여 out_filename 버퍼에 저장
		* @param path 확장자를 변경할 파일 경로. 루트 디렉토리 기준 상대경로
		* @param new_extension 변경할 새 확장자 (예: ".txt"). 점(.)을 포함해야 합니다.
		* @param out_filename 변경된 파일 경로를 저장할 버퍼
		* @param out_size out_filename 버퍼의 크기
		* @return 없음
		*/
		static void change_extension(
			const char* path,
			const char* new_extension,
			char* out_filename,
			size_t out_size
		);

		/**
		* @brief  파일 경로에서 확장자를 제거하여 out_filename 버퍼에 저장
		* @param path 확장자를 제거할 파일 경로. 루트 디렉토리 기준 상대경로
		* @param out_filename 확장자가 제거된 파일 경로를 저장할 버퍼
		* @param out_size out_filename 버퍼의 크기
		* @return 없음
		*/
		static void get_filename_without_extension(
			const char* path,
			char* out_filename,
			size_t out_size
		);

		/**
		* @brief  파일 경로에서 파일 이름을 추출하여 out_filename 버퍼에 저장
		* @param path 파일 이름을 추출할 파일 경로. 루트 디렉토리 기준 상대경로
		* @param out_filename 추출된 파일 이름을 저장할 버퍼
		* @param out_size out_filename 버퍼의 크기
		* @return 없음
		*/
		static void get_filename(
			const char* path,
			char* out_filename,
			size_t out_size
		);

		/**
		* @brief  파일 경로에서 디렉토리 경로를 추출하여 out_directory 버퍼에 저장
		* @param path 디렉토리 경로를 추출할 파일 경로. 루트 디렉토리 기준 상대경로
		* @param out_directory 추출된 디렉토리 경로를 저장할 버퍼
		* @param out_size out_directory 버퍼의 크기
		* @param without_root true로 설정하면 루트 디렉토리를 제외한 경로를 반환합니다.
		*/
		static void get_directory(
			const char* path,
			char* out_directory,
			size_t out_size,
			bool without_root
		);

	};
}
