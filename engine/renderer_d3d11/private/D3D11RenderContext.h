#ifndef __D3D11_RENDER_CONTEXT_H__
#define __D3D11_RENDER_CONTEXT_H__


class D3D11RenderContext final : public IRenderContext
{
public:
	D3D11RenderContext();
	~D3D11RenderContext() noexcept;

	// IUNKNOWN interface
	long AddRef() final;
	long Release() final;
	long RefCnt() final;

	// IRenderContext interface
	void BeginRender(IRenderCamera* pRenderCamera) final;
	void EndRender() final;
	void SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial) final;
	void SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer) final;
	void DrawPrimitive(int32 PrimitiveIndex) final;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

};


#endif // __D3D11_RENDER_CONTEXT_H__
