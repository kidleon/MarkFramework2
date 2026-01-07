#include "pch.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderCamera.h"


D3D11RenderQueue::D3D11RenderQueue()
	: m_pRenderCamera(nullptr)
	, m_LinkNode{}
{
	m_LinkNode.data = this;
}

D3D11RenderQueue::~D3D11RenderQueue() noexcept
{
	Reset();
}

void D3D11RenderQueue::SetRenderCamera(D3D11RenderCamera* pCamera) noexcept
{
	m_pRenderCamera = pCamera;
	m_pRenderCamera->AddRef();
}

void D3D11RenderQueue::Reset()
{
	if (m_pRenderCamera)
	{
		m_pRenderCamera->Release();
		m_pRenderCamera = nullptr;
	}
	m_OpaqueCmdList.clear();
}

void D3D11RenderQueue::Add(RENDER_QUEUE_TYPE QueueType, BASE_RENDER_COMMAND* pRenderCmd) noexcept
{
	switch (QueueType)
	{
		case RENDER_QUEUE_TYPE::RQ_OPAQUE:
		{
			m_OpaqueCmdList.push_back(pRenderCmd);
		}
		break;

		default:
			break;
	}
}