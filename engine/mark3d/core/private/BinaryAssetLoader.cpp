#include "pch.h"
#include "BinaryAssetLoader.h"
#include "Log.h"
#include "BinaryAsset.h"
#include "AsyncAssetOp.h"


BOOL LoadBinaryAssetFromFileSystem(
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	BinaryAsset* pBinaryAsset
)
{
	if (!pBinaryAsset)
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
	pBuffer = (char*)CORE_SYS_ALLOC(streamSize + 1);
	pDataStream->Read(pBuffer, streamSize);

	pBinaryAsset->INL_SetData(pBuffer, streamSize);
	pDataStream->Release();

	return TRUE;

LB_FAILED:
	if (pBuffer)
	{
		CORE_SYS_FREE(pBuffer);
		pBuffer = nullptr;
	}

	pBinaryAsset->INL_SetData(NULL, 0);

	return FALSE;
}

void AsyncLoadBinaryAssetFromFileSystem(void* pArg)
{
	if (!pArg) return;

	AsyncAssetOp* pAsyncOp = (AsyncAssetOp*)pArg;
	if (!pAsyncOp->pAsset) return;

	BinaryAsset* pBinaryAsset = static_cast<BinaryAsset*>(pAsyncOp->pAsset);

	BOOL result = LoadBinaryAssetFromFileSystem(
		pAsyncOp->pFileSystem,
		pAsyncOp->szRelativePath,
		pBinaryAsset
	);

	pBinaryAsset->Release(); // 비동기 작업에서 증가시킨 참조 카운트 해제

	CORE_POOL_FREE(pAsyncOp);
}
