#include "pch.h"
#include "D3D11ShaderCache.h"
#include "D3D11Shader.h"


D3D11ShaderCache::D3D11ShaderCache()
	: m_pCacheTable(nullptr)
{
}

D3D11ShaderCache::~D3D11ShaderCache() noexcept
{
	if (m_pCacheTable)
	{
		for (size_t b = 0; b < m_pCacheTable->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pCacheTable->buckets[b];
			while (pNode)
			{
				HASH_NODE* pNextNode = pNode->next;
				D3D11Shader* pShader = (D3D11Shader*)pNode->data;
				if (pShader)
				{
					D3D11_POOL_DELETE(pShader, D3D11Shader);
					pShader = nullptr;
				}
				pNode = pNextNode;
			}
		}

		delete_hash_table(m_pCacheTable);
		m_pCacheTable = nullptr;
	}
}

void D3D11ShaderCache::Init()
{
	m_pCacheTable = create_hash_table(64);
}

void D3D11ShaderCache::Add(NameHash Name, D3D11Shader* pShader)
{
	insert_hash_node(m_pCacheTable, Name.value(), pShader->INL_GetHashNode());
}

D3D11Shader* D3D11ShaderCache::Get(NameHash Name)
{
	D3D11Shader* pShader = (D3D11Shader*)query_hash_node(m_pCacheTable, Name.value());
	return pShader;
}