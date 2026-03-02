#include "pch.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11BlobAllocator.h"
#include "D3D11Blob.h"
#include "D3D11Buffer.h"
#include "D3D11BufferPool.h"


D3D11PrimitiveBuffer::PRIMITIVE_DESC D3D11PrimitiveBuffer::INVALID_PRIMITIVE_DESC = {};


D3D11PrimitiveBuffer::~D3D11PrimitiveBuffer() noexcept
{
	for(int i = 0; i < (int)VERTEX_FORMAT_INDEX::MAX; ++i)
	{
		if (m_pVBs[i])
		{
			D3D11BufferPool::Get()->ReleaseVB(m_pVBs[i]);
			m_pVBs[i] = nullptr;
		}
	}

	if (m_pIB)
	{
		D3D11BufferPool::Get()->ReleaseIB(m_pIB);
		m_pIB = nullptr;
	}

	for (int i = 0; i < (int)VERTEX_FORMAT_INDEX::MAX; ++i)
	{
		if (m_pVBlobs)
		{
			D3D11BlobAllocator::Get()->Release(m_pVBlobs[i]);
			m_pVBlobs[i] = nullptr;
		}
	}

	if (m_pIBlob)
	{
		D3D11BlobAllocator::Get()->Release(m_pIBlob);
		m_pIBlob = nullptr;
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

void D3D11PrimitiveBuffer::ResetPrimitive() noexcept
{
	m_NumPrimitives = 0;
}

INT32 D3D11PrimitiveBuffer::AddPrimitive(
	PRIMITIVE_TYPE PrimitiveType,
	uint32 VertexCount,
	uint32 IndexCount
) noexcept
{
	if (m_NumPrimitives >= MAX_PRIMITIVES)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded maximum number of primitives.");
		return -1;
	}

	if (m_CurrentVertexCount + VertexCount > m_MaxVertexCount)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded maximum vertex count.");
		return -1;
	}

	if (m_CurrentIndexCount + IndexCount > m_MaxIndexCount)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::AddPrimitive - Exceeded maximum index count.");
		return -1;
	}

	m_Primitives[m_NumPrimitives].PrimitiveType = PrimitiveType;
	m_Primitives[m_NumPrimitives].VertexStart = m_CurrentVertexCount;
	m_Primitives[m_NumPrimitives].VertexCount = VertexCount;
	m_Primitives[m_NumPrimitives].IndexStart = m_CurrentVertexCount;
	m_Primitives[m_NumPrimitives].IndexCount = IndexCount;

	m_CurrentVertexCount += VertexCount;
	m_CurrentIndexCount += IndexCount;

	return static_cast<INT32>(m_NumPrimitives++);
}


size_t D3D11PrimitiveBuffer::GetNumPrimitives() const noexcept
{
	return m_NumPrimitives;
}

