#include "pch.h"
#include "D3D11GPUBuffer.h"


namespace mark
{
	D3D11GPUBuffer::D3D11GPUBuffer(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pD3D11Buffer) noexcept
		: m_pDeviceContext(pDeviceContext)
		, m_pD3D11Buffer(pD3D11Buffer)
	{
		m_pD3D11Buffer->GetDesc(&m_BufferDesc);
	}

	D3D11GPUBuffer::~D3D11GPUBuffer() noexcept
	{
		CHECK_RELEASE(m_pD3D11Buffer);
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

	BUFFER_TYPE D3D11GPUBuffer::GetBufferType() const noexcept
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

	BUFFER_USAGE D3D11GPUBuffer::GetBufferUsage() const noexcept
	{
		switch (m_BufferDesc.Usage)
		{
			case D3D11_USAGE_DYNAMIC:
				return BUFFER_USAGE::DYNAMIC;
			case D3D11_USAGE_DEFAULT:
				return BUFFER_USAGE::DEFAULT;
		}
		return BUFFER_USAGE::EMAX;
	}

	size_t D3D11GPUBuffer::GetBufferSize() const noexcept
	{
		return m_BufferDesc.ByteWidth;
	}

	void* D3D11GPUBuffer::GetNativePointer() const noexcept
	{
		return static_cast<void*>(m_pD3D11Buffer);
	}

	bool D3D11GPUBuffer::UpdateBuffer(const void* pData, size_t DataSize, size_t* pWrittenOffset)
	{
		if (!m_pD3D11Buffer || !m_pDeviceContext || !pData || 0 == DataSize)
			return false;

		if (m_BufferDesc.ByteWidth < DataSize)
		{
			SYS_LOG_ERR_F("Buffer update failed: Buffer size {} is smaller than data size {}",
				m_BufferDesc.ByteWidth, DataSize);
			return false;
		}

		ID3D11DeviceContext* pDeviceContext = m_pDeviceContext;

		if (m_BufferDesc.Usage == D3D11_USAGE_DYNAMIC)
		{
			D3D11_MAP   MapType = D3D11_MAP_WRITE_NO_OVERWRITE;
			size_t      WriteOffset = m_DynamicOffset;

			if (WriteOffset + DataSize > static_cast<size_t>(m_BufferDesc.ByteWidth))
			{
				MapType = D3D11_MAP_WRITE_DISCARD;
				WriteOffset = 0;
			}

			D3D11_MAPPED_SUBRESOURCE MappedResource = {};
			HRESULT hr = pDeviceContext->Map(
				m_pD3D11Buffer,
				0,
				MapType,
				0,
				&MappedResource
			);

			if (FAILED(hr))
			{
				SYS_LOG_ERR_F("Map failed: HRESULT = 0x{:08X}", static_cast<uint32_t>(hr));
				return false;
			}

			uint8_t* pDst = static_cast<uint8_t*>(MappedResource.pData) + WriteOffset;
			memcpy(pDst, pData, DataSize);

			pDeviceContext->Unmap(m_pD3D11Buffer, 0);

			if (m_BufferDesc.BindFlags & D3D11_BIND_CONSTANT_BUFFER)
			{
				constexpr size_t BUFFER_ALIGNMENT = 256;
				m_DynamicOffset = WriteOffset + ((DataSize + BUFFER_ALIGNMENT - 1) & ~(BUFFER_ALIGNMENT - 1));
			}
			else
			{
				m_DynamicOffset = WriteOffset + DataSize;
			}

			// 호출자가 실제 쓰인 오프셋을 알아야 할 경우 전달
			// (IASetVertexBuffers의 offset 인자, VSSetConstantBuffers1의 FirstConstant 등)
			if (pWrittenOffset)
				*pWrittenOffset = WriteOffset;

			return true;
		}
		else if (m_BufferDesc.Usage == D3D11_USAGE_DEFAULT)
		{
			// UpdateSubresource는 void 반환 — 실패는 D3D11 Debug Layer / InfoQueue를 통해서만 관찰 가능.
			// 따라서 호출 전 검증을 충실히 한 후 진행하고, DEFAULT는 항상 buffer 시작에 기록한다.
			pDeviceContext->UpdateSubresource(m_pD3D11Buffer, 0, nullptr, pData, 0, 0);

			if (pWrittenOffset)
				*pWrittenOffset = 0;

			return true;
		}

		// IMMUTABLE / STAGING 등 — UpdateBuffer로는 업데이트할 수 없는 Usage.
		SYS_LOG_ERR_F("Buffer update failed: unsupported D3D11_USAGE = {}", (int)m_BufferDesc.Usage);
		return false;
	}
}
