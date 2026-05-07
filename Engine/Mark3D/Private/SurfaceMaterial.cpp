#include "pch.h"
#include "SurfaceMaterial.h"
/*
#include "SurfaceMaterialFactory.h"
#include "SurfaceMaterialBlock.h"


namespace mark
{
	SurfaceMaterial::~SurfaceMaterial() noexcept
	{
		for (uint32_t i = 0; i < m_NumPasses; ++i)
		{
			if (m_pBlocks[i])
			{
				SurfaceMaterialFactory::ReleaseBlock(m_pBlocks[i]);
				m_pBlocks[i] = nullptr;
			}
		}
	}

	void SurfaceMaterial::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void SurfaceMaterial::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			SurfaceMaterialFactory::Release(this);
		}
	}

	// ISurfaceMaterial 인터페이스 구현
	int32_t SurfaceMaterial::AddPass(const char* szPassName)
	{
		assert(m_NumPasses < MAX_PASS && "Exceeded maximum number of passes.");

		SurfaceMaterialBlock* pBlock = SurfaceMaterialFactory::NewBlock();
		if (!pBlock)
		{
			SYS_LOG_ERR("Failed to create SurfaceMaterialBlock for new pass.");
			return -1;
		}

		m_pBlocks[m_NumPasses++] = pBlock;

		return static_cast<int32_t>(m_NumPasses - 1);
	}

	uint32_t SurfaceMaterial::GetNumPasses() const noexcept
	{
		return m_NumPasses;
	}

	void SurfaceMaterial::SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader)
	{
		assert(Pass >= 0 && Pass < static_cast<int32_t>(m_NumPasses) && "Invalid pass index in SetVertexShader.");
		m_pBlocks[Pass]->pVertexShader = pVertexShader;
	}

	void SurfaceMaterial::SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader)
	{
		assert(Pass >= 0 && Pass < static_cast<int32_t>(m_NumPasses) && "Invalid pass index in SetPixelShader.");
		m_pBlocks[Pass]->pPixelShader = pPixelShader;
	}
}
*/
