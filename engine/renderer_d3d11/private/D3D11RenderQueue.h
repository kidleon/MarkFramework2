#pragma once
#include "TArray.h"
using namespace mark;


class D3D11RenderCamera;
struct BASE_RENDER_COMMAND;

class D3D11RenderQueue
{
public:
	D3D11RenderQueue();
	~D3D11RenderQueue() noexcept;

	void SetRenderCamera(D3D11RenderCamera* pCamera) noexcept;

	void Reset();
	void Add(RENDER_QUEUE_TYPE QueueType, BASE_RENDER_COMMAND* pRenderCmd) noexcept;

	__FORCEINLINE D3D11RenderCamera* INL_GetRenderCamera() const noexcept { return m_pRenderCamera; }
	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }

private:
	D3D11RenderCamera* m_pRenderCamera;
	TArray<BASE_RENDER_COMMAND*, TA_POOL> m_OpaqueCmdList;

	LINK_NODE m_LinkNode;

};