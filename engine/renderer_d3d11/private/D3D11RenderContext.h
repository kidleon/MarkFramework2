#pragma once

class D3D11RenderQueue;

struct RENDER_FRAME
{
	TArray<D3D11RenderQueue*, TA_POOL> OpaqueRQs;
	TArray<D3D11RenderQueue*, TA_POOL> TransparentRQs;
	/*
	TArray<D3D11RenderQueue*, TA_POOL> ShadowRQs;
	TArray<D3D11RenderQueue*, TA_POOL> SpriteRQs;
	TArray<D3D11RenderQueue*, TA_POOL> UIRQs;
	TArray<D3D11RenderQueue*, TA_POOL> OverrayRQs;
	*/
};

class D3D11RenderContext final : public IRenderContext
{
	static constexpr size_t MAX_RENDER_FRAME = 4;

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

	D3D11RenderQueue* m_pCurOpaqueRQ = nullptr;
	D3D11RenderQueue* m_pCurTransparentRQ = nullptr;
	
	RENDER_FRAME m_RenderFrames[MAX_RENDER_FRAME];

	int32 m_LastFrameIndex = -1;
	int32 m_CurrentFrameIndex = -1;

};