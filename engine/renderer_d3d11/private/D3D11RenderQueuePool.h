#ifndef __D3D11_RENDER_QUEUE_POOL_H__
#define __D3D11_RENDER_QUEUE_POOL_H__


#include "D3D11RenderQueue.h"

class D3D11RenderQueuePool
{
public:
	D3D11RenderQueuePool();
	~D3D11RenderQueuePool() noexcept;

	BOOL Init(size_t InitialCapacity);
	void Shutdown();

	D3D11RenderQueue* GetRQ();
	void ReleaseRQ(D3D11RenderQueue* pRQ);

private:
	LINKED_LIST m_PoolList;
	spin_lock_t m_Lock;

};


#endif // __D3D11_RENDER_QUEUE_POOL_H__
