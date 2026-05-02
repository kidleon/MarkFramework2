#include "pch.h"
#include "Model.h"


namespace mark
{
	Model::~Model() noexcept
	{
	}

	void Model::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void Model::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(Model, this);
		}
	}
}
