#ifndef __ASSETS_H__
#define __ASSETS_H__


interface IFileSystem;
interface ITextAsset;
interface IBinaryAsset;
interface IModelAsset;
interface ITexture1D;
interface ITexture2D;
interface IModel;

class Assets final : public IAssets
{
	friend class Mark3DImpl;

	static Assets* s_pInstance;

public:
	static HANDLE ID_GEN_HANDLE;

	static Assets* Get() noexcept;

public:
	Assets();

	BOOL Init(const char* szRootPath);
	void Shutdown();

	// IAssets interface
	virtual BOOL Load(const char* szRelativePath, ITextAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITextAsset** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, IBinaryAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut) final;
	
	virtual BOOL Load(const char* szRelativePath, IModelAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IModelAsset** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, ITexture1D** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture1D** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, ITexture2D** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture2D** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, IModel** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IModel** ppOut) final;

private:
	virtual ~Assets() noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	IFileSystem* m_pFileSystem;
	HANDLE m_hThreadPool;
	HANDLE m_hIDGen;
	HANDLE m_hSyncLoadTempPool; // 동기 로드 시 임시로 사용할 메모리 풀 핸들
	BOOL m_Initialized;

};

#endif // __ASSETS_H__