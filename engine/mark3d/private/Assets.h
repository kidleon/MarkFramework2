#ifndef __ASSETS_H__
#define __ASSETS_H__

#include "IAssets.h"


interface IFileSystem;
interface ITextAsset;
interface IBinaryAsset;
interface ITexture1D;
interface ITexture2D;

class Assets : public IAssets
{
	DECLARATION_IUNKNOWN_INTERFACE(Assets);

	constexpr static size_t THREAD_POOL_SIZE = 4;

public:
	Assets();

	BOOL Init(const char* szRootPath);
	void Shutdown();

	// IAssets interface
	virtual BOOL Load(const char* szRelativePath, ITextAsset** ppOut) override;
	virtual BOOL LoadAsync(const char* szRelativePath, ITextAsset** ppOut) override;
	virtual BOOL Load(const char* szRelativePath, IBinaryAsset** ppOut) override;
	virtual BOOL LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut) override;
	virtual BOOL Load(const char* szRelativePath, ITexture1D** ppOut) override;
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture1D** ppOut) override;
	virtual BOOL Load(const char* szRelativePath, ITexture2D** ppOut) override;
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture2D** ppOut) override;

private:
	IFileSystem* m_pFileSystem;
	HANDLE m_hThreadPool;
	HANDLE m_hIDGen;
	BOOL m_Initialized;

};

#endif // __ASSETS_H__