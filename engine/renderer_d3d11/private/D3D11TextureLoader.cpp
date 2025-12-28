#include "pch.h"
#include "D3D11TextureLoader.h"
#include "D3D11Texture1D.h"
#include "D3D11Texture2D.h"
#include "D3D11RenderDevice.h"
#include "stb_image.h"
#include "Log.h"


BOOL LoadD3D11Texture1DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	D3D11Texture1D* pTexture1D
)
{
	return LoadD3D11Texture1DFromMemory(
		pRenderDevice,
		szTextureName,
		pData,
		DataSize,
		1,
		COLOR_FORMAT::R8G8B8A8_UNORM,
		pTexture1D
	);
}


BOOL LoadD3D11Texture1DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	int MipLevels,
	COLOR_FORMAT ColorFormat,
	D3D11Texture1D* pTexture1D
)
{
	int Width = 0;
	int Height = 0;
	int Channels = 0;

	stbi_uc* pImage = stbi_load_from_memory(
		(stbi_uc*)pData,
		(int)DataSize,
		&Width,
		&Height,
		&Channels,
		0
	);

	if (!pImage)
	{
		// Error: Failed to load image from memory.
		SYS_LOG_E("LoadD3D11Texture1DFromMemory: Failed to load image from memory.");
		return FALSE;
	}

	if (Height > 1)
	{
		// Error: Not a 1D texture.
		SYS_LOG_E("LoadD3D11Texture1DFromMemory: The image height is greater than 1, not a valid 1D texture.");

		if (pImage)
		{
			stbi_image_free(pImage);
			pImage = nullptr;
		}

		return FALSE;
	}

	D3D11_TEXTURE1D_CREATE_DESC Desc = {};
	Desc.Name = szTextureName;
	Desc.Width = (UINT32)Width;
	Desc.MipLevels = MipLevels;
	Desc.pInitialData = pImage;
	Desc.InitialSize = Width * Channels;
	Desc.Format = ColorFormat;
	Desc.Usage = RESOURCE_USAGE::STATIC;

	if (!pRenderDevice->FillTexture1D(
		&Desc,
		pTexture1D
	))
	{
		SYS_LOG_E("LoadD3D11Texture1DFromMemory: Failed to create D3D11 texture1D.");

		if (pImage)
		{
			stbi_image_free(pImage);
			pImage = nullptr;
		}

		return FALSE;
	}

	if (pImage)
	{
		stbi_image_free(pImage);
		pImage = nullptr;
	}

	return TRUE;
}

BOOL LoadD3D11Texture2DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	D3D11Texture2D* pTexture2D
)
{
	return LoadD3D11Texture2DFromMemory(
		pRenderDevice,
		szTextureName,
		pData,
		DataSize,
		1,
		COLOR_FORMAT::R8G8B8A8_UNORM,
		pTexture2D
	);
}

BOOL LoadD3D11Texture2DFromMemory(
	D3D11RenderDevice* pRenderDevice,
	const char* szTextureName,
	const void* pData,
	size_t DataSize,
	int MipLevels,
	COLOR_FORMAT ColorFormat,
	D3D11Texture2D* pTexture2D
)
{
	int Width = 0;
	int Height = 0;
	int Channels = 0;

	stbi_uc* pImage = stbi_load_from_memory(
		(stbi_uc*)pData,
		(int)DataSize,
		&Width,
		&Height,
		&Channels,
		0
	);

	if (!pImage)
	{
		// Error: Failed to load image from memory.
		SYS_LOG_E("LoadD3D11Texture2DFromMemory: Failed to load image from memory.");
		return FALSE;
	}

	D3D11_TEXTURE2D_CREATE_DESC Desc = {};
	Desc.Name = szTextureName;
	Desc.Width = (UINT32)Width;
	Desc.Height = (UINT32)Height;
	Desc.MipLevels = MipLevels;
	Desc.pInitialData = pImage;
	Desc.InitialSize = Width * Channels;
	Desc.Format = ColorFormat;
	Desc.Usage = RESOURCE_USAGE::STATIC;

	if (!pRenderDevice->FillTexture2D(
		&Desc,
		pTexture2D
	))
	{
		SYS_LOG_E("LoadD3D11Texture1DFromMemory: Failed to create D3D11 texture2D.");

		if (pImage)
		{
			stbi_image_free(pImage);
			pImage = nullptr;
		}

		return FALSE;
	}

	if (pImage)
	{
		stbi_image_free(pImage);
		pImage = nullptr;
	}

	return TRUE;
}

