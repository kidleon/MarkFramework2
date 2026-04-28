#include "pch.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11Buffer.h"
#include "D3D11BufferPool.h"


namespace mark
{
	/*
	D3D11PrimitiveBuffer::~D3D11PrimitiveBuffer()
	{
		for (uint32_t i = 0; i < MAX_BIND_VB; ++i)
		{
			if (m_pBindVBs[i])
			{
			}
		}
	}

	void D3D11PrimitiveBuffer::Reset()
	{
		m_NumPrimitives = 0;
		m_CurrentVertexCount = 0;
		m_CurrentIndexCount = 0;
		for (UINT32 i = 0; i < MAX_PRIMITIVES; ++i)
		{
			m_Primitives[i] = {};
		}
	}

	int32_t D3D11PrimitiveBuffer::AddPrimitive(PRIMITIVE_TYPE PrimitiveType, uint32_t VertexCount, uint32_t IndexCount)
	{
		if (m_NumPrimitives >= MAX_PRIMITIVES)
			return -1; // 최대 프리미티브 수 초과

		if (m_CurrentVertexCount + VertexCount > m_MaxVertexCount ||
			m_CurrentIndexCount + IndexCount > m_MaxIndexCount)
		{
			return -1; // 버퍼 용량 초과
		}

		PRIMITIVE_DESC& Desc = m_Primitives[m_NumPrimitives];
		Desc.PrimitiveType = PrimitiveType;
		Desc.VertexStart = m_CurrentVertexCount;
		Desc.VertexCount = VertexCount;
		Desc.IndexStart = m_CurrentIndexCount;
		Desc.IndexCount = IndexCount;

		m_CurrentVertexCount += VertexCount;
		m_CurrentIndexCount += IndexCount;

		return m_NumPrimitives++;
	}
	*/
}
