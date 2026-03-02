#include "pch.h"
#include "D3D11Buffer.h"


D3D11Buffer::D3D11Buffer(
	ID3D11Buffer* pD3D11Buffer,
	UINT32 BufferSize,
	BUFFER_USAGE Usage
)
	: m_pD3D11Buffer(pD3D11Buffer)
	, m_BufferSize(BufferSize)
	, m_Usage(Usage)
{
	m_PoolLinkNode.data = this;
}

D3D11Buffer::~D3D11Buffer() noexcept
{
	CHECK_RELEASE(m_pD3D11Buffer);
}