#include "pch.h"
#include "D3D11GPUBuffer.h"
#include "D3D11RenderDevice.h"


namespace mark
{
	D3D11GPUBuffer::D3D11GPUBuffer(ID3D11Buffer* pD3D11Buffer) noexcept
		: m_pD3D11Buffer(pD3D11Buffer)
	{
		m_pD3D11Buffer->GetDesc(&m_BufferDesc);
	}

	D3D11GPUBuffer::~D3D11GPUBuffer() noexcept
	{
		if (m_pD3D11Buffer)
		{
			D3D11RenderDevice::Get().ReleaseBuffer(m_pD3D11Buffer);
			m_pD3D11Buffer = nullptr;
		}
	}

	void D3D11GPUBuffer::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void D3D11GPUBuffer::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(D3D11GPUBuffer, this);
		}
	}

	BUFFER_TYPE D3D11GPUBuffer::GetBufferType() const
	{
		switch (m_BufferDesc.BindFlags)
		{
			case D3D11_BIND_VERTEX_BUFFER:
				return BUFFER_TYPE::VERTEX_BUFFER;

			case D3D11_BIND_INDEX_BUFFER:
				return BUFFER_TYPE::INDEX_BUFFER;

			case D3D11_BIND_CONSTANT_BUFFER:
				return BUFFER_TYPE::CONSTANT_BUFFER;
		}

		return BUFFER_TYPE::EMAX;
	}

	size_t D3D11GPUBuffer::GetBufferSize() const
	{
		return m_BufferDesc.ByteWidth;
	}

	void* D3D11GPUBuffer::GetNativePointer() const
	{
		return static_cast<void*>(m_pD3D11Buffer);
	}
}
