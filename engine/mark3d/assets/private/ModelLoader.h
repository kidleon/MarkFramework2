#ifndef __MODEL_LOADER_H__
#define __MODEL_LOADER_H__


class ModelAsset;
class D3D11Model;
interface IAssets;
interface IFileSystem;
interface IRenderSystem;
interface ITexture2D;

extern BOOL LoadModelFromModelAsset(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	IAssets* pAssets,
	IRenderSystem* pRenderSystem,
	const char* szRelativePath,
	IModelAsset* pModelAsset,
	IModel* pModel
);

extern BOOL AsyncLoadModelFromModelAsset(HANDLE temppool_handle, void* pArg);

#endif // __MODEL_LOADER_H__
