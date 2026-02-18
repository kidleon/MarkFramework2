#include "pch.h"
#include "D3D11ConstantBuffer.h"


D3D11ConstantBuffer::D3D11ConstantBuffer(ID3D11Buffer* pBuffer, void* pAlignedBuffer, size_t BufferSize)
	: m_pD3D11Buffer(pBuffer)
	, m_BufferSize(BufferSize)
	, m_pBufferData(pAlignedBuffer)
	, m_pBufferDataRef(nullptr)
	, m_LinkNode{}
{
	m_LinkNode.data = this;
}

D3D11ConstantBuffer::~D3D11ConstantBuffer() noexcept
{
	CHECK_RELEASE(m_pD3D11Buffer);

	if (m_pBufferData)
	{
		D3D11_SYS_FREE_ALIGN(m_pBufferData);
		m_pBufferData = nullptr;
	}
}

long D3D11ConstantBuffer::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11ConstantBuffer::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11ConstantBuffer);
	}
	return NewRefCnt;
}

long D3D11ConstantBuffer::RefCnt()
{
	return m_RefCnt;
}

void D3D11ConstantBuffer::UpdateData(void* pData, size_t DataSize)
{
	if (m_BufferSize < DataSize || !pData || DataSize == 0)
	{
		SYS_LOG_E("D3D11ConstantBuffer::UpdateData: Invalid data or size.");
		return;
	}

	memcpy(m_pBufferData, pData, DataSize);
}

BOOL D3D11ConstantBuffer::UploadToGPU(
	ID3D11DeviceContext* pDeviceContext,
	void* pBufferData,
	size_t BufferSize
)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource = {};
	HRESULT hr = pDeviceContext->Map(
		m_pD3D11Buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource
	);

	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11ConstantBuffer::UploadToGPU: Failed to map constant buffer to GPU.");
		return FALSE;
	}

	memcpy(MappedResource.pData, pBufferData, BufferSize);
	pDeviceContext->Unmap(m_pD3D11Buffer, 0);

	return TRUE;
}