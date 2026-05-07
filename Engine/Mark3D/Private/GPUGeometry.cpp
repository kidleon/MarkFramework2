#include "pch.h"
#include "GPUGeometry.h"
#include "ModelAsset.h"
#include "PrimitiveBuffer.h"


namespace mark
{
	GPUGeometry::~GPUGeometry() noexcept
	{
	}

	void GPUGeometry::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void GPUGeometry::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(GPUGeometry, this);
		}
	}

	bool GPUGeometry::Create(IModelAsset* pModelAsset, GPU_BUFFER_LAYOUT BufferLayout, BOOL HasModelAsset)
	{
		if (!pModelAsset)
		{
			LOG_ERR("Invalid model asset provided to GPUGeometry::Create.");
			return false;
		}

		pModelAsset->AddRef(); // GPUGeometry가 모델 자산을 참조하므로 참조 카운트 증가
		if (HasModelAsset)
		{
			m_pModelAsset = static_cast<ModelAsset*>(pModelAsset);
		}

		m_BufferLayout = BufferLayout;

		uint32_t NumMesh = m_pModelAsset->INL_GetNumMeshes();

		uint32_t TotalVertexCount = m_pModelAsset->INL_GetTotalVertexCount();

		switch (m_BufferLayout)
		{
			case GPU_BUFFER_LAYOUT::MERGED:
				{
					PrimitiveBuffer* pMergedBuffer = CORE_NEW(PrimitiveBuffer);
					m_lstPrimitiveBuffers.push_back(pMergedBuffer);

					for (uint32_t i = 0; i < NumMesh; ++i)
					{
						ModelAsset::Mesh* pMesh = m_pModelAsset->INL_GetMesh(i);
						
					}
				} break;

			case GPU_BUFFER_LAYOUT::SEPARATE:
				{
					for (uint32_t i = 0; i < NumMesh; ++i)
					{
						ModelAsset::Mesh* pMesh = m_pModelAsset->INL_GetMesh(i);
					}

				} break;
		}
		

		pModelAsset->Release();

		return true;
	}


}
