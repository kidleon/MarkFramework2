#include "pch.h"
#include "guid.h"
#include "strings.h"

#if defined(__TARGET_COMPILER_MSC)
#include <objbase.h>  // CoCreateGuid용
#include <rpcdce.h>

#pragma comment(lib, "rpcrt4.lib")

BOOL make_guid(
	char** ppDest,
	size_t length
)
{
    UUID uuid;
    RPC_STATUS status = UuidCreate(&uuid);

    if (status == RPC_S_OK) 
    {
        // UUID를 문자열로 변환
        RPC_CSTR str;
        status = UuidToStringA(&uuid, &str);

        if (status == RPC_S_OK) 
        {
            fstrlcpy(*ppDest, (CHAR*)str, length);

            // 문자열 해제
            RpcStringFreeA(&str);

            return TRUE;
        }
    }

    return FALSE;
}
#elif defined(__TARGET_COMPILER_GCC)
#include "guid.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uuid/uuid.h>

BOOL make_guid(
    char** ppDest,
    size_t length
)
{
    // 입력 포인터 확인
    if (ppDest == NULL || *ppDest == NULL) {
        return FALSE;
    }

    // 버퍼 크기 확인 (표준 UUID 문자열은 36자 + 널 종료 문자)
    if (length < 37) {
        return FALSE;
    }

    uuid_t uuid;

    // UUID 생성
    uuid_generate(uuid);

    // UUID를 문자열로 변환 (36자 + 널 종료 문자)
    uuid_unparse(uuid, *ppDest);

    return TRUE;
}

#endif // __TARGET_COMPILER_MSC
