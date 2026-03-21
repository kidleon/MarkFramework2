#ifndef __TEXTURE_CACHE_H__
#define __TEXTURE_CACHE_H__


struct ITexture1D;
struct ITexture2D;

VOID InitTextureCache(
	size_t Texture1DBucketSize,
	size_t Texture2DBucketSize
);

VOID ShutdownTextureCache();

VOID ClearTextureCache();

BOOL QueryTexture1D(
	const char* szTextureName,
	ITexture1D** ppOut
);

BOOL QueryTexture2D(
	const char* szTextureName,
	ITexture2D** ppOut
);

BOOL RegiTexture1D(
	const char* szTextureName,
	ITexture1D* pTexture
);

BOOL RegiTexture2D(
	const char* szTextureName,
	ITexture2D* pTexture
);



#endif // __TEXTURE_CACHE_H__
