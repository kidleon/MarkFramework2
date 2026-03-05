#ifndef __DDS_TEXTURE_LOADER_H__
#define __DDS_TEXTURE_LOADER_H__


class BinaryAsset;
interface IFileSystem;

extern BOOL LoadTextureFromDDS(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	ITexture2D* pTexture
);

extern void AsyncLoadTextureFromDDS(HANDLE temppool_handle, void* pArg);

#endif // __DDS_TEXTURE_LOADER_H__
