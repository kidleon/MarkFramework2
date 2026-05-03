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

	bool D3D11GPUBuffer::UpdateBuffer(const void* pData, size_t DataSize)
	{
		if (!m_pD3D11Buffer)
			return false;

		if (m_BufferDesc.ByteWidth < DataSize)
		{
			SYS_LOG_ERR_F("Buffer update failed: Buffer size {} is smaller than data size {}",
				m_BufferDesc.ByteWidth, DataSize);

			return false;
		}

		ID3D11DeviceContext* pDeviceContext = D3D11RenderDevice::Get().INL_GetD3D11Context();

		if (m_BufferDesc.Usage == D3D11_USAGE_DYNAMIC)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = pDeviceContext->Map(m_pD3D11Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(hr))
				return false;

			memcpy(static_cast<uint8_t*>(mappedResource.pData), pData, DataSize);

			pDeviceContext->Unmap(m_pD3D11Buffer, 0);

			return true;
		}
		else if (m_BufferDesc.Usage == D3D11_USAGE_DEFAULT)
		{
			pDeviceContext->UpdateSubresource(m_pD3D11Buffer, 0, nullptr, pData, 0, 0);
		}

		return true;
	}

	void* D3D11GPUBuffer::Lock()
	{
		if (!m_pD3D11Buffer)
			return nullptr;

		ID3D11DeviceContext* pDeviceContext = D3D11RenderDevice::Get().INL_GetD3D11Context();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext->Map(m_pD3D11Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (FAILED(hr))
			return nullptr;

		return mappedResource.pData;
	}

	void D3D11GPUBuffer::Unlock()
	{
		if (!m_pD3D11Buffer)
			return;

		ID3D11DeviceContext* pDeviceContext = D3D11RenderDevice::Get().INL_GetD3D11Context();
		pDeviceContext->Unmap(m_pD3D11Buffer, 0);
	}
}
