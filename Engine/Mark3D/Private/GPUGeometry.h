#pragma once
#include "name_hash.h"
#include "ModelAsset.h"


namespace mark
{
	class PrimitiveBuffer;

	class GPUGeometry final : public IGPUGeometry
	{
		constexpr static uint32_t MAX_SUBSET_COUNT = 8;
		struct MeshDesc
		{
			name_hash Name;
			uint32_t VertexStart;
			uint32_t IndexStart;
			uint32_t VertexCount;
			uint32_t IndexCount;

			uint32_t NumSubset;
			uint32_t Subset_MatID[MAX_SUBSET_COUNT];
			uint32_t Subset_StartIndex[MAX_SUBSET_COUNT];
			uint32_t Subset_IndexCount[MAX_SUBSET_COUNT];
		};

	public:
		virtual void AddRef();
		virtual void Release();

		virtual bool Create(IModelAsset* pModelAsset, GPU_BUFFER_LAYOUT BufferLayout, BOOL HasModelAsset);

	private:
		virtual ~GPUGeometry();

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		GPU_BUFFER_LAYOUT m_BufferLayout = GPU_BUFFER_LAYOUT::MERGED;
		unknown_ptr<ModelAsset> m_pModelAsset;

		sys_vector<PrimitiveBuffer*> m_lstPrimitiveBuffers;
		
	};
}
