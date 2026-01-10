#pragma once


class D3D11_RENDER_QUEUE;

class D3D11RenderQueuePool
{
	static D3D11RenderQueuePool* s_pInstance;

public:
	D3D11RenderQueuePool();
	virtual ~D3D11RenderQueuePool() noexcept;

	void Init();
	void Shutdown() noexcept;

	D3D11_RENDER_QUEUE* GetRQ() noexcept;
	void ReleaseRQ(D3D11_RENDER_QUEUE* pRQ) noexcept;

	static inline D3D11RenderQueuePool& Get() noexcept { return *s_pInstance; }

private:
	void AllocRQ(size_t Count) noexcept;

private:
	LINKED_LIST m_FreeList;
	LINKED_LIST m_UsedList;
	BOOL m_Shutdown;

};