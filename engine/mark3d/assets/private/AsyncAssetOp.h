#ifndef __ASYNC_ASSET_ARGUMENT_H__
#define __ASYNC_ASSET_ARGUMENT_H__


interface IFileSystem;
interface IAsset;

struct AsyncAssetOp
{
	char szRelativePath[264];
	IFileSystem* pFileSystem;
	IAsset* pAsset;
};

#endif // __ASYNC_ASSET_ARGUMENT_H__
