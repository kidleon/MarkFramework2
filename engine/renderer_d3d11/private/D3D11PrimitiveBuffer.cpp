#include "pch.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11BlobAllocator.h"
#include "D3D11Blob.h"


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
	if (m_NumPrimitives < PrimitiveIndex)
	{
		m_DirtyVertexBuffer = FALSE;
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex: Invalid primitive index.");
		return FALSE;
	}

	if (m_pVertexBlob)
	{
		if (m_pVertexBlob->INL_GetSize() < VertexSize)
		{
			D3D11BlobAllocator::Get()->Release(m_pVertexBlob);
			m_pVertexBlob = nullptr;
		}
	}

	if (!m_pVertexBlob)
	{
		m_pVertexBlob = D3D11BlobAllocator::Get()->Acquire(VertexSize);
		if (!m_pVertexBlob)
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex: Failed to acquire vertex blob.");
			return FALSE;
		}
	}

	m_pVertexBlob->Update(
		const_cast<void*>(pVertexData), 
		VertexSize, 
		m_Primitives[PrimitiveIndex].VertexOffset
	);

	m_DirtyVertexBuffer = TRUE;

	return TRUE;
}

BOOL D3D11PrimitiveBuffer::UpdateIndex(
	int32 PrimitiveIndex,
	const void* pIndexData,
	size_t IndexSize
) 
{
	if (m_NumPrimitives < PrimitiveIndex)
	{
		m_DirtyIndexBuffer = FALSE;
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateIndex: Invalid primitive index.");
		return FALSE;
	}

	if (m_pIndexBlob)
	{
		if (m_pIndexBlob->INL_GetSize() < IndexSize)
		{
			D3D11BlobAllocator::Get()->Release(m_pIndexBlob);
			m_pIndexBlob = nullptr;
		}
	}

	if (!m_pIndexBlob)
	{
		m_pIndexBlob = D3D11BlobAllocator::Get()->Acquire(IndexSize);
		if (!m_pIndexBlob)
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateIndex: Failed to acquire index blob.");
			return FALSE;
		}
	}

	m_pIndexBlob->Update(const_cast<void*>(pIndexData), IndexSize, );
	m_DirtyIndexBuffer = TRUE;

	return TRUE;
}

