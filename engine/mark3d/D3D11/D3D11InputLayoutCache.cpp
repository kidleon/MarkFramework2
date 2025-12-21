#include "pch.h"
#include "D3D11InputLayoutCache.h"
#include "D3D11InputLayout.h"


D3D11InputLayoutCache::D3D11InputLayoutCache()
	: m_pCacheTable(nullptr)
{
}

D3D11InputLayoutCache::~D3D11InputLayoutCache() noexcept
{
	if (m_pCacheTable)
	{
		for (int b = 0; b < m_pCacheTable->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pCacheTable->buckets[b];

			while (pNode)
			{
				HASH_NODE* pNext = pNode->next;

				D3D11InputLayout* pIL = (D3D11InputLayout*)pNode->data;
				MARK_POOL_DELETE(pIL, D3D11InputLayout);

				pNode = pNext;
			}
		}

		delete_hash_table(m_pCacheTable);
		m_pCacheTable = nullptr;
	}
}

void D3D11InputLayoutCache::Init()
{
	if (!m_pCacheTable)
	{
		m_pCacheTable = create_hash_table(16);
	}
}

void D3D11InputLayoutCache::AddInputLayout(UINT32 VertexFormat, D3D11InputLayout* pInputLayout)
{
	if (m_pCacheTable && pInputLayout)
	{
		insert_hash_node(m_pCacheTable, static_cast<int64>(VertexFormat), pInputLayout->INL_GetHashNode());
	}
}

D3D11InputLayout* D3D11InputLayoutCache::GetInputLayout(UINT32 VertexFormat)
{
	if (m_pCacheTable)
	{
		D3D11InputLayout* pInputLayout = (D3D11InputLayout*)query_hash_node(m_pCacheTable, static_cast<int64>(VertexFormat));
		return pInputLayout;
	}

	return nullptr;
}
