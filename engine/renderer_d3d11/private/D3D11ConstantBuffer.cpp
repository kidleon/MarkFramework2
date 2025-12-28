#include "pch.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11RenderDevice.h"


D3D11ConstantBuffer::D3D11ConstantBuffer(
	size_t BufferSize,
	ID3D11Buffer* pD3D11Buffer
)
	: m_BufferSize(BufferSize)
	, m_pD3D11Buffer(pD3D11Buffer)
	, m_LinkNode{}
{
	m_LinkNode.data = this;
}

D3D11ConstantBuffer::~D3D11ConstantBuffer() noexcept
{
	CHECK_RELEASE(m_pD3D11Buffer);
}