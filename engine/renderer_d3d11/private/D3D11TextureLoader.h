#ifndef __D3D11_TEXTURELOADER_H__
#define __D3D11_TEXTURELOADER_H__


class D3D11Texture1D;
class D3D11Texture2D;
class D3D11RenderDevice;

static BOOL LoadD3D11Texture1DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData, 
	size_t DataSize, 
	D3D11Texture1D* pTexture1D
);

static BOOL LoadD3D11Texture1DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	int MipLevels,
	COLOR_FORMAT ColorFormat,
	D3D11Texture1D* pTexture1D
);

static BOOL LoadD3D11Texture2DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	D3D11Texture2D* pTexture2D
);

static BOOL LoadD3D11Texture2DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	int MipLevels,
	COLOR_FORMAT ColorFormat,
	D3D11Texture2D* pTexture2D
);


#endif // __D3D11_TEXTURELOADER_H__
