#include "pch.h"
#include "D3D11PrimitiveBuffer.h"


D3D11PrimitiveBuffer::D3D11PrimitiveBuffer(
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer
) noexcept
	: m_Usage(BUFFER_USAGE::DEFAULT)
	, m_pD3D11VertexBuffer(pVertexBuffer)
	, m_pD3D11IndexBuffer(pIndexBuffer)
{
	m_ID = static_cast<UINT64>(D3D11Common::GetUID());
	m_LoadStat = LOAD_STAT::LOADED;
}

D3D11PrimitiveBuffer::~D3D11PrimitiveBuffer() noexcept
{
	if (m_pVertexData)
	{
		D3D11_SYS_FREE(m_pVertexData);
		m_pVertexData = nullptr;
	}

	if (m_pIndexData)
	{
		D3D11_SYS_FREE(m_pIndexData);
		m_pIndexData = nullptr;
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

UINT64 D3D11PrimitiveBuffer::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11PrimitiveBuffer::GetAssetType() const noexcept
{
	return ASSET_TYPE::PRIMITIVE_BUFFER;
}

LOAD_STAT D3D11PrimitiveBuffer::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

BUFFER_USAGE D3D11PrimitiveBuffer::GetUsage() const noexcept
{
	return m_Usage;
}

void D3D11PrimitiveBuffer::ResetPrimitive() noexcept
{
	m_NumPrimitives = 0;
}

INT32 D3D11PrimitiveBuffer::AddPrimitive(
	PRIMITIVE_TYPE PrimitiveType,
	uint32 VertexOffset,
	uint32 VertexCount,
	uint32 IndexOffset,
	uint32 IndexCount
) noexcept
{
	if (m_NumPrimitives >= MAX_PRIMITIVES)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded maximum number of primitives.");
		return -1;
	}

	m_Primitives[m_NumPrimitives].PrimitiveType = PrimitiveType;
	m_Primitives[m_NumPrimitives].VertexOffset = VertexOffset;
	m_Primitives[m_NumPrimitives].VertexCount = VertexCount;
	m_Primitives[m_NumPrimitives].IndexOffset = IndexOffset;
	m_Primitives[m_NumPrimitives].IndexCount = IndexCount;

	return static_cast<INT32>(m_NumPrimitives++);
}

size_t D3D11PrimitiveBuffer::GetNumPrimitives() const noexcept
{
	return m_NumPrimitives;
}

BOOL D3D11PrimitiveBuffer::UpdateVertex(
	int32 PrimitiveIndex,
	const void* pVertexData,
	size_t VertexSize
) 
{
	if (m_Usage == BUFFER_USAGE::DEFAULT)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex - Cannot update vertex data on DEFAULT usage buffer.");
		return FALSE;
	}

	return TRUE;
}

BOOL D3D11PrimitiveBuffer::UpdateIndex(
	int32 PrimitiveIndex,
	const void* pIndexData,
	size_t IndexSize
) 
{
	return TRUE;
}

