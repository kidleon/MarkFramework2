#pragma once
#include "THashMap.h"
using namespace mark;


struct RENDER_SORT_INDEXER
{
	THashMap<uintptr_t, uint32, TA_POOL, 128> VertexShaderIndexer; // 버텍스 셰이더 인덱서
	THashMap<uintptr_t, uint32, TA_POOL, 128> PixelShaderIndexer; // 픽셀 셰이더 인덱서
	THashMap<uintptr_t, uint32, TA_POOL, 256> MaterialIndexer; // 머티리얼 인덱서
	THashMap<uint32, uint32, TA_POOL, 16> RasterizerStateIndexer; // 래스터라이저 상태 인덱서
	THashMap<uint32, uint32, TA_POOL, 16> BlendStateIndexer; // 블렌드 상태 인덱서
	THashMap<uint32, uint32, TA_POOL, 16> DepthStencilStateIndexer; // 뎁스 스텐실 상태 인덱서

	void Reset();

	inline uint32 GetNextVertexShaderIndex(uintptr_t VSKey)
	{
		auto it = VertexShaderIndexer.find(VSKey);
		if (VertexShaderIndexer.end() != it)
			return it->pair.value;

		uint32 NewIndex = static_cast<uint32>(VertexShaderIndexer.size());
		NewIndex %= SORT_VERTEX_SHADER_BIT;

		VertexShaderIndexer.insert(VSKey, NewIndex);

		return NewIndex;
	}

	inline uint32 GetNextPixelShaderIndex(uintptr_t PSKey)
	{
		auto it = PixelShaderIndexer.find(PSKey);
		if (PixelShaderIndexer.end() != it)
			return it->pair.value;

		uint32 NewIndex = static_cast<uint32>(PixelShaderIndexer.size());
		NewIndex %= SORT_PIXEL_SHADER_BIT;

		PixelShaderIndexer.insert(PSKey, NewIndex);

		return NewIndex;
	}

	inline uint32 GetNextMaterialIndex(uintptr_t MaterialKey)
	{
		auto it = MaterialIndexer.find(MaterialKey);
		if (MaterialIndexer.end() != it)
			return it->pair.value;

		uint32 NewIndex = static_cast<uint32>(MaterialIndexer.size());
		NewIndex %= SORT_TEXTURE_ID_BIT;

		MaterialIndexer.insert(MaterialKey, NewIndex);

		return NewIndex;
		
	}

	inline uint32 GetNextRasterizerStateIndex(uint32 RasterizerStateKey)
	{
		auto it = RasterizerStateIndexer.find(RasterizerStateKey);
		if (RasterizerStateIndexer.end() != it)
			return it->pair.value;

		uint32 NewIndex = static_cast<uint32>(RasterizerStateIndexer.size());
		NewIndex %= 4;
		RasterizerStateIndexer.insert(RasterizerStateKey, NewIndex);

		return NewIndex;
	}

	inline uint32 GetNextBlendStateIndex(uint32 BlendStateKey)
	{
		auto it = BlendStateIndexer.find(BlendStateKey);
		if (BlendStateIndexer.end() != it)
			return it->pair.value;

		uint32 NewIndex = static_cast<uint32>(BlendStateIndexer.size());
		NewIndex %= 4;

		BlendStateIndexer.insert(BlendStateKey, NewIndex);

		return NewIndex;
	}

	inline uint32 GetNextDepthStencilStateIndex(uint32 DepthStencilStateKey)
	{
		auto it = DepthStencilStateIndexer.find(DepthStencilStateKey);
		if (DepthStencilStateIndexer.end() != it)
			return it->pair.value;

		uint32 NewIndex = static_cast<uint32>(DepthStencilStateIndexer.size());
		NewIndex %= 4;

		DepthStencilStateIndexer.insert(DepthStencilStateKey, NewIndex);

		return NewIndex;
	}
};