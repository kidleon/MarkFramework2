#include "pch.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderCommand.h"
#include "D3D11RenderCommandPool.h"
#include "D3D11RenderState.h"


D3D11RenderQueue::D3D11RenderQueue()
{
	m_LinkNode.data = this;
}

D3D11RenderQueue::~D3D11RenderQueue() noexcept
{
	Shutdown();
}

void D3D11RenderQueue::Init(size_t InitialCapacity)
{
	for (int32 q = 0; q < EMAX_RQ; ++q)
	{
		m_CmdList[q].reserve(InitialCapacity);
	}
}

void D3D11RenderQueue::Shutdown()
{
	Clear();
}

void D3D11RenderQueue::Clear()
{
	for (int32 q = 0; q < EMAX_RQ; ++q)
	{
		for(size_t i = 0; i < m_CmdList[q].size(); ++i)
		{
			D3D11RenderCommandPool::Get()->Release(m_CmdList[q][i]);
		}
		m_CmdList[q].clear();
	}

	m_pRenderCamera = nullptr;
}

void D3D11RenderQueue::Add(RENDER_QUEUE_TYPE QueueType, MESH_RENDER_COMMAND* pRenderCommand)
{
	if (QueueType >= EMAX_RQ || !pRenderCommand)
		return;
	m_CmdList[QueueType].push_back(pRenderCommand);
}

void D3D11RenderQueue::Process()
{
}
