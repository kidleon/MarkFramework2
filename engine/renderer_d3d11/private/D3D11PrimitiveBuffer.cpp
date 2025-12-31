#include "pch.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11Buffer.h"


D3D11PrimitiveBuffer::~D3D11PrimitiveBuffer() noexcept
{
	if (m_pVertexBuffer)
	{
		D3D11_POOL_DELETE(m_pVertexBuffer, D3D11Buffer);
		m_pVertexBuffer = nullptr;
	}

	if (m_pIndexBuffer)
	{
		D3D11_POOL_DELETE(m_pIndexBuffer, D3D11Buffer);
		m_pIndexBuffer = nullptr;
	}
}

long D3D11PrimitiveBuffer::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11PrimitiveBuffer::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11PrimitiveBuffer);
	}
	return NewRefCnt;
}

long D3D11PrimitiveBuffer::RefCnt()
{
	return m_RefCnt;
}

UINT32 D3D11PrimitiveBuffer::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11PrimitiveBuffer::GetAssetType() const noexcept
{
	return ASSET_TYPE::SHADER_PROGRAM;
}

LOAD_STAT D3D11PrimitiveBuffer::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

void D3D11PrimitiveBuffer::AddPrimitive(
	PRIMITIVE_TYPE PrimitiveType,
	uint32 VertexOffset,
	uint32 VertexCount,
	uint32 IndexOffset,
	uint32 IndexCount
) noexcept
{
	if (m_NumPrimitives >= MAX_PRIMITIVE)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive: Exceeded maximum number of primitives (%d)", MAX_PRIMITIVE);
		return;
	}

	PRIMITIVE& NewPrimitive = m_Primitives[m_NumPrimitives];
	NewPrimitive.PrimitiveType = PrimitiveType;
	NewPrimitive.VertexOffset = VertexOffset;
	NewPrimitive.VertexCount = VertexCount;
	NewPrimitive.IndexOffset = IndexOffset;
	NewPrimitive.IndexCount = IndexCount;
	NewPrimitive.pVertexData = nullptr;
	NewPrimitive.pIndexData = nullptr;
	NewPrimitive.VertexDataSize = 0;
	NewPrimitive.IndexDataSize = 0;
	m_NumPrimitives++;

}

uint32 D3D11PrimitiveBuffer::GetNumPrimitives() const noexcept
{
	return m_NumPrimitives;
}

void D3D11PrimitiveBuffer::UpdateVertex(
	int32 PrimitiveIndex,
	const void* pVertexData,
	size_t VertexSize
)
{
	if (PrimitiveIndex < 0 || static_cast<size_t>(PrimitiveIndex) >= m_NumPrimitives)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex: Invalid PrimitiveIndex (%d)", PrimitiveIndex);
		return;
	}

	m_Primitives[PrimitiveIndex].pVertexData = (VOID*)pVertexData;
	m_Primitives[PrimitiveIndex].VertexDataSize = static_cast<UINT32>(VertexSize);
	m_Dirty = TRUE;
}

void D3D11PrimitiveBuffer::UpdateIndex(
	int32 PrimitiveIndex,
	const void* pIndexData,
	size_t IndexSize
)
{
	if (PrimitiveIndex < 0 || static_cast<size_t>(PrimitiveIndex) >= m_NumPrimitives)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateIndex: Invalid PrimitiveIndex (%d)", PrimitiveIndex);
		return;
	}
	m_Primitives[PrimitiveIndex].pIndexData = (VOID*)pIndexData;
	m_Primitives[PrimitiveIndex].IndexDataSize = static_cast<UINT32>(IndexSize);
	m_Dirty = TRUE;
}