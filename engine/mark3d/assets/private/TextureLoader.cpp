#include "pch.h"
#include "TextureLoader.h"
#include "temp_pool.h"
#include "AsyncAssetOp.h"


static TEXTURE_FILE_FORMAT GetTextureFileFormat(const char* szRelativePath)
{
	if (!szRelativePath)
		return TEXTURE_FILE_FORMAT::UNKNOWN;

	char szExtension[16] = { 0 };
	get_file_extension(szRelativePath, szExtension, sizeof(szExtension));

	if (fstrstr(szExtension, "dds"))
		return TEXTURE_FILE_FORMAT::DDS;

	else if (fstrstr(szExtension, "png"))
		return TEXTURE_FILE_FORMAT::PNG;

	else if (fstrstr(szExtension, "jpg") || fstrstr(szExtension, "jpeg"))
		return TEXTURE_FILE_FORMAT::JPEG;

	else if (fstrstr(szExtension, "bmp"))
		return TEXTURE_FILE_FORMAT::BMP;

	else if (fstrstr(szExtension, "tga"))
		return TEXTURE_FILE_FORMAT::TGA;

	return TEXTURE_FILE_FORMAT::UNKNOWN;
}


BOOL LoadTexture1DFromFileSystem(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	IRenderSystem* pRenderSystem,
	const char* szRelativePath,
	BOOL sRGB,
	ITexture1D* pTexture
)
{
	IDataStream* pDataStream = pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Assets::Load: Failed to open texture1D file: %s", szRelativePath);
		return FALSE;
	}

	size_t DataSize = pDataStream->GetSize();

	temppool_clear(hTempPool);

	void* pData = temppool_alloc(hTempPool, DataSize);
	if (!pData)
	{
		pDataStream->Release();
		SYS_LOG_E("Assets::Load: Failed to allocate memory for texture1D data: %s", szRelativePath);
		return FALSE;
	}

	if (!pDataStream->Read(pData, DataSize))
	{
		SYS_LOG_E("Assets::Load: Failed to read texture1D data from file: %s", szRelativePath);
		return FALSE;
	}

	CHECK_RELEASE(pDataStream);

	TEXTURE1D_CREATE_DESC Desc = {};
	Desc.hTempHeap = hTempPool;
	Desc.FileFormat = GetTextureFileFormat(szRelativePath);
	Desc.pData = (char*)pData;
	Desc.DataSize = DataSize;
	Desc.sRGB = sRGB;

	temppool_clear(hTempPool);

	if (!pRenderSystem->CreateTexture1D(Desc, pTexture))
	{
		SYS_LOG_E("Assets::Load: Failed to create Texture1D from data: %s", szRelativePath);
		return FALSE;
	}

	return TRUE;
}

BOOL LoadTexture2DFromFileSystem(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	IRenderSystem* pRenderSystem,
	const char* szRelativePath,
	BOOL sRGB,
	ITexture2D* pTexture
)
{
	IDataStream* pDataStream = pFileSystem->OpenFile(szRelativePath, TRUE);
	if (!pDataStream)
	{
		SYS_LOG_E("Assets::Load: Failed to open texture1D file: %s", szRelativePath);
		return FALSE;
	}

	size_t DataSize = pDataStream->GetSize();

	temppool_clear(hTempPool);

	void* pData = temppool_alloc(hTempPool, DataSize);
	if (!pData)
	{
		pDataStream->Release();
		SYS_LOG_E("Assets::Load: Failed to allocate memory for texture1D data: %s", szRelativePath);
		return FALSE;
	}

	if (!pDataStream->Read(pData, DataSize))
	{
		SYS_LOG_E("Assets::Load: Failed to read texture1D data from file: %s", szRelativePath);
		return FALSE;
	}

	CHECK_RELEASE(pDataStream);

	TEXTURE2D_CREATE_DESC Desc = {};
	Desc.hTempHeap = hTempPool;
	Desc.FileFormat = GetTextureFileFormat(szRelativePath);
	Desc.pData = (char*)pData;
	Desc.DataSize = DataSize;
	Desc.sRGB = sRGB;

	temppool_clear(hTempPool);

	if (!pRenderSystem->CreateTexture2D(Desc, pTexture))
	{
		SYS_LOG_E("Assets::Load: Failed to create Texture1D from data: %s", szRelativePath);
		return FALSE;
	}

	return TRUE;
}

void AsyncLoadTexture1DFromFileSystem(HANDLE temppool_handle, void* pArg)
{
	if (!pArg) return;

	AsyncAssetOp* pAsyncOp = (AsyncAssetOp*)pArg;
	if (!pAsyncOp->pAsset) return;

	BOOL result = LoadTexture1DFromFileSystem(
		temppool_handle,
		pAsyncOp->pFileSystem,
		pAsyncOp->pRenderSystem,
		pAsyncOp->szRelativePath,
		(BOOL)pAsyncOp->Argument1, // sRGB 여부
		static_cast<ITexture1D*>(pAsyncOp->pAsset)
	);

	if (!result)
	{
		SYS_LOG_E("AsyncLoadTexture1DFromFileSystem: Failed to load texture1D from file system: %s", pAsyncOp->szRelativePath);
	}

	CORE_POOL_FREE(pAsyncOp);
}

void AsyncLoadTexture2DFromFileSystem(HANDLE temppool_handle, void* pArg)
{
	if (!pArg) return;

	AsyncAssetOp* pAsyncOp = (AsyncAssetOp*)pArg;
	if (!pAsyncOp->pAsset) return;

	BOOL result = LoadTexture2DFromFileSystem(
		temppool_handle,
		pAsyncOp->pFileSystem,
		pAsyncOp->pRenderSystem,
		pAsyncOp->szRelativePath,
		(BOOL)pAsyncOp->Argument1, // sRGB 여부
		static_cast<ITexture2D*>(pAsyncOp->pAsset)
	);

	if (!result)
	{
		SYS_LOG_E("AsyncLoadTexture2DFromFileSystem: Failed to load texture2D from file system: %s", pAsyncOp->szRelativePath);
	}

	CORE_POOL_FREE(pAsyncOp);
}
