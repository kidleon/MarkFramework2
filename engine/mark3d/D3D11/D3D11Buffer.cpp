#include "pch.h"
#include "D3D11Buffer.h"


D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	size_t BufferSize,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferStrategy(D3D11_BUFFER_STRATEGY::INDEPENDENT)
	, m_BufferType(BufferType)
	, m_BufferSize(BufferSize)
	, m_Offset(0)
	, m_pCPUAddress(nullptr)
	, m_pD3D11Buffer(pBuffer)
	, m_BufferFormat(0)
	, m_PoolIndex(0)
{
}

D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	size_t BufferSize,
	uint32 Format,
	uint32 PoolIndex,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferStrategy(D3D11_BUFFER_STRATEGY::POOL)
	, m_BufferType(BufferType)
	, m_BufferSize(BufferSize)
	, m_Offset(0)
	, m_pCPUAddress(nullptr)
	, m_pD3D11Buffer(pBuffer)
	, m_BufferFormat(Format)
	, m_PoolIndex(PoolIndex)
{
}

D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	size_t BufferSize,
	size_t Offset,
	void* pCPUAddress,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferStrategy(D3D11_BUFFER_STRATEGY::TRANSIENT)
	, m_BufferType(BufferType)
	, m_BufferSize(BufferSize)
	, m_Offset(Offset)
	, m_pCPUAddress(pCPUAddress)
	, m_pD3D11Buffer(pBuffer)
	, m_BufferFormat(0)
	, m_PoolIndex(0)
{
}

D3D11Buffer::~D3D11Buffer() noexcept
{
	switch(m_BufferStrategy)
	{
		case D3D11_BUFFER_STRATEGY::INDEPENDENT:
		{
			if (m_pD3D11Buffer)
			{
				m_pD3D11Buffer->Release();
				m_pD3D11Buffer = nullptr;
			}
		} break;

		case D3D11_BUFFER_STRATEGY::POOL:
			// 독립 버퍼와 풀 버퍼는 여기서 해제
			break;

		case D3D11_BUFFER_STRATEGY::TRANSIENT:
			// 트랜지언트 버퍼는 D3D11TransientBuffer에서 관리하므로 여기서 해제하지 않음
			return;
	}

	
}