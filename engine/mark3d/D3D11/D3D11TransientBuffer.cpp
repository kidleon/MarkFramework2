#include "pch.h"
#include "D3D11TransientBuffer.h"
#include "D3D11RenderDevice.h"
#include "D3D11Buffer.h"


D3D11TransientBuffer::D3D11TransientBuffer(D3D11RenderDevice* pRenderDevice)
	: m_pRenderDevice(pRenderDevice)
{
}

D3D11TransientBuffer::~D3D11TransientBuffer() noexcept
{
	Shutdown();
}

BOOL D3D11TransientBuffer::Init(size_t BufferSize)
{
	// Create Vertex Buffers
	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = static_cast<UINT>(BufferSize);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		ID3D11Buffer* pD3D11Buffer = nullptr;
		if (!m_pRenderDevice->CreateBuffer(&desc, &pD3D11Buffer))
		{
			SYS_LOG_E("D3D11TransientBuffer::Init: Failed to create transient buffer");
			return FALSE;
		}

		m_pVBPool[i] = pD3D11Buffer;
	}

	// Create Index Buffers
	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = static_cast<UINT>(BufferSize);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		ID3D11Buffer* pD3D11Buffer = nullptr;
		if (!m_pRenderDevice->CreateBuffer(&desc, &pD3D11Buffer))
		{
			SYS_LOG_E("D3D11TransientBuffer::Init: Failed to create transient buffer");
			return FALSE;
		}

		m_pIBPool[i] = pD3D11Buffer;
	}

	// Create Constant Buffers
	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = static_cast<UINT>(BufferSize);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		ID3D11Buffer* pD3D11Buffer = nullptr;
		if (!m_pRenderDevice->CreateBuffer(&desc, &pD3D11Buffer))
		{
			SYS_LOG_E("D3D11TransientBuffer::Init: Failed to create transient buffer");
			return FALSE;
		}

		m_pCBPool[i] = pD3D11Buffer;
	}

	return TRUE;
}

void D3D11TransientBuffer::Shutdown() noexcept
{
	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		if (m_pVBPool[i])
		{
			m_pVBPool[i]->Release();
			m_pVBPool[i] = nullptr;
		}
	}

	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		if (m_pIBPool[i])
		{
			m_pIBPool[i]->Release();
			m_pIBPool[i] = nullptr;
		}
	}

	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		if (m_pCBPool[i])
		{
			m_pCBPool[i]->Release();
			m_pCBPool[i] = nullptr;
		}
	}
}

void D3D11TransientBuffer::Reset() noexcept
{
	m_CurrOffset_VB = 0u;
	m_CurrOffset_IB = 0u;
	m_CurrOffset_CB = 0u;

	m_FrameIndex_VB = (m_FrameIndex_VB + 1) % MAX_TRANSIENT_BUFFERS;
	m_FrameIndex_IB = (m_FrameIndex_IB + 1) % MAX_TRANSIENT_BUFFERS;
	m_FrameIndex_CB = (m_FrameIndex_CB + 1) % MAX_TRANSIENT_BUFFERS;
}

