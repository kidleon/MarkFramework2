#include "pch.h"
#include "GPUGeometry.h"
#include "ModelAsset.h"
#include "PrimitiveBuffer.h"


namespace mark
{
	GPUGeometry::~GPUGeometry() noexcept
	{
		for (auto pBuffer : m_lstPrimitiveBuffers)
		{
			CORE_DELETE(PrimitiveBuffer, pBuffer);
		}
		m_lstPrimitiveBuffers.clear();
		m_pModelAsset.reset();
		m_lstMeshDescs.clear();
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

}
