#ifndef __FBX_MODEL_LOADER_H__
#define __FBX_MODEL_LOADER_H__

class ModelAsset;
interface IFileSystem;

extern BOOL LoadModelFromFBX(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	ModelAsset* pModelAsset
);

extern void AsyncLoadModelFromFBX(void* pArg);

#endif // __FBX_MODEL_LOADER_H__