#include "pch.h"
#include "PrimitiveBuffer.h"
#include "Engine.h"


namespace mark
{
	PrimitiveBuffer::~PrimitiveBuffer() noexcept
	{
		for (size_t i = 0; i < (size_t)VERTEX_FORMAT_INDEX::MAX; ++i)
		{
			CHECK_RELEASE(m_pVBs[i]);
		}

		CHECK_RELEASE(m_pIB);
	}

	void PrimitiveBuffer::UpdateVertexDataImmediate(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize)
	{

	}

	void PrimitiveBuffer::UpdateVertexData(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize)
	{

	}

	void PrimitiveBuffer::UpdateIndexDataImmediate(INDEX_FORMAT IndexFormat, const void* pData, size_t DataSize)
	{

	}

	void PrimitiveBuffer::UpdateIndexData(INDEX_FORMAT IndexFormat, const void* pData, size_t DataSize)
	{

	}

}
