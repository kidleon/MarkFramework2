#pragma once
#include "D3D11RenderFrame.h"


class D3D11RenderContext final : public IRenderContext
{
	static constexpr size_t MAX_RENDER_FRAME = 4;

public:
	D3D11RenderContext() = default;

	// IUNKNOWN interface
	long AddRef() final;
	long Release() final;
	long RefCnt() final;

	// IRenderContext interface
	void BeginFrame() noexcept final;
	void EndFrame() noexcept final;

	void BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept final;
	void EndRenderCamera() noexcept final;

	void SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial) final;
	void SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer) final;
	void DrawPrimitive(int32 PrimitiveIndex) final;

private:
	~D3D11RenderContext() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	D3D11_RENDER_QUEUE_GROUP* m_pCurRQs = nullptr;
	D3D11_RENDER_FRAME m_RenderFrames[MAX_RENDER_FRAME];

	int32 m_LastFrameIndex = -1;
	int32 m_CurrentFrameIndex = -1;

};