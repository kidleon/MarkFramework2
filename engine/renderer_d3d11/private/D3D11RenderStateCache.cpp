#include "pch.h"
#include "D3D11RenderStateCache.h"
#include "D3D11RenderState.h"
#include "D3D11RenderDevice.h"


static constexpr size_t MAX_STATE_INDEX = 0xFFFFu - 1;

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

static inline D3D11BlendState* CreateBlendState(
	const RS_BLEND_STATE& BlendState
)
{
	ID3D11BlendState* pD3D11_BS = nullptr;
	if (!D3D11RenderDevice::Get().CreateBlendState(BlendState, &pD3D11_BS))
	{
		SYS_LOG_E("CreateBlendState - Failed to create default blend state.");
		return nullptr;
	}

	D3D11BlendState* pBS = D3D11_NEW(D3D11BlendState)(pD3D11_BS);
	return pBS;
}

static inline D3D11DepthStencilState* CreateDepthStencilState(
	const RS_DEPTH_STENCIL_STATE& DepthStencilState
)
{
	ID3D11DepthStencilState* pD3D11_DSS = nullptr;
	if (!D3D11RenderDevice::Get().CreateDepthStencilState(DepthStencilState, &pD3D11_DSS))
	{
		SYS_LOG_E("CreateDepthStencilState - Failed to create default depth stencil state.");
		return nullptr;
	}

	D3D11DepthStencilState* pDSS = D3D11_NEW(D3D11DepthStencilState)(pD3D11_DSS);
	return pDSS;
}


D3D11RenderStateCache* D3D11RenderStateCache::s_pInstance = nullptr;

D3D11RenderStateCache::D3D11RenderStateCache()
	: m_pRasterizerStateCache(nullptr)
	, m_pBlendStateCache(nullptr)
	, m_pDepthStencilStateCache(nullptr)
	, m_pSamplerStateCache(nullptr)
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
	m_pDepthStencilStateCache = create_hash_table(32); // Initialize depth-stencil state cache

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
	{
		RS_BLEND_STATE DefaultState;
		uint64 Hash = fnv64_c(&DefaultState, sizeof(RS_BLEND_STATE));
		D3D11BlendState* pBS = CreateBlendState(DefaultState);
		insert_hash_node(
			m_pBlendStateCache,
			(int64)Hash,
			pBS->INL_GetHashNode()
		);
	}
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

				D3D11BlendState* pCacheData = reinterpret_cast<D3D11BlendState*>(pNode->data);
				if (pCacheData)
				{
					D3D11_DELETE(pCacheData, D3D11BlendState);
					pCacheData = nullptr;
				}
				pNode = pNextNode;
			}
			m_pBlendStateCache->buckets[b] = nullptr;
		}

		delete_hash_table(m_pBlendStateCache);
		m_pBlendStateCache = nullptr;
	}

	if (m_pDepthStencilStateCache)
	{
		for (size_t b = 0; b < m_pDepthStencilStateCache->bucket_size; ++b)
		{
			HASH_NODE* pNode = m_pDepthStencilStateCache->buckets[b];
			while (pNode)
			{
				HASH_NODE* pNextNode = pNode->next;

				D3D11DepthStencilState* pCacheData = reinterpret_cast<D3D11DepthStencilState*>(pNode->data);
				if (pCacheData)
				{
					D3D11_DELETE(pCacheData, D3D11DepthStencilState);
					pCacheData = nullptr;
				}
				pNode = pNextNode;
			}
			m_pDepthStencilStateCache->buckets[b] = nullptr;
		}

		delete_hash_table(m_pDepthStencilStateCache);
		m_pDepthStencilStateCache = nullptr;
	}
}

D3D11RasterizerState* D3D11RenderStateCache::Register(const RS_RASTERIZER_STATE& RasterizerState) noexcept
{
	uint64 Hash = fnv64_c(&RasterizerState, sizeof(RS_RASTERIZER_STATE));

	D3D11RasterizerState* pRS = (D3D11RasterizerState*)query_hash_node(
		m_pRasterizerStateCache,
		(int64)Hash
	);

	if (pRS)
		return pRS;

	pRS = CreateRasterizerState(RasterizerState);

	insert_hash_node(
		m_pRasterizerStateCache,
		(int64)Hash,
		pRS->INL_GetHashNode()
	);

	return pRS;
}

D3D11BlendState* D3D11RenderStateCache::Register(const RS_BLEND_STATE& BlendState) noexcept
{
	uint64 Hash = fnv64_c(&BlendState, sizeof(RS_BLEND_STATE));

	D3D11BlendState* pBS = (D3D11BlendState*)query_hash_node(
		m_pBlendStateCache,
		(int64)Hash
	);

	if (pBS)
		return pBS;

	pBS = CreateBlendState(BlendState);

	insert_hash_node(
		m_pBlendStateCache,
		(int64)Hash,
		pBS->INL_GetHashNode()
	);

	return pBS;
}

D3D11DepthStencilState* D3D11RenderStateCache::Register(const RS_DEPTH_STENCIL_STATE& DepthStencilState) noexcept
{
	uint64 Hash = fnv64_c(&DepthStencilState, sizeof(RS_DEPTH_STENCIL_STATE));

	D3D11DepthStencilState* pDSS = (D3D11DepthStencilState*)query_hash_node(
		m_pDepthStencilStateCache,
		(int64)Hash
	);

	if (pDSS)
		return pDSS;

	pDSS = CreateDepthStencilState(DepthStencilState);

	insert_hash_node(
		m_pDepthStencilStateCache,
		(int64)Hash,
		pDSS->INL_GetHashNode()
	);

	return pDSS;
}

D3D11RasterizerState* D3D11RenderStateCache::Find_RS(const RS_RASTERIZER_STATE& RasterizerState) noexcept
{
	uint64 Hash = fnv64_c(&RasterizerState, sizeof(RS_RASTERIZER_STATE));
	return Find_RS(Hash);
}

D3D11RasterizerState* D3D11RenderStateCache::Find_RS(uint64 Hash) noexcept
{
	D3D11RasterizerState* pRS = (D3D11RasterizerState*)query_hash_node(
		m_pRasterizerStateCache,
		(int64)Hash
	);
	return pRS;
}

D3D11BlendState* D3D11RenderStateCache::Find_BS(const RS_BLEND_STATE& BlendState) noexcept
{
	uint64 Hash = fnv64_c(&BlendState, sizeof(RS_BLEND_STATE));
	return Find_BS(Hash);
}

D3D11BlendState* D3D11RenderStateCache::Find_BS(uint64 Hash) noexcept
{
	D3D11BlendState* pBS = (D3D11BlendState*)query_hash_node(
		m_pBlendStateCache,
		(int64)Hash
	);
	return pBS;
}

D3D11DepthStencilState* D3D11RenderStateCache::Find_DSS(const RS_DEPTH_STENCIL_STATE& DepthStencilState) noexcept
{
	uint64 Hash = fnv64_c(&DepthStencilState, sizeof(RS_DEPTH_STENCIL_STATE));
	return Find_DSS(Hash);
}

D3D11DepthStencilState* D3D11RenderStateCache::Find_DSS(uint64 Hash) noexcept
{
	D3D11DepthStencilState* pDSS = (D3D11DepthStencilState*)query_hash_node(
		m_pDepthStencilStateCache,
		(int64)Hash
	);
	return pDSS;
}

