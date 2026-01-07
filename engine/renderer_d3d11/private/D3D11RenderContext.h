#pragma once

class D3D11RenderQueue;

class D3D11RenderContext final : public IRenderContext
{
public:
	// IUNKNOWN interface
	long AddRef() final;
	long Release() final;
	long RefCnt() final;

	// IRenderContext interface
	void BeginFrame() noexcept final;
	void EndFrame() noexcept final;

	void BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept final;
	void EndRenderCamera() noexcept final;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	D3D11RenderQueue* m_pCurRQ = nullptr;

};