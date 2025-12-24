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
	if (m_pD3D11Buffer)
	{
		m_pD3D11Buffer->Release();
		m_pD3D11Buffer = nullptr;
	}
}