#include "pch.h"
#include "D3D11RenderStateCache.h"
#include "D3D11RenderState.h"
#include "D3D11RenderDevice.h"


static inline D3D11RasterizerState* CreateRasterizerState(
	const RS_RASTERIZER_STATE& RasterizerState
)
{
	ID3D11RasterizerState* pD3D11_RS = nullptr;
	if (!D3D11RenderDevice::Get().CreateRasterizerState(RasterizerState, &pD3D11_RS))
	{
		SYS_LOG_E("CreateRasterizerState - Failed to create default rasterizer state.");
		return nullptr;
	}

	D3D11RasterizerState* pRS = D3D11_NEW(D3D11RasterizerState)(pD3D11_RS);
	return pRS;
}

D3D11RenderStateCache* D3D11RenderStateCache::s_pInstance = nullptr;

D3D11RenderStateCache::D3D11RenderStateCache()
	: m_pRasterizerStateCache(nullptr)
	, m_pBlendStateCache(nullptr)
{
	// Constructor
	s_pInstance = this;
}

D3D11RenderStateCache::~D3D11RenderStateCache() noexcept
{
	Shutdown();
	s_pInstance = nullptr;
}

void D3D11RenderStateCache::Init()
{
	m_pRasterizerStateCache = create_hash_table(32); // Initialize rasterizer state cache
	m_pBlendStateCache = create_hash_table(32); // Initialize blend state cache

	// Pre-create common rasterizer states
	{
		RS_RASTERIZER_STATE DefaultState = GetRS_Default();
		uint64 Hash = fnv64_c(&DefaultState, sizeof(RS_RASTERIZER_STATE));
		D3D11RasterizerState* pRS = CreateRasterizerState(DefaultState);
		insert_hash_node(
			m_pRasterizerStateCache,
			(int64)Hash,
			pRS->INL_GetHashNode()
		);
	}
	
	{
		RS_RASTERIZER_STATE WireframeState = GetRS_Wireframe();
		uint64 Hash = fnv64_c(&WireframeState, sizeof(RS_RASTERIZER_STATE));
		D3D11RasterizerState* pRS = CreateRasterizerState(WireframeState);
		insert_hash_node(
			m_pRasterizerStateCache,
			(int64)Hash,
			pRS->INL_GetHashNode()
		);
	}

	{
		RS_RASTERIZER_STATE WireframeTwoSideState = GetRS_WireframeTwoSide();
		uint64 Hash = fnv64_c(&WireframeTwoSideState, sizeof(RS_RASTERIZER_STATE));
		D3D11RasterizerState* pRS = CreateRasterizerState(WireframeTwoSideState);
		insert_hash_node(
			m_pRasterizerStateCache,
			(int64)Hash,
			pRS->INL_GetHashNode()
		);
	}

	{
		RS_RASTERIZER_STATE TwoSideState = GetRS_TwoSide();
		uint64 Hash = fnv64_c(&TwoSideState, sizeof(RS_RASTERIZER_STATE));
		D3D11RasterizerState* pRS = CreateRasterizerState(TwoSideState);
		insert_hash_node(
			m_pRasterizerStateCache,
			(int64)Hash,
			pRS->INL_GetHashNode()
		);
	}

	// Pre-create common blend states

	
}

void D3D11RenderStateCache::Shutdown()
{
	// Release rasterizer state cache
	if (m_pRasterizerStateCache)
	{
		for (size_t b = 0; b < m_pRasterizerStateCache->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pRasterizerStateCache->buckets[b];
			while (pNode)
			{
				HASH_NODE* pNextNode = pNode->next;

				D3D11RasterizerState* pCacheData = reinterpret_cast<D3D11RasterizerState*>(pNode->data);
				if (pCacheData)
				{
					D3D11_DELETE(pCacheData, D3D11RasterizerState);
					pCacheData = nullptr;
				}
				pNode = pNextNode;
			}
			m_pRasterizerStateCache->buckets[b] = nullptr;
		}

		delete_hash_table(m_pRasterizerStateCache);
		m_pRasterizerStateCache = nullptr;
	}

	// Release blend state cache
	if (m_pBlendStateCache)
	{
		for (size_t b = 0; b < m_pBlendStateCache->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pBlendStateCache->buckets[b];
			while (pNode)
			{
				HASH_NODE* pNextNode = pNode->next;

				ID3D11BlendState* pCacheData = reinterpret_cast<ID3D11BlendState*>(pNode->data);
				if (pCacheData)
				{
					D3D11_DELETE(pCacheData, ID3D11BlendState);
					pCacheData = nullptr;
				}
				pNode = pNextNode;
			}
			m_pBlendStateCache->buckets[b] = nullptr;
		}

		delete_hash_table(m_pBlendStateCache);
		m_pBlendStateCache = nullptr;
	}
}

ID3D11RasterizerState* D3D11RenderStateCache::Register(const RS_RASTERIZER_STATE& RasterizerState) noexcept
{
	uint64 Hash = fnv64_c(&RasterizerState, sizeof(RS_RASTERIZER_STATE));

	D3D11RasterizerState* pRS = (D3D11RasterizerState*)query_hash_node(
		m_pRasterizerStateCache,
		(int64)Hash
	);

	if (pRS)
	{
		return pRS->INL_GetD3D11RasterizerState();
	}

	pRS = CreateRasterizerState(RasterizerState);

	insert_hash_node(
		m_pRasterizerStateCache,
		(int64)Hash,
		pRS->INL_GetHashNode()
	);

	return pRS->INL_GetD3D11RasterizerState();
}

void D3D11RenderStateCache::Register(const RS_BLEND_STATE& BlendState) noexcept
{
}

ID3D11RasterizerState* D3D11RenderStateCache::Find(const RS_RASTERIZER_STATE& RasterizerState) noexcept
{
	uint64 Hash = fnv64_c(&RasterizerState, sizeof(RS_RASTERIZER_STATE));

	return Find(Hash);
}

ID3D11RasterizerState* D3D11RenderStateCache::Find(uint64 Hash) noexcept
{
	D3D11RasterizerState* pRS = (D3D11RasterizerState*)query_hash_node(
		m_pRasterizerStateCache,
		(int64)Hash
	);

	if (!pRS) return nullptr; // 찾지 못함

	return pRS->INL_GetD3D11RasterizerState();
}

ID3D11BlendState* D3D11RenderStateCache::Find(const RS_BLEND_STATE& BlendState) noexcept
{
	return nullptr;
}