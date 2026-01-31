#pragma once
#include "TQueue.h"
#include "TArray.h"
using namespace mark;


class D3D11RenderDevice;
class D3D11RenderQueue;
class D3D11ShaderProgram;
class D3D11PrimitiveBuffer;
struct D3D11_RENDER_FRAME;

class D3D11RenderCommandExecutor
{
	static D3D11RenderCommandExecutor* s_pInstance;

	struct RenderStats
	{
		size_t NumDrawCalls;
		size_t NumTriangles;
	};

	struct PipelineStateState
	{
		D3D11ShaderProgram* pSetVS;
		D3D11ShaderProgram* pSetPS;
		D3D11PrimitiveBuffer* pSetPB;
		ID3D11RasterizerState* pSetRS;
		ID3D11BlendState* pSetBS;
		ID3D11DepthStencilState* pSetDSS;
	};
	
public:
	D3D11RenderCommandExecutor(D3D11RenderDevice* pRenderDevice);
	~D3D11RenderCommandExecutor() noexcept;

	void Push(const D3D11_RENDER_FRAME* pRenderFrame) noexcept;
	void Execute() noexcept;

	static inline D3D11RenderCommandExecutor& Get() noexcept { return *s_pInstance; }

private:
	void ResetFrame(D3D11_RENDER_FRAME* pRenderFrame);
	void ExcuteResourceCommands(
		ID3D11DeviceContext* pDeviceContext,
		D3D11_RENDER_FRAME* pRenderFrame
	) noexcept;

private:
	D3D11RenderDevice* m_pRenderDevice;
	TQueue<D3D11_RENDER_FRAME*, TA_POOL> m_RenderFrameQueue;

	RenderStats m_RenderStats;
	PipelineStateState m_PipelineStateState;
};


