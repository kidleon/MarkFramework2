#include "pch.h"
#include "D3D11InputLayoutCache.h"

D3D11InputLayoutCache* D3D11InputLayoutCache::s_pInstance = nullptr;


D3D11InputLayoutCache::D3D11InputLayoutCache()
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11InputLayoutCache::~D3D11InputLayoutCache() noexcept
{
	Shutdown();
	s_pInstance = nullptr;
}

BOOL D3D11InputLayoutCache::Init()
{
	m_pHashTable = create_hash_table(32);
	return TRUE;
}

void D3D11InputLayoutCache::Shutdown()
{
	if (!m_pHashTable)
		return;

	for (size_t b = 0; b < m_pHashTable->bucket_size; ++b)
	{
		HASH_NODE* pNode = m_pHashTable->buckets[b];
		while (pNode)
		{
			HASH_NODE* pNextNode = pNode->next;

			CACHE_DATA* pCacheData = reinterpret_cast<CACHE_DATA*>(pNode->data);
			if (pCacheData)
			{
				CHECK_RELEASE(pCacheData->pInputLayout);
				D3D11_POOL_FREE(pCacheData, CACHE_DATA);
				pCacheData = nullptr;
			}
			pNode = pNextNode;
		}
		m_pHashTable->buckets[b] = nullptr;
	}

	delete_hash_table(m_pHashTable);
	m_pHashTable = nullptr;
}

void D3D11InputLayoutCache::Register(UINT32 VertexFormat, ID3D11InputLayout* pInputLayout) noexcept
{
	if (!m_pHashTable || !pInputLayout)
		return;

	UINT32 VertexFormatHash = fnv1(&VertexFormat, sizeof(UINT32), 0);

	CACHE_DATA* pCacheData = (CACHE_DATA*)D3D11_POOL_ALLOC(sizeof(CACHE_DATA));
	pCacheData->pInputLayout = pInputLayout;
	pCacheData->HashNode.data = pCacheData;

	insert_hash_node(
		m_pHashTable,
		VertexFormatHash,
		&pCacheData->HashNode
	);
}

ID3D11InputLayout* D3D11InputLayoutCache::Find(UINT32 VertexFormat) noexcept
{
	if (!m_pHashTable)
		return nullptr;

	UINT32 VertexFormatHash = fnv1(&VertexFormat, sizeof(UINT32), 0);
	CACHE_DATA* pCacheData = (CACHE_DATA*)query_hash_node(m_pHashTable, VertexFormatHash);

	if (!pCacheData)
		return nullptr;
	
	return pCacheData->pInputLayout;
}