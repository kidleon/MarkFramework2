#include "pch.h"
#include "D3D11Buffer.h"


namespace mark
{
	D3D11Buffer::D3D11Buffer(
		ID3D11Buffer* pD3D11Buffer,
		size_t BufferSize,
		BUFFER_TYPE BufferType,
		BUFFER_USAGE BufferUsage,
		BOOL Pooled
	) noexcept
		: m_pD3D11Buffer(pD3D11Buffer)
		, m_BufferSize(BufferSize)
		, m_BufferType(BufferType)
		, m_BufferUsage(BufferUsage)
		, m_Pooled(Pooled)
	{
	}

	D3D11Buffer::~D3D11Buffer() noexcept
	{
		CHECK_RELEASE(m_pD3D11Buffer);
	}
}
