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

BOOL D3D11TransientBuffer::Init(D3D11_BUFFER_TYPE BufferType, size_t BufferSize)
{
	UINT32 BindFlags = 0;

	m_BufferType = BufferType;

	switch (BufferType)
	{
		case D3D11_BUFFER_TYPE::VERTEX_BUFFER:
			m_CurrOffset = 0u;
			m_FrameIndex = 0u;
			BindFlags = D3D11_BIND_VERTEX_BUFFER;
			break;

		case D3D11_BUFFER_TYPE::INDEX_BUFFER:
			m_CurrOffset = 0u;
			m_FrameIndex = 0u;
			BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;

		case D3D11_BUFFER_TYPE::CONSTANT_BUFFER:
			m_CurrOffset = 0u;
			m_FrameIndex = 0u;
			BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			break;
	}

	// Create Vertex Buffers
	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = static_cast<UINT>(BufferSize);
		desc.BindFlags = BindFlags;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		ID3D11Buffer* pD3D11Buffer = nullptr;
		if (!m_pRenderDevice->CreateBuffer(&desc, &pD3D11Buffer))
		{
			SYS_LOG_E("D3D11TransientBuffer::Init: Failed to create transient buffer");
			return FALSE;
		}

		m_pFrameBuffer[i] = pD3D11Buffer;
	}

	return TRUE;
}

void D3D11TransientBuffer::Shutdown() noexcept
{
	for (size_t i = 0; i < MAX_TRANSIENT_BUFFERS; ++i)
	{
		if (m_pFrameBuffer[i])
		{
			m_pFrameBuffer[i]->Release();
			m_pFrameBuffer[i] = nullptr;
		}
	}
}

void D3D11TransientBuffer::Reset() noexcept
{
	m_CurrOffset = 0u;
	m_FrameIndex = (m_FrameIndex + 1) % MAX_TRANSIENT_BUFFERS;
}

D3D11Buffer* D3D11TransientBuffer::Alloc(size_t AllocSize)
{
	size_t AlignedOffset = 0;
	size_t ALIGNMENT = 0;

	switch (m_BufferType)
	{
		case D3D11_BUFFER_TYPE::VERTEX_BUFFER:
			ALIGNMENT = 16; // 버텍스 버퍼는 16바이트 정렬
			AlignedOffset = (m_CurrOffset + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
			break;
			
		case D3D11_BUFFER_TYPE::INDEX_BUFFER:
			ALIGNMENT = 4; // 인덱스 버퍼는 4바이트 정렬
			AlignedOffset = (m_CurrOffset + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
			break;

		case D3D11_BUFFER_TYPE::CONSTANT_BUFFER:
			ALIGNMENT = 16; // 상수 버퍼는 16바이트 정렬
			AlignedOffset = (m_CurrOffset + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
			break;
	}

	if (AlignedOffset + AllocSize > m_BufferSize)
	{
		SYS_LOG_E("D3D11TransientBuffer::AllocVB: Not enough space in transient vertex buffer");
		return nullptr;
	}

	ID3D11DeviceContext* pD3D11Context = m_pRenderDevice->INL_GetD3D11Context();

	ID3D11Buffer* pCurrBuffer = m_pFrameBuffer[m_FrameIndex];

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
		m_CurrOffset = 0;
	}

	void* pCPUAddress = static_cast<char*>(Mapped.pData) + AlignedOffset;
	pD3D11Context->Unmap(pCurrBuffer, 0);

	D3D11Buffer* pBuffer = D3D11_POOL_NEW(D3D11Buffer)(
		D3D11_BUFFER_TYPE::VERTEX_BUFFER,
		AllocSize,
		AlignedOffset,
		pCPUAddress,
		pCurrBuffer
	);

	m_CurrOffset = AlignedOffset + AllocSize;

	return pBuffer;
}