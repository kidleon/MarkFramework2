#include "pch.h"
#include "D3D11BlobPool.h"
#include "D3D11Blob.h"


D3D11BlobPool::~D3D11BlobPool()
{
    Shutdown();
}

BOOL D3D11BlobPool::Init(
    size_t BufferSize,
    size_t InitialCount
)
{
    init_linked_list(&m_FreeList);
    init_linked_list(&m_UsedList);

    m_InitialCount = InitialCount;
    AllocBlob(BufferSize, InitialCount);

    return TRUE;
}

void D3D11BlobPool::Shutdown()
{
    while (!linked_list_empty(&m_UsedList))
    {
        LINK_NODE* pNode = linked_list_pop_front(&m_UsedList);
        D3D11_BLOB* pBlob = static_cast<D3D11_BLOB*>(pNode->data);
        pBlob->Release();
    }

    while (!linked_list_empty(&m_FreeList))
    {
        LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
        D3D11_BLOB* pBlob = static_cast<D3D11_BLOB*>(pNode->data);
        pBlob->Release();
    }
}

D3D11_BLOB* D3D11BlobPool::Acquire()
{
    if (linked_list_empty(&m_FreeList))
    {
        AllocBlob(m_BufferSize, m_InitialCount >> 1);
    }

    LINK_NODE* pNode = linked_list_pop_front(&m_FreeList);
    linked_list_push_back(&m_UsedList, pNode);

    D3D11_BLOB* pBlob = static_cast<D3D11_BLOB*>(pNode->data);
    pBlob->AddRef();

    return pBlob;
}

void D3D11BlobPool::Release(D3D11_BLOB* pBlob)
{
    if (!pBlob) return;

    long RefCnt = pBlob->Release();
    if (!RefCnt) return;

    LINK_NODE* pNode = pBlob->INL_GetLinkNode();

    linked_list_remove_node(&m_UsedList, pNode);
    linked_list_push_back(&m_FreeList, pNode);
}

void D3D11BlobPool::AllocBlob(
    size_t BufferSize,
    size_t Count
)
{
    void* pAlignedBuffer = D3D11_SYS_ALLOC_ALGIN(BufferSize, 16);
    D3D11_BLOB* pBlob = D3D11_NEW(D3D11_BLOB)(pAlignedBuffer, BufferSize);

    linked_list_push_back(&m_FreeList, pBlob->INL_GetLinkNode());
}