#include "pch.h"
#include "D3D11Buffer.h"


D3D11Buffer::D3D11Buffer(
	D3D11_BUFFER_TYPE BufferType,
	size_t BufferSize,
	size_t Offset,
	void* pCPUAddress,
	ID3D11Buffer* pBuffer
) noexcept
	: m_BufferType(BufferType)
	, m_BufferSize(BufferSize)
	, m_Offset(Offset)
	, m_pCPUAddress(pCPUAddress)
	, m_pD3D11Buffer(pBuffer)
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