#include "pch.h"
#include "PrimitiveBuffer.h"
#include "Engine.h"
#include "RenderSystem.h"


namespace mark
{
	PrimitiveBuffer::~PrimitiveBuffer() noexcept
	{
		Cleanup();
	}

	bool PrimitiveBuffer::Create(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount)
	{
		if (!VertexCount || !IndexCount || !VertexFormats)
			return false;

		RenderSystem* pRenderSys = Engine::Get().INL_GetRenderSystem();

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::POSITION) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::POSITION * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::POSITION] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::POSITION])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for POSITION.");
				Cleanup();

				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::NORMAL) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::NORMAL * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::NORMAL] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::NORMAL])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for NORMAL.");
				Cleanup();

				return false;
			}
		}
		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::COLOR) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::COLOR * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::COLOR] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::COLOR])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for COLOR.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TANGENT) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::TANGENT * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::TANGENT] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TANGENT])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for TANGENT.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::BONE) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::BONE * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::BONE] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::BONE])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for BONE.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::WEIGHT) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::WEIGHT * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::WEIGHT] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::WEIGHT])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for WEIGHT.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD0) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::TEXCOORD0 * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD0] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD0])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for TEXCOORD0.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::TEXCOORD1) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::TEXCOORD1 * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD1] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD1])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for TEXCOORD1.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::CUSTOM0) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::CUSTOM0 * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::CUSTOM0] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::CUSTOM0])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for CUSTOM0.");
				Cleanup();
				return false;
			}
		}

		if ((VertexFormats & (uint32_t)VERTEX_FORMAT::CUSTOM1) != 0)
		{
			GPUBufferCreateDesc CreateDesc = {};
			CreateDesc.Type = BUFFER_TYPE::VERTEX_BUFFER;
			CreateDesc.Usage = BUFFER_USAGE::DEFAULT;
			CreateDesc.BufferSize = (uint32_t)VERTEX_FORMAT_STRIDE::CUSTOM1 * VertexCount;
			m_pVBs[(int)VERTEX_FORMAT_INDEX::CUSTOM1] = pRenderSys->CreateGPUBuffer(CreateDesc);
			if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::CUSTOM1])
			{
				SYS_LOG_ERR("Failed to create vertex buffer for CUSTOM1.");
				Cleanup();
				return false;
			}
		}

		GPUBufferCreateDesc CreateDesc = {};
		CreateDesc.Type = BUFFER_TYPE::INDEX_BUFFER;
		CreateDesc.Usage = BUFFER_USAGE::DEFAULT;

		uint32_t IndexSize = sizeof(uint16_t);
		if(IndexFormat == INDEX_FORMAT::AUTO)
		{
			IndexSize = (VertexCount > 65500) ? sizeof(uint32_t) : sizeof(uint16_t);
			m_IndexFormat = (IndexSize == sizeof(uint32_t)) ? INDEX_FORMAT::UINT32 : INDEX_FORMAT::UINT16;
		}
		else
		{
			m_IndexFormat = IndexFormat;
			IndexSize = (IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t);
		}

		CreateDesc.BufferSize = IndexCount * IndexSize;
		m_pIB = pRenderSys->CreateGPUBuffer(CreateDesc);
		if (!m_pIB)
		{
			SYS_LOG_ERR("Failed to create index buffer.");
			Cleanup();
			return false;
		}

		return true;
	}

	bool PrimitiveBuffer::UpdateVertexDataImmediate(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize)
	{
		if (VERTEX_FORMAT::NONE == VertexFormat || !DataSize || !pData)
			return false;

		VERTEX_FORMAT_INDEX FormatIndex = ToVertexFormatIndex(VertexFormat);

		if (FormatIndex == VERTEX_FORMAT_INDEX::EMAX)
		{
			SYS_LOG_ERR_F("Invalid vertex format specified for update: {}", (uint32_t)VertexFormat);
			return false;
		}

		if (!m_pVBs[(int)FormatIndex])
		{
			SYS_LOG_ERR_F("Vertex buffer for format {} is not initialized.", (uint32_t)VertexFormat);
			return false;
		}

		if (!m_pVBs[(int)FormatIndex]->UpdateBuffer(pData, DataSize))
		{
			SYS_LOG_ERR_F("Failed to update vertex buffer for format {}.", (uint32_t)VertexFormat);
			return false;
		}

		return true;
	}

	bool PrimitiveBuffer::UpdateVertexData(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize)
	{
		return true;
	}

	bool PrimitiveBuffer::UpdateIndexDataImmediate(const void* pData, size_t DataSize)
	{
		 if (!DataSize || !pData)
			 return false;

		 if (!m_pIB)
		 {
			 SYS_LOG_ERR("Index buffer is not initialized.");
			 return false;
		 }

		 if (!m_pIB->UpdateBuffer(pData, DataSize))
		 {
			 SYS_LOG_ERR("Failed to update index buffer.");
			 return false;
		 }

		return true;
	}

	bool PrimitiveBuffer::UpdateIndexData(const void* pData, size_t DataSize)
	{
		return true;
	}

	void PrimitiveBuffer::Cleanup()
	{
		for (int i = 0; i < (int)VERTEX_FORMAT_INDEX::EMAX; ++i)
			CHECK_RELEASE(m_pVBs[i]);
		CHECK_RELEASE(m_pIB);

		m_VertexFormat = 0;
		m_VertexCount = 0;
		m_IndexCount = 0;
		m_IndexFormat = INDEX_FORMAT::UINT16;
	}
}
