#include "pch.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderCamera.h"
#include "D3D11RenderQueuePool.h"
#include "D3D11RenderCommand.h"
#include "D3D11RenderCommandPool.h"
#include "TAlgorithm.h"


//-----------------------------------------------------------------------------
// class D3D11_RENDER_QUEUE
D3D11_RENDER_QUEUE::D3D11_RENDER_QUEUE()
	: m_LinkNode{}
{
	m_LinkNode.data = this;
}

D3D11_RENDER_QUEUE::~D3D11_RENDER_QUEUE() noexcept
{
	Reset();
}

void D3D11_RENDER_QUEUE::Reset()
{
	for (D3D11_DRAW_COMMAND* pCmd : m_OpaqueCmdList)
	{
		if (pCmd)
		{
			D3D11RenderCommandPool::Get()->Release(pCmd);
		}
	}

	m_OpaqueCmdList.clear();
}

void D3D11_RENDER_QUEUE::Add(RENDER_QUEUE_TYPE QueueType, D3D11_DRAW_COMMAND* pDrawCmd) noexcept
{
	switch (QueueType)
	{
		case RENDER_QUEUE_TYPE::RQ_OPAQUE:
		{
			m_OpaqueCmdList.push_back(pDrawCmd);
		}
		break;

		default:
			break;
	}
}

void D3D11_RENDER_QUEUE::Sort() noexcept
{
	if (m_OpaqueCmdList.empty())
		return;

	mark::sort(
		&m_OpaqueCmdList[0], 
		m_OpaqueCmdList.size(),
		[](D3D11_DRAW_COMMAND* pA, D3D11_DRAW_COMMAND* pB) noexcept
		{
			return pA->SortKey.Value < pB->SortKey.Value;
		}
	);
}

//-----------------------------------------------------------------------------
// class D3D11_RENDER_QUEUE_GROUP
D3D11_RENDER_QUEUE_GROUP::D3D11_RENDER_QUEUE_GROUP() noexcept
	: m_pRenderCamera(nullptr)
	, m_pOpaqueRQ(nullptr)
	, m_pTransparentRQ(nullptr)
{

}

D3D11_RENDER_QUEUE_GROUP::~D3D11_RENDER_QUEUE_GROUP() noexcept
{
	Reset();
}

void D3D11_RENDER_QUEUE_GROUP::PrepareRQ(D3D11RenderCamera* pRenderCamera)
{
	if (!m_pRenderCamera)
	{
		m_pRenderCamera = pRenderCamera;
		m_pRenderCamera->AddRef();
	}
	
	if (!m_pOpaqueRQ)
	{
		m_pOpaqueRQ = D3D11RenderQueuePool::Get().GetRQ();
	}
	
	if (!m_pTransparentRQ)
	{
		m_pTransparentRQ = D3D11RenderQueuePool::Get().GetRQ();
	}
}

void D3D11_RENDER_QUEUE_GROUP::Reset()
{
	if (m_pRenderCamera)
	{
		m_pRenderCamera->Release();
		m_pRenderCamera = nullptr;
	}

	if (m_pOpaqueRQ)
	{
		D3D11RenderQueuePool::Get().ReleaseRQ(m_pOpaqueRQ);
		m_pOpaqueRQ = nullptr;
	}

	if (m_pTransparentRQ)
	{
		D3D11RenderQueuePool::Get().ReleaseRQ(m_pTransparentRQ);
		m_pTransparentRQ = nullptr;
	}

}