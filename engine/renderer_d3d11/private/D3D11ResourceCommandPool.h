#pragma once

struct D3D11_RESOURCE_COMMAND;

class D3D11ResourceCommandPool
{
	static D3D11ResourceCommandPool* s_pInstance;

public:
	D3D11ResourceCommandPool();
	~D3D11ResourceCommandPool() noexcept;

	BOOL Init(size_t InitialCapacity);
	void Cleanup();

	D3D11_RESOURCE_COMMAND* Acquire();
	void Release(D3D11_RESOURCE_COMMAND* pCommand);

	static inline D3D11ResourceCommandPool* Get() noexcept { return s_pInstance; }

private:
	void ExpandPool(size_t Count);

private:
	LINKED_LIST m_FreeList = {};
	LINKED_LIST m_UsedList = {};

};
