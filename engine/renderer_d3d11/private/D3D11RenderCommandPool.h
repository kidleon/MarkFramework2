#ifndef __D3D11_RENDER_COMMAND_POOL_H__
#define __D3D11_RENDER_COMMAND_POOL_H__


struct MESH_RENDER_COMMAND;

class D3D11RenderCommandPool
{
	static D3D11RenderCommandPool* s_pInstance;

public:
	D3D11RenderCommandPool();
	~D3D11RenderCommandPool() noexcept;

	BOOL Init(size_t InitialCapacity);
	void Cleanup();

	MESH_RENDER_COMMAND* Alloc();
	void Release(MESH_RENDER_COMMAND* pCommand);

	static inline D3D11RenderCommandPool* Get() noexcept
	{
		return s_pInstance;
	}

private:
	void ExpandPool(size_t Count);

private:
	LINKED_LIST m_FreeList = {};
	LINKED_LIST m_UsedList = {};

};


#endif // __D3D11_RENDER_COMMAND_POOL_H__