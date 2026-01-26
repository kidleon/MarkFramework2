#pragma once
#include "TArray.h"
using namespace mark;

#include "D3D11RenderCamera.h"


struct D3D11_DRAW_COMMAND;

class D3D11_RENDER_QUEUE
{
public:
	D3D11_RENDER_QUEUE();
	~D3D11_RENDER_QUEUE() noexcept;

	void Reset();
	void Add(RENDER_QUEUE_TYPE QueueType, D3D11_DRAW_COMMAND* pRenderCmd) noexcept;
	void Sort() noexcept;
	
	__FORCEINLINE size_t INL_GetNumCommands() const noexcept { return m_OpaqueCmdList.size(); }
	__FORCEINLINE D3D11_DRAW_COMMAND* INL_GetCommandAt(size_t Index) noexcept { return m_OpaqueCmdList[Index]; }
	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }

private:
	TArray<D3D11_DRAW_COMMAND*, TA_POOL> m_OpaqueCmdList;
	
	LINK_NODE m_LinkNode;

};


class D3D11_RENDER_QUEUE_GROUP
{
public:
	D3D11_RENDER_QUEUE_GROUP() noexcept;
	~D3D11_RENDER_QUEUE_GROUP() noexcept;

	__FORCEINLINE D3D11_RENDER_QUEUE_GROUP(const D3D11_RENDER_QUEUE_GROUP& RQs) noexcept
	{
		if (m_pRenderCamera)
			m_pRenderCamera->Release();

		m_pRenderCamera = RQs.m_pRenderCamera;
		m_pOpaqueRQ = RQs.m_pOpaqueRQ;
		m_pTransparentRQ = RQs.m_pTransparentRQ;

		if (m_pRenderCamera)
			m_pRenderCamera->AddRef();
	}

	__FORCEINLINE D3D11_RENDER_QUEUE_GROUP(D3D11_RENDER_QUEUE_GROUP&& RQs) noexcept
	{
		if (m_pRenderCamera)
			m_pRenderCamera->Release();

		m_pRenderCamera = RQs.m_pRenderCamera;
		m_pOpaqueRQ = RQs.m_pOpaqueRQ;
		m_pTransparentRQ = RQs.m_pTransparentRQ;

		RQs.m_pRenderCamera = nullptr;
		RQs.m_pOpaqueRQ = nullptr;
		RQs.m_pTransparentRQ = nullptr;
	}

	__FORCEINLINE D3D11_RENDER_QUEUE_GROUP& operator=(const D3D11_RENDER_QUEUE_GROUP& RQs) noexcept
	{
		if (this != &RQs)
		{
			if (m_pRenderCamera)
				m_pRenderCamera->Release();

			m_pRenderCamera = RQs.m_pRenderCamera;
			m_pOpaqueRQ = RQs.m_pOpaqueRQ;
			m_pTransparentRQ = RQs.m_pTransparentRQ;

			if (m_pRenderCamera)
				m_pRenderCamera->AddRef();
		}

		return *this;
	}

	__FORCEINLINE D3D11_RENDER_QUEUE_GROUP& operator=(D3D11_RENDER_QUEUE_GROUP&& RQs) noexcept
	{
		if (this != &RQs)
		{
			if (m_pRenderCamera)
				m_pRenderCamera->Release();
			m_pRenderCamera = RQs.m_pRenderCamera;
			m_pOpaqueRQ = RQs.m_pOpaqueRQ;
			m_pTransparentRQ = RQs.m_pTransparentRQ;
			RQs.m_pRenderCamera = nullptr;
			RQs.m_pOpaqueRQ = nullptr;
			RQs.m_pTransparentRQ = nullptr;
		}

		return *this;
	}

	void PrepareRQ(D3D11RenderCamera* pRenderCamera);
	void Reset();

	__FORCEINLINE const D3D11RenderCamera* INL_GetRenderCamera() const noexcept
	{
		return m_pRenderCamera;
	}

	__FORCEINLINE D3D11_RENDER_QUEUE* INL_GetOpaqueRQ() const noexcept
	{
		return m_pOpaqueRQ;
	}

	__FORCEINLINE const D3D11_RENDER_QUEUE* INL_GetTransparentRQ() const noexcept
	{
		return m_pTransparentRQ;
	}

private:
	D3D11RenderCamera* m_pRenderCamera;
	D3D11_RENDER_QUEUE* m_pOpaqueRQ;
	D3D11_RENDER_QUEUE* m_pTransparentRQ;

};
