#pragma once


class D3D11RenderQueue;

class D3D11RenderQueuePool
{
	static D3D11RenderQueuePool* s_pInstance;

public:
	D3D11RenderQueuePool();
	virtual ~D3D11RenderQueuePool() noexcept;

	void Init();
	void Shutdown() noexcept;

	D3D11RenderQueue* GetRQ() noexcept;
	void ReleaseRQ(D3D11RenderQueue* pRQ) noexcept;

	static inline D3D11RenderQueuePool& Get() noexcept { return *s_pInstance; }

private:
	void AllocRQ(size_t Count) noexcept;

private:
	LINKED_LIST m_FreeList;
	LINKED_LIST m_UsedList;

};