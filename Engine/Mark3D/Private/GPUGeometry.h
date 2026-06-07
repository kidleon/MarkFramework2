#pragma once
#include "name_hash.h"
#include "ModelAsset.h"


namespace mark
{
	class PrimitiveBuffer;

	class GPUGeometry final : public IGPUGeometry
	{
	public:
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

		virtual bool IsLoaded() const noexcept { return m_IsLoaded; }

		void AddSubset(
			uint32_t VertexCount,
			uint32_t IndexCount
		) noexcept;

		inline BOOL INL_IsLoaded() const noexcept { return m_IsLoaded; }
		inline void INL_SetLoaded(BOOL Loaded) { m_IsLoaded = Loaded; }
		inline GPU_BUFFER_LAYOUT INL_GetBufferLayout() const noexcept { return m_BufferLayout; }
		inline void INL_SetBufferLayout(GPU_BUFFER_LAYOUT BufferLayout) { m_BufferLayout = BufferLayout; }
		inline void INL_AddPrimitiveBuffer(const PrimitiveBuffer* pBuffer) { m_lstPrimitiveBuffers.push_back(const_cast<PrimitiveBuffer*>(pBuffer)); }
		inline void INL_SetModelAsset(const unknown_ptr<ModelAsset>& pModelAsset) { m_pModelAsset = pModelAsset; }
		inline MeshDesc& INL_AddAndGetMeshDesc()
		{
			MeshDesc Desc = {};
			m_lstMeshDescs.push_back(Desc);
			return m_lstMeshDescs.back();
		}

	private:
		virtual ~GPUGeometry();

	private:
		std::atomic<int64_t> m_RefCount{ 1 };
		BOOL m_IsLoaded = FALSE;
		GPU_BUFFER_LAYOUT m_BufferLayout = GPU_BUFFER_LAYOUT::MERGED;
		unknown_ptr<ModelAsset> m_pModelAsset;
		sys_vector<MeshDesc> m_lstMeshDescs;
		sys_vector<PrimitiveBuffer*> m_lstPrimitiveBuffers;
		
	};
}
