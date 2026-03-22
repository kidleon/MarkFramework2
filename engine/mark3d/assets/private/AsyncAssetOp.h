#ifndef __ASYNC_ASSET_ARGUMENT_H__
#define __ASYNC_ASSET_ARGUMENT_H__


interface IFileSystem;
interface IRenderSystem;
interface IAsset;

struct AsyncAssetOp
{
	char szRelativePath[264];
	IFileSystem* pFileSystem;
	IAsset* pAsset;
	IRenderSystem* pRenderSystem;

	uintptr_t Argument1;
	uintptr_t Argument2;
};

#endif // __ASYNC_ASSET_ARGUMENT_H__
