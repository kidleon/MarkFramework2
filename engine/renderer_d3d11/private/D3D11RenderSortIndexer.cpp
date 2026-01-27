#include "pch.h"
#include "D3D11RenderSortIndexer.h"
#include "stack_pool.h"


struct RENDER_SORT_INDEX_ELEMENT
{
	UINT64 Key;
	UINT32 Index;
	HASH_NODE HashNode;
};

static inline UINT64 MakeStateKey(
	UINT16 BlendStateIndex,
	UINT16 RasterizeStateIndex,
	UINT16 DepthStencilStateIndex
)
{
	UINT64 key = 0;
	key |= (static_cast<UINT64>(RasterizeStateIndex) << 32);
	key |= (static_cast<UINT64>(BlendStateIndex) << 16);
	key |= static_cast<UINT64>(DepthStencilStateIndex);

	return key;
}

RENDER_SORT_INDEXER::RENDER_SORT_INDEXER() noexcept
	: pVertexShaderIndexTable(nullptr)
	, pPixelShaderIndexTable(nullptr)
	, pTextureIndexTable(nullptr)
	, pStateIndexTable(nullptr)
{
}

void RENDER_SORT_INDEXER::Init()
{
	pVertexShaderIndexTable = create_hash_table(128);
	pPixelShaderIndexTable = create_hash_table(128);
	pTextureIndexTable = create_hash_table(256);
	pStateIndexTable = create_hash_table(128);
}

void RENDER_SORT_INDEXER::Destroy()
{
	if (pVertexShaderIndexTable)
	{
		delete_hash_table(pVertexShaderIndexTable);
		pVertexShaderIndexTable = nullptr;
	}

	if (pPixelShaderIndexTable)
	{
		delete_hash_table(pPixelShaderIndexTable);
		pPixelShaderIndexTable = nullptr;
	}

	if (pTextureIndexTable)
	{
		delete_hash_table(pTextureIndexTable);
		pTextureIndexTable = nullptr;
	}

	if (pStateIndexTable)
	{
		delete_hash_table(pStateIndexTable);
		pStateIndexTable = nullptr;
	}
}

void RENDER_SORT_INDEXER::Reset()
{
	if (pVertexShaderIndexTable)
	{
		for (size_t i = 0; i < pVertexShaderIndexTable->bucket_size; ++i)
			pVertexShaderIndexTable->buckets[i] = nullptr;
		pVertexShaderIndexTable->node_count = 0;
	}

	if (pPixelShaderIndexTable)
	{
		for (size_t i = 0; i < pPixelShaderIndexTable->bucket_size; ++i)
			pPixelShaderIndexTable->buckets[i] = nullptr;
		pPixelShaderIndexTable->node_count = 0;
	}

	if (pTextureIndexTable)
	{
		for (size_t i = 0; i < pTextureIndexTable->bucket_size; ++i)
			pTextureIndexTable->buckets[i] = nullptr;
		pTextureIndexTable->node_count = 0;
	}

	if (pStateIndexTable)
	{
		for (size_t i = 0; i < pStateIndexTable->bucket_size; ++i)
			pStateIndexTable->buckets[i] = nullptr;
		pStateIndexTable->node_count = 0;
	}
}

uint32 RENDER_SORT_INDEXER::GetVertexShaderIndex(HANDLE StackPool, UINT64 key)
{
	void* pData = query_hash_node(
		pVertexShaderIndexTable,
		static_cast<int64>(key)
	);

	if (!pData)
	{
		RENDER_SORT_INDEX_ELEMENT* pNewElement = (RENDER_SORT_INDEX_ELEMENT*)stackpool_alloc(
			StackPool,
			sizeof(RENDER_SORT_INDEX_ELEMENT)
		);

		pNewElement->Key = key;
		pNewElement->Index = static_cast<UINT32>(pVertexShaderIndexTable->node_count);
		pNewElement->HashNode.key = static_cast<int64>(key);
		pNewElement->HashNode.data = pNewElement;

		int result =  insert_hash_node(
			pVertexShaderIndexTable,
			static_cast<int64>(key),
			&pNewElement->HashNode
		);

		return pNewElement->Index;
	}
	else
	{
		RENDER_SORT_INDEX_ELEMENT* pExistingElement = static_cast<RENDER_SORT_INDEX_ELEMENT*>(pData);
		return pExistingElement->Index;
	}
}

