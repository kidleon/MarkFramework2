#ifndef __GUID_H__
#define __GUID_H__


/**
* make_guid
* GUID를 얻어옵니다.
* @brief GUID를 얻어옵니다.
* @param ppDest GUID를 얻어올 문자 버퍼의 포인터
* @param length Dest 문저 버퍼의 사이즈
* @return GUID생성 성공시 true, 실패시 false
*/
MARKENGINE_C_API bool make_guid(
	char** ppDest, 
	size_t length
);



#endif // __GUID_H__
