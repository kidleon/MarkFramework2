#include "pch.h"
#include "Log.h"
#include "IDataStream.h"
#include "IFileSystem.h"
#include "TextAssetLoader.h"
#include "TextAsset.h"
#include "AsyncAssetArgument.h"


BOOL LoadTextAssetFromFileSystem(
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	TextAsset* pTextAsset
)
{
	if(!pTextAsset)
		return FALSE;

	char* pBuffer = nullptr;

	if (!pFileSystem->ExistFile(szRelativePath))
	{
		SYS_LOG_E("Assets::Load - File not found: %s", szRelativePath);
		goto LB_FAILED;
	}

	IDataStream* pDataStream = pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Assets::Load - Failed to open file: %s", szRelativePath);
		goto LB_FAILED;
	}
		
	size_t streamSize = pDataStream->GetSize();
	pBuffer = (char*)MARK_SYS_ALLOC(streamSize + 1);
	pDataStream->Read(pBuffer, streamSize);
	pBuffer[streamSize] = '\0'; // Null-terminate

	if (streamSize > 2 && !is_valid_utf8((unsigned char*)pBuffer, streamSize))
	{
		SYS_LOG_E("Assets::Load - Invalid UTF-8 encoding: %s", szRelativePath);
		pDataStream->Release();

		// 추후 이곳에서 다른 인코딩 변환 로직을 추가할 수 있음.
		goto LB_FAILED;
	}

	pTextAsset->INL_SetData(pBuffer, streamSize);
	pDataStream->Release();

	return TRUE;

LB_FAILED:
	if (pBuffer)
	{
		MARK_SYS_FREE(pBuffer);
		pBuffer = nullptr;
	}

	pTextAsset->INL_SetData(NULL, 0);

	return FALSE;
}

void AsyncLoadTextAssetFromFileSystem(void* pArg)
{
	if (!pArg) return;

	AsyncArgument* pAsyncArg = (AsyncArgument*)pArg;
	if (!pAsyncArg->pAsset) return;

	TextAsset* pTextAsset = static_cast<TextAsset*>(pAsyncArg->pAsset);

	BOOL result = LoadTextAssetFromFileSystem(
		pAsyncArg->pFileSystem,
		pAsyncArg->szRelativePath,
		pTextAsset
	);

	pTextAsset->Release(); // 비동기 작업에서 증가시킨 참조 카운트 해제

	MARK_POOL_FREE(pAsyncArg);
}