D3D11Buffer* D3D11TransientBuffer::AllocVB(size_t AllocSize)
{
	constexpr size_t ALIGNMENT = 16u; // 버텍스 버퍼는 16바이트 정렬

	size_t AlignedOffset = (m_CurrOffset_VB + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

	if (AlignedOffset + AllocSize > m_BufferSize)
	{
		SYS_LOG_E("D3D11TransientBuffer::AllocVB: Not enough space in transient vertex buffer");
		return nullptr;
	}

	ID3D11DeviceContext* pD3D11Context = m_pRenderDevice->INL_GetD3D11Context();

	ID3D11Buffer* pCurrBuffer = m_pVBPool[m_FrameIndex_VB];

	D3D11_MAPPED_SUBRESOURCE Mapped;
	HRESULT hr = pD3D11Context->Map(pCurrBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped);
	
	if (FAILED(hr))
	{
		// NO_OVERWRITE 실패 시 DISCARD로 재시도
		hr = pD3D11Context->Map(pCurrBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11TransientBuffer::AllocVB: Failed to map transient vertex buffer");
			return nullptr;
		}

		// DISCARD를 사용했으므로 오프셋 리셋
		AlignedOffset = 0;
		m_CurrOffset_VB = 0;
	}

	void* pCPUAddress = static_cast<char*>(Mapped.pData) + AlignedOffset;
	pD3D11Context->Unmap(pCurrBuffer, 0);

	D3D11Buffer* pBuffer = MARK_POOL_NEW(D3D11Buffer)(
		D3D11_BUFFER_TYPE::VERTEX_BUFFER,
		AllocSize,
		AlignedOffset,
		pCPUAddress,
		pCurrBuffer
	);

	m_CurrOffset_VB = AlignedOffset + AllocSize;

	return pBuffer;
}

D3D11Buffer* D3D11TransientBuffer::AllocIB(size_t AllocSize)
{
	constexpr size_t ALIGNMENT = 4u; // 인덱스 버퍼는 4바이트 정렬

	size_t AlignedOffset = (m_CurrOffset_IB + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
	if (AlignedOffset + AllocSize > m_BufferSize)
	{
		SYS_LOG_E("D3D11TransientBuffer::AllocIB: Not enough space in transient index buffer");
		return nullptr;
	}

	ID3D11DeviceContext* pD3D11Context = m_pRenderDevice->INL_GetD3D11Context();
	ID3D11Buffer* pCurrBuffer = m_pIBPool[m_FrameIndex_IB];

	D3D11_MAPPED_SUBRESOURCE Mapped;
	HRESULT hr = pD3D11Context->Map(pCurrBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped);

	if (FAILED(hr))
	{
		// NO_OVERWRITE 실패 시 DISCARD로 재시도
		hr = pD3D11Context->Map(pCurrBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11TransientBuffer::AllocIB: Failed to map transient index buffer");
			return nullptr;
		}
		// DISCARD를 사용했으므로 오프셋 리셋
		AlignedOffset = 0;
		m_CurrOffset_IB = 0;
	}

	void* pCPUAddress = static_cast<char*>(Mapped.pData) + AlignedOffset;
	pD3D11Context->Unmap(pCurrBuffer, 0);

	D3D11Buffer* pBuffer = MARK_POOL_NEW(D3D11Buffer)(
		D3D11_BUFFER_TYPE::INDEX_BUFFER,
		AllocSize,
		AlignedOffset,
		pCPUAddress,
		pCurrBuffer
	);

	m_CurrOffset_IB = AlignedOffset + AllocSize;

	return pBuffer;
}

D3D11Buffer* D3D11TransientBuffer::AllocCB(size_t AllocSize)
{
	constexpr size_t ALIGNMENT = 16u; // 상수 버퍼는 16바이트 정렬

	size_t AlignedOffset = (m_CurrOffset_CB + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

	if (AlignedOffset + AllocSize > m_BufferSize)
	{
		SYS_LOG_E("D3D11TransientBuffer::AllocCB: Not enough space in transient constant buffer");
		return nullptr;
	}

	ID3D11DeviceContext* pD3D11Context = m_pRenderDevice->INL_GetD3D11Context();
	ID3D11Buffer* pCurrBuffer = m_pCBPool[m_FrameIndex_CB];

	D3D11_MAPPED_SUBRESOURCE Mapped;
	HRESULT hr = pD3D11Context->Map(pCurrBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped);
	
	if (FAILED(hr))
	{
		// NO_OVERWRITE 실패 시 DISCARD로 재시도
		hr = pD3D11Context->Map(pCurrBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11TransientBuffer::AllocCB: Failed to map transient constant buffer");
			return nullptr;
		}
		// DISCARD를 사용했으므로 오프셋 리셋
		AlignedOffset = 0;
		m_CurrOffset_CB = 0;
	}

	void* pCPUAddress = static_cast<char*>(Mapped.pData) + AlignedOffset;

	pD3D11Context->Unmap(pCurrBuffer, 0);

	D3D11Buffer* pBuffer = MARK_POOL_NEW(D3D11Buffer)(
		D3D11_BUFFER_TYPE::CONSTANT_BUFFER,
		AllocSize,
		AlignedOffset,
		pCPUAddress,
		pCurrBuffer
	);

	m_CurrOffset_CB = AlignedOffset + AllocSize;

	return pBuffer;
}