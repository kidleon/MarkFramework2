#ifndef __D3D11_RENDER_QUEUE_H__
#define __D3D11_RENDER_QUEUE_H__

#include "TArray.h"
#include "D3D11RenderCamera.h"

using namespace mark;

struct MESH_RENDER_COMMAND;
class D3D11RenderCamera;

class D3D11RenderQueue
{
	enum RENDER_QUEUE_TYPE : uint8
	{
		RQ_OPAQUE = 0,
		RQ_TRANSPARENCY,
		RQ_PARTICLE,
		RQ_UI,
		EMAX_RQ
	};

public:
	D3D11RenderQueue();
	~D3D11RenderQueue() noexcept;

	void Init(size_t InitialCapacity);
	void Shutdown();
	void Clear();

	void Add(RENDER_QUEUE_TYPE QueueType, MESH_RENDER_COMMAND* pRenderCommand);
	void Process();

	__FORCEINLINE void INL_SetRenderCamera(D3D11RenderCamera* pRenderCamera) noexcept
	{
		m_pRenderCamera = pRenderCamera;
	}

	__FORCEINLINE uint32 INL_GetRenderCameraID() const noexcept
	{
		return m_pRenderCamera ? m_pRenderCamera->GetID() : 0;
	}

	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept
	{
		return &m_LinkNode;
	}

	__FORCEINLINE size_t INL_GetNumCommand(RENDER_QUEUE_TYPE QueueType) const noexcept
	{
		if (QueueType >= EMAX_RQ)
			return 0;
		return m_CmdList[QueueType].size();
	}

	__FORCEINLINE const MESH_RENDER_COMMAND* INL_GetCommand(RENDER_QUEUE_TYPE QueueType, size_t Index) const noexcept
	{
		if (QueueType >= EMAX_RQ || Index >= m_CmdList[QueueType].size())
			return nullptr;
		return m_CmdList[QueueType][Index];
	}

private:
	D3D11RenderCamera* m_pRenderCamera = nullptr;
	TArray<MESH_RENDER_COMMAND*, TA_POOL> m_CmdList[EMAX_RQ];

	LINK_NODE m_LinkNode;

};

#endif // __D3D11_RENDER_QUEUE_H__
