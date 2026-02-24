#include "pch.h"
#include "FBXModelLoader.h"
#include "ModelAsset.h"
#include "temp_pool.h"
#include "fbx_loader.h"
#include "TArray.h"



BOOL LoadModelFromFBX(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	ModelAsset* pModelAsset
)
{
	IDataStream* pDataStream = pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Failed to open FBX file: %s", szRelativePath);
		return FALSE;
	}

	size_t streamSize = pDataStream->GetSize();
	//char* pBuffer = (char*)CORE_SYS_ALLOC(streamSize);
	char* pBuffer = (char*)temppool_alloc(hTempPool, streamSize);
	if (!pBuffer)
	{
		SYS_LOG_E("Failed to allocate memory for FBX file: %s", szRelativePath);
		pDataStream->Release();
		return FALSE;
	}

	pDataStream->Read(pBuffer, streamSize);

	FBX_SCENE* fbx_scene = fbx_load(hTempPool, pBuffer, streamSize);
	if (!fbx_scene)
	{
		SYS_LOG_E("Failed to load FBX scene from file: %s", szRelativePath);
		pDataStream->Release();
		return FALSE;
	}

	pModelAsset->LoadFromFBX(fbx_scene);

	//fbx_unload(fbx_scene);
	

	return TRUE;
}


void AsyncLoadModelFromFBX(void* pArg)
{

}
