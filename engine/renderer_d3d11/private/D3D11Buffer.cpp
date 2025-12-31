#include "pch.h"
#include "D3D11Buffer.h"


D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	BUFFER_USAGE Usage,
	size_t BufferSize,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferStrategy(D3D11_BUFFER_STRATEGY::INDEPENDENT)
	, m_BufferType(BufferType)
	, m_Usage(Usage)
	, m_BufferSize(BufferSize)
	, m_pD3D11Buffer(pBuffer)
{
}

D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	D3D11BufferPool* pBufferPool,
	int PoolIndex,
	uint32 BufferSize,
	uint32 Offset,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferStrategy(D3D11_BUFFER_STRATEGY::POOL)
	, m_BufferType(BufferType)
	, m_Usage(BUFFER_USAGE::DEFAULT)
	, m_BufferSize(BufferSize)
	, m_Offset(Offset)
	, m_pD3D11Buffer(pBuffer)
	, m_pBufferPool(pBufferPool)
	, m_PoolIndex(PoolIndex)
{
}

D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	size_t BufferSize,
	size_t Offset,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferStrategy(D3D11_BUFFER_STRATEGY::TRANSIENT)
	, m_BufferType(BufferType)
	, m_Usage(BUFFER_USAGE::DYNAMIC)
	, m_BufferSize(BufferSize)
	, m_Offset(Offset)
	, m_pD3D11Buffer(pBuffer)
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