BOOL D3D11PrimitiveBuffer::UpdatePosition(
	int32 PrimitiveIndex,
	const FLOAT3* pPositions,
	UINT32 PositionCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::POSITION,
		pPositions,
		PositionCount,
		sizeof(FLOAT3)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateNormal(
	int32 PrimitiveIndex,
	const FLOAT3* pNormals,
	UINT32 NormalCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::NORMAL,
		pNormals,
		NormalCount,
		sizeof(FLOAT3)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateColor(
	int32 PrimitiveIndex,
	const FLOAT4* pColors,
	UINT32 ColorCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::COLOR,
		pColors,
		ColorCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord0(
	int32 PrimitiveIndex,
	const FLOAT2* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT2)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTangent(
	int32 PrimitiveIndex,
	const FLOAT3* pTangents,
	UINT32 TangentCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TANGENT,
		pTangents,
		TangentCount,
		sizeof(FLOAT3)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateBinormal(
	int32 PrimitiveIndex,
	const FLOAT3* pBinormals,
	UINT32 BinormalCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::BINORMAL,
		pBinormals,
		BinormalCount,
		sizeof(FLOAT3)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateBlendIndices(
	int32 PrimitiveIndex,
	const UINT4* pBlendIndices,
	UINT32 BlendIndexCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::BONE,
		pBlendIndices,
		BlendIndexCount,
		sizeof(UINT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateBlendWeights(
	int32 PrimitiveIndex,
	const FLOAT4* pBlendWeights,
	UINT32 BlendWeightCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::WEIGHT,
		pBlendWeights,
		BlendWeightCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord1(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD1,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord2(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD2,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord3(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD3,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord4(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD4,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord5(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD5,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord6(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD6,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord7(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD7,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateTexCoord8(
	int32 PrimitiveIndex,
	const FLOAT4* pTexCoords,
	UINT32 TexCoordCount
)
{
	return UpdateVertexBuffer(
		PrimitiveIndex,
		(int)VERTEX_FORMAT_INDEX::TEXCOORD8,
		pTexCoords,
		TexCoordCount,
		sizeof(FLOAT4)
	);
}

BOOL D3D11PrimitiveBuffer::UpdateIndex(
	int32 PrimitiveIndex,
	const uint32* pIndices,
	UINT32 IndexCount
)
{
	m_DirtyIB = FALSE;
	if (m_NumPrimitives < PrimitiveIndex)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateIndex: Invalid primitive index.");
		return FALSE;
	}

	if (m_Primitives[PrimitiveIndex].IndexCount + IndexCount > m_MaxIndexCount)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateIndex: Exceeded maximum index count for primitive.");
		return FALSE;
	}

	if (!m_pIBlob)
	{
		m_pIBlob = D3D11BlobAllocator::Get()->Acquire(m_MaxIndexCount * sizeof(uint32));
		if (!m_pIBlob)
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateIndex: Failed to acquire index blob.");
			return FALSE;
		}
	}

	m_pIBlob->Update(
		pIndices,
		IndexCount * sizeof(uint32),
		m_Primitives[PrimitiveIndex].IndexStart * sizeof(uint32)
	);

	m_DirtyIB = TRUE;
	m_DirtyBuffer = TRUE;

	return TRUE;
}

UINT32 D3D11PrimitiveBuffer::GetMaxVertexCount() const noexcept
{
	return m_MaxVertexCount;
}

UINT32 D3D11PrimitiveBuffer::GetMaxIndexCount() const noexcept
{
	return m_MaxIndexCount;
}

void D3D11PrimitiveBuffer::UploadToGPU(ID3D11DeviceContext* pDeviceContext)
{
	if (!m_DirtyBuffer)
		return;

	m_DirtyBuffer = FALSE;

	for (int i = 0; i < (int)VERTEX_FORMAT_INDEX::MAX; ++i)
	{
		if (!m_DirtyVBs[i])
			continue;

		if (!m_pVBs[i])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UploadToGPU: No vertex buffer for vertex format index %d.", i);
			continue;
		}

		m_DirtyVBs[i] = FALSE;

		void* pVertexData =	m_pVBlobs[i]->INL_GetPointer();
		size_t VertexSize = m_pVBlobs[i]->INL_GetSize();

		BUFFER_USAGE Usage = m_pVBs[i]->INL_GetUsage();

		switch (Usage)
		{
			case BUFFER_USAGE::IMMUTABLE:
				SYS_LOG_E("D3D11PrimitiveBuffer::UploadToGPU: Cannot update immutable vertex buffer for vertex format index %d.", i);
				return;

			case BUFFER_USAGE::DEFAULT:
			{
				pDeviceContext->UpdateSubresource(
					m_pVBs[i]->INL_GetD3D11Buffer(),
					0,
					nullptr,
					pVertexData,
					(UINT)VertexSize,
					0
				);
			} break;

			case BUFFER_USAGE::STAGING:
				SYS_LOG_E("D3D11PrimitiveBuffer::UploadToGPU: Cannot update staging vertex buffer for vertex format index %d.", i);
				return;

			case BUFFER_USAGE::DYNAMIC:
			{
				D3D11_MAPPED_SUBRESOURCE MappedResource = {};
				HRESULT hr = pDeviceContext->Map(
					m_pVBs[i]->INL_GetD3D11Buffer(),
					0,
					D3D11_MAP_WRITE_DISCARD,
					0,
					&MappedResource
				);

				if (FAILED(hr))
				{
					SYS_LOG_E("D3D11PrimitiveBuffer::UploadToGPU: Failed to map vertex buffer to GPU for vertex format index %d.", i);
					break;
				}

				memcpy(
					MappedResource.pData,
					pVertexData,
					VertexSize
				);

				pDeviceContext->Unmap(m_pVBs[i]->INL_GetD3D11Buffer(), 0);

			} break;
		}
	}

	if (m_DirtyIB)
	{
		m_DirtyIB = FALSE;
		
		void* pVertexData = m_pIBlob->INL_GetPointer();
		size_t VertexSize = m_pIBlob->INL_GetSize();

		BUFFER_USAGE Usage = m_pIB->INL_GetUsage();

		switch (Usage)
		{
			case BUFFER_USAGE::IMMUTABLE:
				return;

			case BUFFER_USAGE::DEFAULT:
			{
				pDeviceContext->UpdateSubresource(
					m_pIB->INL_GetD3D11Buffer(),
					0,
					nullptr,
					pVertexData,
					(UINT)VertexSize,
					0
				);
			} break;

			case BUFFER_USAGE::STAGING:
				return;

			case BUFFER_USAGE::DYNAMIC:
			{
				D3D11_MAPPED_SUBRESOURCE MappedResource = {};
				HRESULT hr = pDeviceContext->Map(
					m_pIB->INL_GetD3D11Buffer(),
					0,
					D3D11_MAP_WRITE_DISCARD,
					0,
					&MappedResource
				);

				if (FAILED(hr))
				{
					SYS_LOG_E("D3D11PrimitiveBuffer::UploadToGPU: Failed to map vertex buffer to GPU for vertex format index %d.", i);
					break;
				}

				memcpy(
					MappedResource.pData,
					pVertexData,
					VertexSize
				);

				pDeviceContext->Unmap(m_pIB->INL_GetD3D11Buffer(), 0);

			} break;
		}
	}

	for(int32 i = 0; i < (int)VERTEX_FORMAT_INDEX::MAX; ++i)
	{
		if (m_pVBlobs[i])
		{
			D3D11BlobAllocator::Get()->Release(m_pVBlobs[i]);
			m_pVBlobs[i] = nullptr;
		}
	}

	if (m_pIBlob)
	{
		D3D11BlobAllocator::Get()->Release(m_pIBlob);
		m_pIBlob = nullptr;
	}
}

BOOL D3D11PrimitiveBuffer::Create(
	UINT32 VertexFormat,
	size_t MaxVertexCount,
	size_t MaxIndexCount,
	BUFFER_USAGE UsageVB,
	BUFFER_USAGE UsageIB
) noexcept
{
	m_CurrentVertexCount = 0;
	m_CurrentIndexCount = 0;
	m_MaxVertexCount = (UINT32)MaxVertexCount;
	m_MaxIndexCount = (UINT32)MaxIndexCount;
	m_VertexFormat = VertexFormat;
	m_NumPrimitives = 0;

	int BindVBIndex = 0;
	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::POSITION)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::POSITION] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT3), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::POSITION])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for POSITION.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::POSITION]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT3);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::NORMAL)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::NORMAL] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT3), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::NORMAL])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for NORMAL.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::NORMAL]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT3);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT2), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT2);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::COLOR)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::COLOR] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::COLOR])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for COLOR.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::COLOR]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TANGENT)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TANGENT] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT3), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TANGENT])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TANGENT.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TANGENT]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT3);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::BINORMAL)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::BINORMAL] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT3), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::BINORMAL])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for BINORMAL.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::BINORMAL]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT3);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::BONE)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::BONE] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(UINT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::BONE])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for BONE.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::BONE]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(UINT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::WEIGHT)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::WEIGHT] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::WEIGHT])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for WEIGHT.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::WEIGHT]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD1)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD1] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD1])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD1.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD1]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD2)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD2] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD2])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD2.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD2]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD3)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD3] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD3])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD3.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD3]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD4)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD4] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD4])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD4.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD4]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD5)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD5] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD5])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD5.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD5]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD6)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD6] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD6])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD6.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD6]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD7)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD7] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD7])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD7.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD7]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	if (m_VertexFormat & (UINT32)VERTEX_FORMAT::TEXCOORD8)
	{
		m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD8] = D3D11BufferPool::Get()->AcquireVB(MaxVertexCount * sizeof(FLOAT4), UsageVB);
		if (!m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD8])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire vertex buffer for TEXCOORD8.");
			return FALSE;
		}

		m_pBindVBs[BindVBIndex] = m_pVBs[(int)VERTEX_FORMAT_INDEX::TEXCOORD8]->INL_GetD3D11Buffer();
		m_BindVB_Strides[BindVBIndex] = sizeof(FLOAT4);
		++BindVBIndex;
	}

	m_pIB = D3D11BufferPool::Get()->AcquireIB(MaxIndexCount * sizeof(UINT32), UsageIB);
	if (!m_pIB)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::CreateVertexBuffer: Failed to acquire index buffer.");
		return FALSE;
	}

	return TRUE;
}


