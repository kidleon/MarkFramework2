#ifndef __BINARYASSET_LOADER_H__
#define __BINARYASSET_LOADER_H__


class BinaryAsset;
interface IFileSystem;

extern BOOL LoadBinaryAssetFromFileSystem(
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	BinaryAsset* pBinaryAsset
);

extern void AsyncLoadBinaryAssetFromFileSystem(void* pArg);

#endif // __BINARYASSET_LOADER_H__
