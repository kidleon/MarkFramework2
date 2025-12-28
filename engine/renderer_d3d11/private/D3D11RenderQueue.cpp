#include "pch.h"
#include "D3D11RenderQueue.h"
#include "D3D11RenderCommand.h"
#include "D3D11RenderCommandPool.h"
#include "D3D11RenderState.h"


D3D11RenderQueue::~D3D11RenderQueue() noexcept
{
	Shutdown();
}

void D3D11RenderQueue::Init(size_t InitialCapacity)
{
	for (int32 q = 0; q < RENDER_QUEUE_TYPE_EMAX; ++q)
	{
		m_RenderCommands[q].reserve(InitialCapacity);
	}
}

void D3D11RenderQueue::Shutdown()
{
	Clear();
}

void D3D11RenderQueue::Clear()
{
	for (int32 q = 0; q < RENDER_QUEUE_TYPE_EMAX; ++q)
	{
		for(size_t i = 0; i < m_RenderCommands[q].size(); ++i)
		{
			D3D11RenderCommandPool::Get()->Release(m_RenderCommands[q][i]);
		}
		m_RenderCommands[q].clear();
	}
}

void D3D11RenderQueue::Add(RENDER_QUEUE_TYPE QueueType, MESH_RENDER_COMMAND* pRenderCommand)
{
	if (QueueType >= RENDER_QUEUE_TYPE_EMAX || !pRenderCommand)
		return;
	m_RenderCommands[QueueType].push_back(pRenderCommand);
}

void D3D11RenderQueue::Process()
{
}