BOOL D3D11PrimitiveBuffer::UpdateVertexBuffer(
	int32 PrimitiveIndex,
	int VertexFormatIndex,
	const void* pVertexData,
	UINT32 VertexCount,
	UINT32 VertexStride
)
{
	m_DirtyVBs[VertexFormatIndex] = FALSE;

	if (m_NumPrimitives < PrimitiveIndex)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertexBuffer: Invalid primitive index.");
		return FALSE;
	}

	if (m_Primitives[PrimitiveIndex].VertexCount < VertexCount)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertexBuffer: Vertex data size exceeds primitive vertex buffer size.");
		return FALSE;
	}

	if (!m_pVBlobs[VertexFormatIndex])
	{
		m_pVBlobs[VertexFormatIndex] = D3D11BlobAllocator::Get()->Acquire(m_MaxVertexCount * VertexStride);
		if (!m_pVBlobs[VertexFormatIndex])
		{
			SYS_LOG_E("D3D11PrimitiveBuffer::UpdateVertexBuffer: Failed to acquire vertex blob.");
			return FALSE;
		}
	}

	m_pVBlobs[VertexFormatIndex]->Update(
		pVertexData,
		VertexCount * VertexStride,
		m_Primitives[PrimitiveIndex].VertexStart * VertexStride
	);

	m_DirtyVBs[VertexFormatIndex] = TRUE;
	m_DirtyBuffer = TRUE;

	return TRUE;
}

void D3D11PrimitiveBuffer::ComputeBindVBs(int PrimitiveIndex) noexcept
{
	if (m_NumPrimitives < PrimitiveIndex)
	{
		SYS_LOG_E("D3D11PrimitiveBuffer::ComputeBindVBs: Invalid primitive index.");
		return;
	}

	for (UINT32 i = 0; i < m_BindVB_Count; ++i)
		m_BindVB_Offsets[i] = m_BindVB_Strides[i] * m_Primitives[PrimitiveIndex].VertexStart;
}