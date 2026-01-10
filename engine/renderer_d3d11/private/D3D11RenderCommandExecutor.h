#pragma once
#include "TQueue.h"
#include "TArray.h"
using namespace mark;


class D3D11RenderDevice;
class D3D11RenderQueue;
struct D3D11_RENDER_FRAME;

class D3D11RenderCommandExecutor
{
	static D3D11RenderCommandExecutor* s_pInstance;
	
public:
	D3D11RenderCommandExecutor(D3D11RenderDevice* pRenderDevice);
	~D3D11RenderCommandExecutor() noexcept;

	void Push(const D3D11_RENDER_FRAME* pRenderFrame) noexcept;
	void Execute() noexcept;

	static inline D3D11RenderCommandExecutor& Get() noexcept { return *s_pInstance; }

private:
	void ResetFrame(D3D11_RENDER_FRAME* pRenderFrame);

private:
	D3D11RenderDevice* m_pRenderDevice;
	TQueue<D3D11_RENDER_FRAME*, TA_POOL> m_RenderFrameQueue;

};


