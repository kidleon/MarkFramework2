#ifndef __D3D11_RENDER_SYSTEM_H__
#define __D3D11_RENDER_SYSTEM_H__


interface IRenderCamera;
class D3D11RenderDevice;
class D3D11ConstantBufferAllocator;
class D3D11BlobAllocator;
class D3D11RenderContext;
class D3D11ShaderProgramCache;
class D3D11SurfaceMaterialBlockPool;
class D3D11ResourceCommandPool;
class D3D11RenderCommandPool;
class D3D11RenderCommandExecutor;
class D3D11InputLayoutCache;
class D3D11RenderStateCache;
class D3D11BufferPool;
class D3D11DDSTextureFactory;

class D3D11RenderSystem final : public IRenderSystem
{
public:
	// IUNKNOWN interface
	long AddRef() final;
	long Release() final;
	long RefCnt() final;

	BOOL Init(
		HWND hWnd,
		uint32 ScreenWidth,
		uint32 ScreenHeight,
		BOOL Fullscreen
	);

	void Shutdown();

	virtual const RENDER_SETTINGS& GetRenderSettings() const noexcept final;
	virtual void SetRenderSettings(const RENDER_SETTINGS& Settings) noexcept final;

	virtual BOOL CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& Desc, IPrimitiveBuffer** ppOut) final;
	virtual BOOL CreateRenderCamera(const RENDERCAMERA_CREATE_DESC& Desc, IRenderCamera** ppOut) final;
	virtual BOOL CreateSurfaceMaterial(ISurfaceMaterial** ppOut) final;
	virtual BOOL GetOrCreateShaderProgram(const SHADER_PROGRAM_CREATE_DESC& Desc, IShaderProgram** ppOut) final;
	virtual BOOL GetOrCreateDDSTextureFactory(IDDSTextureFactory** ppOut) final;
	virtual BOOL GetOrCreateRenderContext(IRenderContext** ppContext) final;

	virtual void Update() final;

private:
	virtual ~D3D11RenderSystem() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	D3D11RenderDevice* m_pRenderDevice = nullptr;
	D3D11ConstantBufferAllocator* m_pCBAllocator = nullptr;
	D3D11BlobAllocator* m_pBlobAllocator = nullptr;
	D3D11RenderContext* m_pRenderContext = nullptr;
	D3D11ShaderProgramCache* m_pShaderProgramCache = nullptr;
	D3D11SurfaceMaterialBlockPool* m_pSurfaceMaterialBlockPool = nullptr;
	D3D11RenderStateCache* m_pRenderStateCache = nullptr;
	D3D11RenderCommandExecutor* m_pRenderCommandExecutor = nullptr;
	D3D11InputLayoutCache* m_pInputLayoutCache = nullptr;
	D3D11BufferPool* m_pBufferPool = nullptr;
	D3D11DDSTextureFactory* m_pDDSTextureFactory = nullptr;

};


#endif // __D3D11_RENDER_SYSTEM_H__
