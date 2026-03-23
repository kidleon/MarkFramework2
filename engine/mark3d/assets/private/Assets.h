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

	BOOL Init(
		IRenderSystem* pRenderSystem,
		const char* szRootPath
	);
	void Shutdown();

	// IAssets interface
	virtual BOOL Load(const char* szRelativePath, ITextAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITextAsset** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, IBinaryAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut) final;
	
	virtual BOOL Load(const char* szRelativePath, IModelAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IModelAsset** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, BOOL sRGB, ITexture1D** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, BOOL sRGB, ITexture1D** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, BOOL sRGB, ITexture2D** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, BOOL sRGB, ITexture2D** ppOut) final;

	virtual BOOL Load(const char* szRelativePath, IModel** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IModel** ppOut) final;

	BOOL Load(HANDLE hTempPool, const char* szRelativePath, BOOL sRGB, ITexture1D** ppOut);
	BOOL Load(HANDLE hTempPool, const char* szRelativePath, BOOL sRGB, ITexture2D** ppOut);

private:
	virtual ~Assets() noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	BOOL IsExistTextureFile(const char* szRelativePath, char* szModifiedPath) noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	IRenderSystem* m_pRenderSystem;
	IFileSystem* m_pFileSystem;
	HANDLE m_hThreadPool;
	HANDLE m_hIDGen;
	HANDLE m_hSyncLoadTempPool; // 동기 로드 시 임시로 사용할 메모리 풀 핸들
	BOOL m_Initialized;

	char m_szTexturePath[8][MAX_FILE_LENGTH] = { 0 }; // 텍스처 경로 저장용 배열 (예: Diffuse, Normal, Specular, etc.)
};

#endif // __ASSETS_H__