#include "pch.h"
#include "D3D11Blob.h"


D3D11_BLOB::D3D11_BLOB(void* pAlignedBuffer, size_t size)
	: m_pBuffer(pAlignedBuffer)
	, m_BufferSize(size)
{
	m_LinkNode.data = this;
	Clear();
}

D3D11_BLOB::~D3D11_BLOB()
{
	if (m_pBuffer)
	{
		D3D11_SYS_FREE_ALIGN(m_pBuffer);
		m_pBuffer = nullptr;
	}
}

long D3D11_BLOB::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11_BLOB::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_DELETE(this, D3D11_BLOB);
	}
	return NewRefCnt;
}

long D3D11_BLOB::RefCnt()
{
	return m_RefCnt;
}

void D3D11_BLOB::Update(void* pBuffer, size_t size)
{
	if(!pBuffer || size == 0)
		return;

	if (size > m_BufferSize)
		return;

	memcpy(m_pBuffer, pBuffer, size);
}

void D3D11_BLOB::Update(void* pBuffer, size_t size, size_t offset)
{
	if (!pBuffer || size == 0)
		return;

	size_t endOffset = offset + size;

	if (endOffset > m_BufferSize)
		return;

	memcpy((char*)m_pBuffer + offset, pBuffer, size);
}

void D3D11_BLOB::Clear() noexcept
{
	if (m_pBuffer && m_BufferSize > 0)
	{
		memset(m_pBuffer, 0, m_BufferSize);
	}
}