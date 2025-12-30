#ifndef __D3D11_RENDER_MANAGER_H__
#define __D3D11_RENDER_MANAGER_H__

#include "TArray.h"
using namespace mark;


class D3D11RenderQueuePool;
class D3D11RenderQueue;

class D3D11RenderManager
{
	static D3D11RenderManager* s_pInstance;

public:
	static __FORCEINLINE D3D11RenderManager* Get() noexcept
	{
		return s_pInstance;
	}

public:
	D3D11RenderManager();
	~D3D11RenderManager() noexcept;

	BOOL Init();
	void Shutdown();

	D3D11RenderQueue* PrepareRQ() noexcept;
	void PostRQ(D3D11RenderQueue* pRQ) noexcept;

private:
	D3D11RenderQueuePool* m_pRQPool = nullptr;
	TArray<D3D11RenderQueue*, TA_POOL> m_ActiveRQList;
	spin_lock_t m_RQLock;

};


#endif // __D3D11_RENDER_MANAGER_H__
