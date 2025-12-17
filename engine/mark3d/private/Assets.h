#ifndef __ASSETS_H__
#define __ASSETS_H__

#include "IAssets.h"


interface IFileSystem;
interface ITextAsset;
interface IBinaryAsset;
interface ITexture1D;
interface ITexture2D;
interface IMesh;

class Assets : public IAssets
{
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

	virtual BOOL CreateMesh(
		UINT32 m_VertexFormat, 
		size_t MaxVertexCount, 
		size_t MaxIndexCount, 
		IMesh** ppOut
	) override;

	virtual BOOL CreateMesh(
		const NameHash& Name, 
		UINT32 m_VertexFormat,
		size_t MaxVertexCount, 
		size_t MaxIndexCount,
		IMesh** ppOut
	) override;

	virtual BOOL CreateSurfaceMaterial(ISurfaceMaterial** ppOut) override;


protected:
	virtual ~Assets() noexcept;
	virtual void OnDestroy() override;

private:
	IFileSystem* m_pFileSystem;
	HANDLE m_hThreadPool;
	HANDLE m_hIDGen;
	BOOL m_Initialized;

};

#endif // __ASSETS_H__