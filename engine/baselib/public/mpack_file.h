#ifndef __MPACK_FILE_H__
#define __MPACK_FILE_H__


/**
* @brief MPACK 파일 생성
* @param name MPACK 이름
* @param filepath 생성할 MPACK 파일 경로
* @param password MPACK 패스워드
* @param uuid MPACK UUID
* @return 성공시 TRUE, 실패시 FALSE
*/
MARK_BASELIB_C_API BOOL mpack_file_create(
	const char* name,
	const char* filepath,
	const char* password,
	const char* uuid
);

/**
* @brief MPACK 파일 유효성 검사. 헤더 사이즈와 매직 넘버 만을 검사 하므로 완전한 검증은 아님.
* @param filepath 검사할 MPACK 파일 경로
* @return 유효한 MPACK 파일일 경우 TRUE, 그렇지 않을 경우 FALSE
*/
MARK_BASELIB_C_API BOOL mpack_file_check(
	const char* filepath
);

MARK_BASELIB_C_API HANDLE mpack_file_open(
	const char* filepath,
	const char* password
);

#endif // __MPACK_FILE_H__
