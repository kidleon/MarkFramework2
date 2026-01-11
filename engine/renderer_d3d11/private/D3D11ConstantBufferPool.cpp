#include "pch.h"
#include "D3D11ConstantBufferPool.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11RenderDevice.h"


D3D11ConstantBufferPool::~D3D11ConstantBufferPool()
{
	Shutdown();
}

BOOL D3D11ConstantBufferPool::Init(
    D3D11RenderDevice* pRenderDevice,
    size_t BufferSize,
    size_t InitialCount,
    BOOL EnableReset
)
{
	init_linked_list(&m_FreeList);
	init_linked_list(&m_UsedList);

	m_InitialCount = InitialCount;
	AllocBuffer(pRenderDevice, BufferSize, InitialCount);
	
	m_pRenderDevice = pRenderDevice;
	m_EnableReset = EnableReset;
    
	return TRUE;
}

void D3D11ConstantBufferPool::Shutdown()
{
    while (!linked_list_empty(&m_UsedList))
    {
		LINK_NODE* pNode = linked_list_pop_front(&m_UsedList);
        D3D11ConstantBuffer* pCB = static_cast<D3D11ConstantBuffer*>(pNode->data);
		pCB->Release();
    }

    while (!linked_list_empty(&m_FreeList))
    {
        LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
        D3D11ConstantBuffer* pCB = static_cast<D3D11ConstantBuffer*>(pNode->data);
        pCB->Release();
    }

	m_pRenderDevice = nullptr;
}

void D3D11ConstantBufferPool::Reset()
{
    if (!m_EnableReset)
        return;

    while (!linked_list_empty(&m_UsedList))
    {
        LINK_NODE* pNode = linked_list_pop_front(&m_UsedList);

        D3D11ConstantBuffer* pCB = static_cast<D3D11ConstantBuffer*>(pNode->data);
        pCB->Release();

        linked_list_push_back(&m_FreeList, pNode);
	}
}

D3D11ConstantBuffer* D3D11ConstantBufferPool::Acquire()
{
    if (linked_list_empty(&m_FreeList))
    {
        AllocBuffer(m_pRenderDevice, m_BufferSize, m_InitialCount >> 1);
    }

    LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
    linked_list_push_back(&m_UsedList, pNode);

    D3D11ConstantBuffer* pCB = static_cast<D3D11ConstantBuffer*>(pNode->data);
    pCB->AddRef();

	return pCB;
}

void D3D11ConstantBufferPool::Release(D3D11ConstantBuffer* pCB)
{
    if (!pCB) return;
	pCB->Release();

    LINK_NODE* pNode = pCB->INL_GetLinkNode();

	linked_list_remove_node(&m_UsedList, pNode);
    linked_list_push_back(&m_FreeList, pNode);
}

void D3D11ConstantBufferPool::AllocBuffer(
    D3D11RenderDevice* pRenderDevice,
    size_t BufferSize,
    size_t Count
)
{
    ID3D11Device* pDevice = pRenderDevice->INL_GetD3D11Device();

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT32>(BufferSize);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    for (size_t i = 0; i < Count; ++i)
    {
        ID3D11Buffer* pD3D11Buffer = nullptr;
        HRESULT hr = pDevice->CreateBuffer(&desc, nullptr, &pD3D11Buffer);
        if (FAILED(hr))
        {
			__ASSERT(FALSE, "D3D11ConstantBufferPool::AllocBuffer: Failed to create constant buffer.");
            return;
        }

		void* pAlignedBuffer = D3D11_SYS_ALLOC_ALGIN(BufferSize, 16);
        D3D11ConstantBuffer* pCB = D3D11_POOL_NEW(D3D11ConstantBuffer)(pD3D11Buffer, pAlignedBuffer, BufferSize);

        linked_list_push_back(&m_FreeList, pCB->INL_GetLinkNode());
    }
}