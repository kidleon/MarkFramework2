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


#endif // __MPACK_FILE_H__
