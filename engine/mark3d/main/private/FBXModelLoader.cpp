#include "pch.h"
#include "FBXModelLoader.h"
#include "ufbx.h"


static inline ufbx_scene* load_fbx_from_memory(const void* data, size_t data_size)
{
	ufbx_load_opts opts = { 0 };

	// 필요에 따라 옵션 설정
	opts.target_axes = ufbx_axes_left_handed_y_up;	// 좌표계 변환 (선택)
	opts.target_unit_meters = 1.0f;                 // 단위 변환 (선택)

	ufbx_error error;
	ufbx_scene* scene = ufbx_load_memory(data, data_size, &opts, &error);

	return scene;
}

BOOL LoadModelFromFBX(
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	IModelAsset* pModelAsset
)
{
	IDataStream* pDataStream = pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Failed to open FBX file: %s", szRelativePath);
		return FALSE;
	}

	size_t streamSize = pDataStream->GetSize();
	char* pBuffer = (char*)CORE_SYS_ALLOC(streamSize);
	pDataStream->Read(pBuffer, streamSize);

	ufbx_scene* scene = load_fbx_from_memory(pBuffer, streamSize);
	if (!scene)
	{
		pModelAsset->INL_SetLoadStat(LOAD_STAT::FAILED);
		return FALSE;
	}
				
	CHECK_RELEASE(pDatStream);
	

	return TRUE;
}


void AsyncLoadModelFromFBX(void* pArg)
{

}