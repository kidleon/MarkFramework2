#include "pch.h"
#include "D3D11PrimitiveBuffer.h"
#include "D3D11Buffer.h"


D3D11PrimitiveBuffer::~D3D11PrimitiveBuffer() noexcept
{
	if (m_pVertexBuffer)
	{
		D3D11_POOL_DELETE(m_pVertexBuffer, D3D11Buffer);
		m_pVertexBuffer = nullptr;
	}

	if (m_pIndexBuffer)
	{
		D3D11_POOL_DELETE(m_pIndexBuffer, D3D11Buffer);
		m_pIndexBuffer = nullptr;
	}
}

long D3D11PrimitiveBuffer::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long D3D11PrimitiveBuffer::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		D3D11_POOL_DELETE(this, D3D11PrimitiveBuffer);
	}
	return NewRefCnt;
}

long D3D11PrimitiveBuffer::RefCnt()
{
	return m_RefCnt;
}

UINT32 D3D11PrimitiveBuffer::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE D3D11PrimitiveBuffer::GetAssetType() const noexcept
{
	return ASSET_TYPE::SHADER_PROGRAM;
}

LOAD_STAT D3D11PrimitiveBuffer::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