uint32 RENDER_SORT_INDEXER::GetPixelShaderIndex(HANDLE StackPool, UINT64 key)
{
	void* pData = query_hash_node(
		pPixelShaderIndexTable,
		static_cast<int64>(key)
	);

	if (!pData)
	{
		RENDER_SORT_INDEX_ELEMENT* pNewElement = (RENDER_SORT_INDEX_ELEMENT*)stackpool_alloc(
			StackPool,
			sizeof(RENDER_SORT_INDEX_ELEMENT)
		);

		pNewElement->Key = key;
		pNewElement->Index = static_cast<UINT32>(pPixelShaderIndexTable->node_count);
		pNewElement->HashNode.key = static_cast<int64>(key);
		pNewElement->HashNode.data = pNewElement;

		int result =  insert_hash_node(
			pPixelShaderIndexTable,
			static_cast<int64>(key),
			&pNewElement->HashNode
		);

		return pNewElement->Index;
	}
	else
	{
		RENDER_SORT_INDEX_ELEMENT* pExistingElement = static_cast<RENDER_SORT_INDEX_ELEMENT*>(pData);
		return pExistingElement->Index;
	}
}

uint32 RENDER_SORT_INDEXER::GetTextureIndex(HANDLE StackPool, UINT64 key)
{
	void* pData = query_hash_node(
		pTextureIndexTable,
		static_cast<int64>(key)
	);

	if (!pData)
	{
		RENDER_SORT_INDEX_ELEMENT* pNewElement = (RENDER_SORT_INDEX_ELEMENT*)stackpool_alloc(
			StackPool,
			sizeof(RENDER_SORT_INDEX_ELEMENT)
		);

		pNewElement->Key = key;
		pNewElement->Index = static_cast<UINT32>(pTextureIndexTable->node_count);
		pNewElement->HashNode.key = static_cast<int64>(key);
		pNewElement->HashNode.data = pNewElement;

		int result =  insert_hash_node(
			pTextureIndexTable,
			static_cast<int64>(key),
			&pNewElement->HashNode
		);

		return pNewElement->Index;
	}
	else
	{
		RENDER_SORT_INDEX_ELEMENT* pExistingElement = static_cast<RENDER_SORT_INDEX_ELEMENT*>(pData);
		return pExistingElement->Index;
	}
}

uint32 RENDER_SORT_INDEXER::GetStateIndex(
	HANDLE StackPool,
	UINT16 BlendStateIndex,
	UINT16 RasterizeStateIndex,
	UINT16 DepthStencilStateIndex
)
{
	UINT64 key = MakeStateKey(BlendStateIndex, RasterizeStateIndex, DepthStencilStateIndex);

	void* pData = query_hash_node(
		pStateIndexTable,
		static_cast<int64>(key)
	);

	if (!pData)
	{
		RENDER_SORT_INDEX_ELEMENT* pNewElement = (RENDER_SORT_INDEX_ELEMENT*)stackpool_alloc(
			StackPool,
			sizeof(RENDER_SORT_INDEX_ELEMENT)
		);

		pNewElement->Key = key;
		pNewElement->Index = static_cast<UINT32>(pStateIndexTable->node_count);
		pNewElement->HashNode.key = static_cast<int64>(key);
		pNewElement->HashNode.data = pNewElement;

		int result =  insert_hash_node(
			pStateIndexTable,
			static_cast<int64>(key),
			&pNewElement->HashNode
		);

		return pNewElement->Index;
	}
	else
	{
		RENDER_SORT_INDEX_ELEMENT* pExistingElement = static_cast<RENDER_SORT_INDEX_ELEMENT*>(pData);
		return pExistingElement->Index;
	}
}