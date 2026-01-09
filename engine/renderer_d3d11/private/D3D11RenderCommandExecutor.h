#pragma once
#include "TQueue.h"
#include "TArray.h"
using namespace mark;


class D3D11RenderDevice;
class D3D11RenderQueue;
struct RENDER_FRAME;

class D3D11RenderCommandExecutor
{
	static constexpr size_t MAX_RQ_GROUPS = 4;
	static D3D11RenderCommandExecutor* s_pInstance;
	
	struct RenderQueueGroup
	{
		TArray<D3D11RenderQueue*, TA_POOL> lstRenderQueue;
	};

public:
	D3D11RenderCommandExecutor(D3D11RenderDevice* pRenderDevice);
	~D3D11RenderCommandExecutor() noexcept;

	void Push(const RENDER_FRAME* pRenderFrame) noexcept;
	void Execute() noexcept;

	static inline D3D11RenderCommandExecutor& Get() noexcept { return *s_pInstance; }

private:
	void ResetFrame(size_t Frame);

private:
	D3D11RenderDevice* m_pRenderDevice;
	RenderQueueGroup m_RQGroups[MAX_RQ_GROUPS];

	TQueue<RENDER_FRAME*, TA_POOL> m_RenderFrameQueue;

	size_t m_CurrentFrameIndex;

};


