#ifndef __MODEL_LOADER_H__
#define __MODEL_LOADER_H__


class ModelAsset;
class Model;

extern BOOL LoadModelFromFile(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	ModelAsset* pModelAsset,
	Model* pModel
);

#endif // __MODEL_LOADER_H__
