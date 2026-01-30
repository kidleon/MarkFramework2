#pragma once
#include "D3D11RenderFrame.h"
#include "D3D11RenderSortIndexer.h"
#include "THashMap.h"


class D3D11SurfaceMaterial;
class D3D11PrimitiveBuffer;

class D3D11RenderContext final : public IRenderContext
{
	static constexpr size_t MAX_RENDER_FRAME = 4;

public:
	D3D11RenderContext() = default;

	void Init();
	void Destroy();

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
	void DrawPrimitive(const LOCAL_TRANSFORM& Transform, int32 PrimitiveIndex) final;

private:
	~D3D11RenderContext() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	D3D11RenderCamera* m_pCurRenderCamera = nullptr;
	D3D11SurfaceMaterial* m_pCurSurfaceMaterial = nullptr;
	D3D11PrimitiveBuffer* m_pCurPrimitiveBuffer = nullptr;
	RENDER_SORT_INDEXER* m_pCurRenderSortIndexer = nullptr;

	D3D11_RENDER_QUEUE_GROUP* m_pCurRQs = nullptr;
	D3D11_RENDER_FRAME m_RenderFrames[MAX_RENDER_FRAME];

	MATRIX4 m_ViewProjMatrix = {};

	int32 m_LastFrameIndex = -1;
	int32 m_CurrentFrameIndex = -1;

	HANDLE m_StackPool = nullptr;
	RENDER_SORT_INDEXER m_RenderSortIndexer[MAX_RENDER_FRAME];
};