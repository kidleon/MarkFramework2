#pragma once


struct RENDER_SORT_INDEXER
{
	HASH_TABLE* pVertexShaderIndexTable;
	HASH_TABLE* pPixelShaderIndexTable;
	HASH_TABLE* pTextureIndexTable;
	HASH_TABLE* pStateIndexTable;

	RENDER_SORT_INDEXER() noexcept;

	void Init();
	void Destroy();
	void Reset();

	uint32 GetVertexShaderIndex(HANDLE StackPool, UINT64 key);
	uint32 GetPixelShaderIndex(HANDLE StackPool, UINT64 key);
	uint32 GetTextureIndex(HANDLE StackPool, UINT64 key);
	uint32 GetRenderStateIndex(
		HANDLE StackPool,
		UINT64 BlendStateKey,
		UINT64 RasterizeStateKey,
		UINT64 DepthStencilStateKey
	);
};


