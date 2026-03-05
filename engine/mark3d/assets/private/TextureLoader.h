#ifndef __TEXTURE_LOADER_H__
#define __TEXTURE_LOADER_H__


class BinaryAsset;
interface IFileSystem;
interface ITexture2D;

extern BOOL LoadTextureFromDDS(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	ITexture2D* pTexture
);

extern void AsyncLoadTextureFromDDS(HANDLE temppool_handle, void* pArg);

#endif // __TEXTURE_LOADER_H__
