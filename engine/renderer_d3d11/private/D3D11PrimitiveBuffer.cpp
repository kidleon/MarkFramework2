#include "pch.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11BlobAllocator.h"
#include "D3D11Blob.h"


D3D11PrimitiveBuffer::PRIMITIVE_DESC D3D11PrimitiveBuffer::INVALID_PRIMITIVE_DESC = {};

D3D11PrimitiveBuffer::D3D11PrimitiveBuffer(
	BUFFER_USAGE Usage,
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	size_t VertexBufferSize,
	size_t IndexBufferSize
) noexcept
	: m_Usage(Usage)
	, m_pD3D11VertexBuffer(pVertexBuffer)
	, m_pD3D11IndexBuffer(pIndexBuffer)
	, m_VertexBufferSize(VertexBufferSize)
	, m_IndexBufferSize(IndexBufferSize)
{
	m_ID = static_cast<UINT64>(D3D11_COMMON::GetUID());
	m_LoadStat = LOAD_STAT::LOADED;
}

D3D11PrimitiveBuffer::~D3D11PrimitiveBuffer() noexcept
{
	CHECK_RELEASE(m_pD3D11VertexBuffer);
	CHECK_RELEASE(m_pD3D11IndexBuffer);

	if (m_pVertexBlob)
	{
		D3D11BlobAllocator::Get()->Release(m_pVertexBlob);
		m_pVertexBlob = nullptr;
	}

	if (m_pIndexBlob)
	{
		D3D11BlobAllocator::Get()->Release(m_pIndexBlob);
		m_pIndexBlob = nullptr;
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
	uint32 VertexCount,
	uint32 VertexStride,
	uint32 IndexCount,
	uint32 IndexStride
) noexcept
{
	if (m_NumPrimitives >= MAX_PRIMITIVES)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded maximum number of primitives.");
		return -1;
	}

	if (m_CurrentVertexSize + (VertexCount * VertexStride) > m_VertexBufferSize)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded vertex buffer size.");
		return -1;
	}

	if (m_CurrentIndexSize + (IndexCount * IndexStride) > m_IndexBufferSize)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded index buffer size.");
		return -1;
	}

	m_Primitives[m_NumPrimitives].PrimitiveType = PrimitiveType;
	m_Primitives[m_NumPrimitives].VertexOffset = static_cast<UINT32>(m_CurrentVertexSize);
	m_Primitives[m_NumPrimitives].VertexCount = VertexCount;
	m_Primitives[m_NumPrimitives].VertexStride = VertexStride;
	m_Primitives[m_NumPrimitives].IndexOffset = static_cast<UINT32>(m_CurrentIndexSize);
	m_Primitives[m_NumPrimitives].IndexCount = IndexCount;
	m_Primitives[m_NumPrimitives].IndexStride = IndexStride;

	m_CurrentVertexSize += (VertexCount * VertexStride);
	m_CurrentIndexSize += (IndexCount * IndexStride);
	
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

	if (m_Primitives[PrimitiveIndex].VertexCount * m_Primitives[PrimitiveIndex].VertexStride < VertexSize)
	{
		m_DirtyVertexBuffer = FALSE;
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex: Vertex size exceeds primitive vertex buffer size.");
		return FALSE;
	}

	if (!m_pVertexBlob)
	{
		m_pVertexBlob = D3D11BlobAllocator::Get()->Acquire(m_VertexBufferSize);
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

	if (m_Primitives[PrimitiveIndex].IndexCount * m_Primitives[PrimitiveIndex].IndexStride < IndexSize)
	{
		m_DirtyIndexBuffer = FALSE;
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex: Vertex size exceeds primitive vertex buffer size.");
		return FALSE;
	}

	if (!m_pIndexBlob)
	{
		m_pIndexBlob = D3D11BlobAllocator::Get()->Acquire(m_IndexBufferSize);
		if (!m_pIndexBlob)
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertex: Failed to acquire vertex blob.");
			return FALSE;
		}
	}

	m_pIndexBlob->Update(
		const_cast<void*>(pIndexData),
		IndexSize,
		m_Primitives[PrimitiveIndex].IndexOffset
	);

	m_DirtyIndexBuffer = TRUE;

	return TRUE;
}

size_t D3D11PrimitiveBuffer::GetTotalVertexBufferSize() const noexcept
{
	return m_VertexBufferSize;
}

size_t D3D11PrimitiveBuffer::GetTotalIndexBufferSize() const noexcept
{
	return m_IndexBufferSize;
}

void D3D11PrimitiveBuffer::UploadToGPU_VB(ID3D11DeviceContext* pDeviceContext)
{
	if (!m_DirtyVertexBuffer)
		return;

	m_DirtyVertexBuffer = FALSE;
	if (m_Usage == BUFFER_USAGE::IMMUTABLE)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_VB: Cannot update immutable vertex buffer.");
	}
	else if (m_Usage == BUFFER_USAGE::DEFAULT)
	{
		pDeviceContext->UpdateSubresource(
			m_pD3D11VertexBuffer,
			0,
			nullptr,
			m_pVertexBlob->INL_GetPointer(),
			(UINT32)m_VertexBufferSize,
			0
		);
	}
	else if (m_Usage == BUFFER_USAGE::STAGING)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_VB: Cannot update staging vertex buffer.");
	}
	else if (m_Usage == BUFFER_USAGE::DYNAMIC)
	{
		// 계속 진행
		D3D11_MAPPED_SUBRESOURCE MappedResource = {};

		HRESULT hr = pDeviceContext->Map(
			m_pD3D11VertexBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&MappedResource
		);

		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_VB: Failed to map vertex buffer to GPU.");
			return;
		}

		memcpy(
			MappedResource.pData,
			m_pVertexBlob->INL_GetPointer(),
			m_VertexBufferSize
		);

		pDeviceContext->Unmap(m_pD3D11VertexBuffer, 0);
	}
}

void D3D11PrimitiveBuffer::UploadToGPU_IB(ID3D11DeviceContext* pDeviceContext)
{
	if (!m_DirtyIndexBuffer)
		return;

	m_DirtyIndexBuffer = FALSE;

	if (m_Usage == BUFFER_USAGE::IMMUTABLE)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_IB: Cannot update immutable index buffer.");
	}
	else if (m_Usage == BUFFER_USAGE::DEFAULT)
	{
		pDeviceContext->UpdateSubresource(
			m_pD3D11IndexBuffer,
			0,
			nullptr,
			m_pIndexBlob->INL_GetPointer(),
			(UINT)m_IndexBufferSize,
			0
		);
	}
	else if (m_Usage == BUFFER_USAGE::STAGING)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_IB: Cannot update staging index buffer.");
	}
	else if(m_Usage == BUFFER_USAGE::DYNAMIC)
	{
		// 계속 진행
		D3D11_MAPPED_SUBRESOURCE MappedResource = {};

		HRESULT hr = pDeviceContext->Map(
			m_pD3D11IndexBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&MappedResource
		);

		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_IB: Failed to map index buffer to GPU.");
			return;
		}

		memcpy(
			MappedResource.pData,
			m_pIndexBlob->INL_GetPointer(),
			m_IndexBufferSize
		);

		pDeviceContext->Unmap(m_pD3D11IndexBuffer, 0);
	}
	else
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateToGPU_IB: Unknown buffer usage.");
	}
}

