#include "pch.h"
#include "D3D11RenderStateCache.h"
#include "D3D11RenderState.h"


D3D11RenderStateCache::~D3D11RenderStateCache() noexcept
{
	Shutdown();
}

void D3D11RenderStateCache::Init()
{
	m_pCache_BlendState = create_hash_table(64);
	m_pCache_SamplerState = create_hash_table(64);
	m_pCache_DepthStencilState = create_hash_table(64);
	m_pCache_RasterizerState = create_hash_table(64);
}

void D3D11RenderStateCache::Shutdown()
{
	if (m_pCache_BlendState)
	{
		for (int b = 0; b < m_pCache_BlendState->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pCache_BlendState->buckets[b];

			while (pNode)
			{
				HASH_NODE* pNext = pNode->next;

				D3D11BlendState* pState = (D3D11BlendState*)pNode->data;
				pState->Release();

				pNode = pNext;
			}
		}

		delete_hash_table(m_pCache_BlendState);
		m_pCache_BlendState = nullptr;
	}

	if (m_pCache_SamplerState)
	{
		for (int b = 0; b < m_pCache_SamplerState->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pCache_SamplerState->buckets[b];

			while (pNode)
			{
				HASH_NODE* pNext = pNode->next;

				D3D11SamplerState* pState = (D3D11SamplerState*)pNode->data;
				pState->Release();

				pNode = pNext;
			}
		}

		delete_hash_table(m_pCache_SamplerState);
		m_pCache_SamplerState = nullptr;
	}

	if (m_pCache_DepthStencilState)
	{
		for (int b = 0; b < m_pCache_DepthStencilState->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pCache_DepthStencilState->buckets[b];

			while (pNode)
			{
				HASH_NODE* pNext = pNode->next;

				D3D11DepthStencilState* pState = (D3D11DepthStencilState*)pNode->data;
				pState->Release();

				pNode = pNext;
			}
		}

		delete_hash_table(m_pCache_DepthStencilState);
		m_pCache_DepthStencilState = nullptr;
	}

	if (m_pCache_RasterizerState)
	{
		for (int b = 0; b < m_pCache_RasterizerState->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pCache_RasterizerState->buckets[b];

			while (pNode)
			{
				HASH_NODE* pNext = pNode->next;

				D3D11RasterizerState* pState = (D3D11RasterizerState*)pNode->data;
				pState->Release();

				pNode = pNext;
			}
		}

		delete_hash_table(m_pCache_RasterizerState);
		m_pCache_RasterizerState = nullptr;
	}
}

void D3D11RenderStateCache::Add(uint64 Hash, D3D11BlendState* pState)
{
	if (!Hash || !pState)
		return;

	insert_hash_node(m_pCache_BlendState, Hash, pState->INL_GetHashNode());
}

void D3D11RenderStateCache::Add(uint64 Hash, D3D11SamplerState* pState)
{
	if (!Hash || !pState)
		return;

	insert_hash_node(m_pCache_SamplerState, Hash, pState->INL_GetHashNode());
}

void D3D11RenderStateCache::Add(uint64 Hash, D3D11DepthStencilState* pState)
{
	if (!Hash || !pState)
		return;

	insert_hash_node(m_pCache_DepthStencilState, Hash, pState->INL_GetHashNode());
}

void D3D11RenderStateCache::Add(uint64 Hash, D3D11RasterizerState* pState)
{
	if (!Hash || !pState)
		return;

	insert_hash_node(m_pCache_RasterizerState, Hash, pState->INL_GetHashNode());
}

BOOL D3D11RenderStateCache::TryGet(uint64 Hash, D3D11BlendState** ppOut)
{
	if (!Hash || !ppOut || !*ppOut)
		return FALSE;

	D3D11BlendState* pState = (D3D11BlendState*)query_hash_node(m_pCache_BlendState, Hash);
	if (!pState)
	{
		*ppOut = nullptr;
		return FALSE;
	}

	pState->AddRef();

	*ppOut = pState;

	return TRUE;
}

BOOL D3D11RenderStateCache::TryGet(uint64 Hash, D3D11SamplerState** ppOut)
{
	if (!Hash || !ppOut || !*ppOut)
		return FALSE;

	D3D11SamplerState* pState = (D3D11SamplerState*)query_hash_node(m_pCache_SamplerState, Hash);
	if (!pState)
	{
		*ppOut = nullptr;
		return FALSE;
	}

	pState->AddRef();

	*ppOut = pState;

	return TRUE;
}

BOOL D3D11RenderStateCache::TryGet(uint64 Hash, D3D11DepthStencilState** ppOut)
{
	if (!Hash || !ppOut || !*ppOut)
		return FALSE;

	D3D11DepthStencilState* pState = (D3D11DepthStencilState*)query_hash_node(m_pCache_DepthStencilState, Hash);
	if (!pState)
	{
		*ppOut = nullptr;
		return FALSE;
	}

	pState->AddRef();

	*ppOut = pState;

	return TRUE;
}

BOOL D3D11RenderStateCache::TryGet(uint64 Hash, D3D11RasterizerState** ppOut)
{
	if (!Hash || !ppOut || !*ppOut)
		return FALSE;

	D3D11RasterizerState* pState = (D3D11RasterizerState*)query_hash_node(m_pCache_RasterizerState, Hash);
	if (!pState)
	{
		*ppOut = nullptr;
		return FALSE;
	}

	pState->AddRef();

	*ppOut = pState;

	return TRUE;
}