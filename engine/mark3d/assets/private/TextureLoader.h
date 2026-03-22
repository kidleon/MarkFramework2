#ifndef __TEXTURE_LOADER_H__
#define __TEXTURE_LOADER_H__


class BinaryAsset;
interface IFileSystem;
interface ITexture2D;

extern BOOL LoadTexture1DFromFileSystem(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	IRenderSystem* pRenderSystem,
	const char* szRelativePath,
	BOOL sRGB,
	ITexture1D* pTexture
);

extern BOOL LoadTexture2DFromFileSystem(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	IRenderSystem* pRenderSystem,
	const char* szRelativePath,
	BOOL sRGB,
	ITexture2D* pTexture
);

extern void AsyncLoadTexture1DFromFileSystem(HANDLE temppool_handle, void* pArg);

extern void AsyncLoadTexture2DFromFileSystem(HANDLE temppool_handle, void* pArg);

#endif // __TEXTURE_LOADER_H__
