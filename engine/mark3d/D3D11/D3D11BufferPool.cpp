#include "pch.h"
#include "D3D11BufferPool.h"
#include "vfreelist.h"
#include "D3D11RenderDevice.h"


constexpr size_t MIN_VB_POOL_SIZE = 4096;
constexpr size_t MIN_IB_POOL_SIZE = 512;

D3D11BufferPool::D3D11BufferPool(D3D11RenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice)
{
}

D3D11BufferPool::~D3D11BufferPool() noexcept
{
	Cleanup();
}

BOOL D3D11BufferPool::Init(
	size_t VertexBufferSize,
	size_t IndexBufferSize,
	size_t MaxPages
)
{
	init_linked_list(&m_VBPoolPageList);
	init_linked_list(&m_IBPoolPageList);

	D3D11BufferPoolPage* pPoolPage_VB = CreatePoolPage(
		D3D11_BUFFER_TYPE::VERTEX_BUFFER,
		VertexBufferSize,
		MIN_VB_POOL_SIZE
	);

	if (!pPoolPage_VB)
		return FALSE;

	linked_list_push_back(&m_VBPoolPageList, &pPoolPage_VB->LinkNode);

	D3D11BufferPoolPage* pPoolPage_IB = CreatePoolPage(
		D3D11_BUFFER_TYPE::INDEX_BUFFER,
		IndexBufferSize,
		MIN_IB_POOL_SIZE
	);

	if (!pPoolPage_IB)
		return FALSE;

	linked_list_push_back(&m_IBPoolPageList, &pPoolPage_IB->LinkNode);

	return TRUE;
}

void D3D11BufferPool::Cleanup()
{
	while (!linked_list_empty(&m_VBPoolPageList))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_VBPoolPageList);
		D3D11BufferPoolPage* pPoolPage = (D3D11BufferPoolPage*)pNode->data;

		if (pPoolPage->pD3D11Buffer)
		{
			pPoolPage->pD3D11Buffer->Release();
			pPoolPage->pD3D11Buffer = nullptr;
		}

		if (pPoolPage->hFreelist)
		{
			vfreelist_destroy(pPoolPage->hFreelist);
			pPoolPage->hFreelist = nullptr;
		}

		MARK_POOL_FREE(pPoolPage);
	}

	while (!linked_list_empty(&m_IBPoolPageList))
	{
		LINK_NODE* pNode = linked_list_pop_front(&m_IBPoolPageList);

		D3D11BufferPoolPage* pPoolPage = (D3D11BufferPoolPage*)pNode->data;

		if (pPoolPage->pD3D11Buffer)
		{
			pPoolPage->pD3D11Buffer->Release();
			pPoolPage->pD3D11Buffer = nullptr;
		}

		if (pPoolPage->hFreelist)
		{
			vfreelist_destroy(pPoolPage->hFreelist);
			pPoolPage->hFreelist = nullptr;
		}

		MARK_POOL_FREE(pPoolPage);
	}
}

D3D11Buffer* D3D11BufferPool::AllocVB(size_t Size)
{

	return nullptr;
}

D3D11Buffer* D3D11BufferPool::AllocIB(size_t Size)
{
	return nullptr;
}

D3D11BufferPool::D3D11BufferPoolPage* D3D11BufferPool::CreatePoolPage(
	D3D11_BUFFER_TYPE BufferType,
	size_t PageSize,
	size_t MinPoolSize
)
{
	if ((MinPoolSize & (MinPoolSize - 1)) != 0)
		return nullptr;

	static uint32 PAGE_ID_COUNTER = 0;

	size_t AlignedSize = ALIGNED_SIZE(PageSize, 64);

	HANDLE hFreeListVB = vfreelist_create(AlignedSize, MinPoolSize);
	if (!hFreeListVB)
		return nullptr;

	D3D11BufferPoolPage* pPoolPages = (D3D11BufferPoolPage*)MARK_POOL_ALLOC(sizeof(D3D11BufferPoolPage));
	pPoolPages->PageID = PAGE_ID_COUNTER++;
	pPoolPages->PageSize = (UINT32)AlignedSize;
	pPoolPages->hFreelist = hFreeListVB;

	ID3D11Buffer* pD3D11Buffer = nullptr;
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = (UINT)AlignedSize;

	switch (BufferType)
	{
		case D3D11_BUFFER_TYPE::VERTEX_BUFFER:
			BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			break;

		case D3D11_BUFFER_TYPE::INDEX_BUFFER:
			BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;
	}

	if (!m_pRenderDevice->CreateBuffer(
		&BufferDesc,
		&pD3D11Buffer
	))
	{
		MARK_POOL_FREE(pPoolPages);
		pPoolPages = nullptr;

		return FALSE;
	}

	pPoolPages->pD3D11Buffer = pD3D11Buffer;
	pPoolPages->LinkNode.data = pPoolPages;

	return pPoolPages;
}
