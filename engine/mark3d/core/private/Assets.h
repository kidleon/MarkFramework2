#ifndef __ASSETS_H__
#define __ASSETS_H__


interface IFileSystem;
interface ITextAsset;
interface IBinaryAsset;
interface ITexture1D;
interface ITexture2D;
interface IMesh;

class Assets final : public IAssets
{
public:
	static HANDLE ID_GEN_HANDLE;

public:
	Assets();

	BOOL Init(const char* szRootPath);
	void Shutdown();

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAssets interface
	virtual BOOL Load(const char* szRelativePath, ITextAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITextAsset** ppOut) final;
	virtual BOOL Load(const char* szRelativePath, IBinaryAsset** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, IBinaryAsset** ppOut) final;
	virtual BOOL Load(const char* szRelativePath, ITexture1D** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture1D** ppOut) final;
	virtual BOOL Load(const char* szRelativePath, ITexture2D** ppOut) final;
	virtual BOOL LoadAsync(const char* szRelativePath, ITexture2D** ppOut) final;

	virtual BOOL CreateMesh(
		UINT32 m_BufferFormat, 
		size_t MaxVertexCount, 
		size_t MaxIndexCount, 
		IMesh** ppOut
	) final;

	virtual BOOL CreateMesh(
		const NameHash& Name, 
		UINT32 m_BufferFormat,
		size_t MaxVertexCount, 
		size_t MaxIndexCount,
		IMesh** ppOut
	) final;

	virtual BOOL CreateSurfaceMaterial(ISurfaceMaterial** ppOut) final;


protected:
	virtual ~Assets() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	IFileSystem* m_pFileSystem;
	HANDLE m_hThreadPool;
	HANDLE m_hIDGen;
	BOOL m_Initialized;

};

#endif // __ASSETS_H__