#ifndef __D3D11_RENDER_QUEUE_H__
#define __D3D11_RENDER_QUEUE_H__

#include "TArray.h"

using namespace mark;

struct MESH_RENDER_COMMAND;
class D3D11RenderCamera;

class D3D11RenderQueue
{
	enum RENDER_QUEUE_TYPE : uint8
	{
		RENDER_QUEUE_TYPE_OPAQUE = 0,
		RENDER_QUEUE_TYPE_TRANSPARENCY,
		RENDER_QUEUE_TYPE_EMAX
	};

public:
	D3D11RenderQueue() = default;
	~D3D11RenderQueue() noexcept;

	void Init(size_t InitialCapacity);
	void Shutdown();
	void Clear();

	void Add(RENDER_QUEUE_TYPE QueueType, MESH_RENDER_COMMAND* pRenderCommand);
	void Process();

private:
	D3D11RenderCamera* m_pRenderCamera = nullptr;
	TArray<MESH_RENDER_COMMAND*, TD3D11Allocator<ALLOC_TYPE::POOL>> m_RenderCommands[RENDER_QUEUE_TYPE_EMAX];

};


#endif // __D3D11_RENDER_QUEUE_H__
