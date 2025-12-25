#ifndef __D3D11_BUFFER_POOL_H__
#define __D3D11_BUFFER_POOL_H__


class D3D11Buffer;
class D3D11RenderDevice;

class D3D11BufferPool
{
	struct D3D11BufferPoolPage
	{
		UINT32 PageID;
		UINT32 PageSize;
		HANDLE hFreelist;
		ID3D11Buffer* pD3D11Buffer;
		
		LINK_NODE LinkNode;
	};

public:
	D3D11BufferPool(D3D11RenderDevice* pRenderDevice);
	~D3D11BufferPool() noexcept;

	BOOL Init(
		size_t VertexBufferSize, 
		size_t IndexBufferSize,
		size_t MaxPages
	);

	void Cleanup();

	D3D11Buffer* AllocVB(size_t Size);
	D3D11Buffer* AllocIB(size_t Size);

private:
	D3D11BufferPoolPage* CreatePoolPage(
		D3D11_BUFFER_TYPE BufferType,
		size_t PageSize,
		size_t MinPoolSize
	);

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;
	LINKED_LIST m_VBPoolPageList = {};
	LINKED_LIST m_IBPoolPageList = {};

};


#endif // __D3D11_BUFFER_POOL_H__
