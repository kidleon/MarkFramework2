#ifndef __IDGEN_H__
#define __IDGEN_H__


#define INVALID_UNIQUEID 0xFFFFFFFF

/**
* @brief ID 생성기를 생성합니다. min_id와 max_id 사이의 ID를 생성합니다.
* @param min_id 생성할 ID의 최소값
* @param max_id 생성할 ID의 최대값
* @return 생성된 ID 생성기의 핸들을 반환합니다. 실패시 NULL을 반환합니다.
* @note 생성된 ID 생성기는 min_id와 max_id 사이의 ID를 순차적으로 생성합니다. 생성된 ID는 idgen_release 함수를 통해 반환할 수 있습니다.
*/
MARK_BASELIB_C_API HANDLE idgen_create(
	UINT32 min_id,
	UINT32 max_id
);

/**
* @brief ID 생성기를 파괴합니다. 생성된 ID 생성기를 사용한 후에는 반드시 이 함수를 호출하여 메모리를 해제해야 합니다.
* @param idgen ID 생성기의 핸들
* @return 없음
* @note idgen_create 함수를 통해 생성된 ID 생성기를 파괴합니다. 이 함수를 호출하기 전에 idgen_release 함수를 통해 모든 생성된 ID를 반환해야 합니다.
*/
MARK_BASELIB_C_API void idgen_destroy(
	HANDLE idgen
);

/**
* @brief ID 생성기를 통해 ID를 생성합니다. 생성된 ID는 min_id와 max_id 사이의 값입니다.
* @param idgen ID 생성기의 핸들
* @return 생성된 ID를 반환합니다. 만약 모든 ID가 사용되었다면 INVALID_UNIQUEID를 반환합니다.
* @note 생성된 ID는 idgen_release 함수를 통해 반환할 수 있습니다. 이 함수는 순차적으로 ID를 생성하며, 모든 ID가 사용되면 INVALID_UNIQUEID를 반환합니다.
*/
MARK_BASELIB_C_API UINT32 idgen_getid(
	HANDLE idgen
);

/**
* @brief 생성된 ID를 반환합니다. 반환된 ID는 다시 사용될 수 있습니다.
* @param idgen ID 생성기의 핸들
* @param id 반환할 ID 값
* @return 없음
* @note idgen_getid 함수를 통해 생성된 ID를 반환합니다. 이 함수를 호출하지 않으면 해당 ID는 더 이상 사용되지 않습니다. 반환된 ID는 다시 idgen_getid 함수를 통해 생성될 수 있습니다.
*/
MARK_BASELIB_C_API void idgen_release(
	HANDLE idgen,
	UINT32 id
);


#endif // __IDGEN_H__