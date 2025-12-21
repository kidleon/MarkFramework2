#include "pch.h"
#include "D3D11ConstantBufferImpl.h"
#include "D3D11ConstantBufferBlock.h"
#include "D3D11ConstantBufferBlockPool.h"
#include "Log.h"


D3D11ConstantBufferImpl::~D3D11ConstantBufferImpl() noexcept
{
	if (m_pCBufferBlock)
	{
		D3D11ConstantBufferBlockPool::Release(m_pCBufferBlock);
		m_pCBufferBlock = nullptr;
	}

	m_pDataRefPtr = nullptr;
	m_DataRefSize = 0;
}

void D3D11ConstantBufferImpl::OnDestroy()
{
	MARK_POOL_DELETE(this, D3D11ConstantBufferImpl);
}

void D3D11ConstantBufferImpl::UpdateData(const void* pData, size_t DataSize)
{
	if (!pData || (DataSize == 0))
		return;

	if (m_pDataRefPtr)
	{
		// 참조 모드에서 복사 모드로 전환
		m_pDataRefPtr = nullptr;
		m_DataRefSize = 0;
	}

	if (m_pCBufferBlock)
	{
		if (m_pCBufferBlock->BufferSize >= DataSize)
		{
			memcpy(m_pCBufferBlock->pData, pData, DataSize);
			return;
		}

		D3D11ConstantBufferBlockPool::Release(m_pCBufferBlock);
		m_pCBufferBlock = nullptr;
	}

	m_pCBufferBlock = D3D11ConstantBufferBlockPool::Alloc(DataSize);
	if (!m_pCBufferBlock)
	{
		SYS_LOG_E("ConstantBuffer::UpdateData - Failed to allocate constant buffer block of size %zu.", DataSize);
		return;
	}

	memcpy(m_pCBufferBlock->pData, pData, DataSize);
}

void D3D11ConstantBufferImpl::UpdateDataRef(const void* pData, size_t DataSize)
{
	if (!pData || (DataSize == 0))
		return;

	// 참조 모드로 전환
	if (m_pCBufferBlock)
	{
		D3D11ConstantBufferBlockPool::Release(m_pCBufferBlock);
		m_pCBufferBlock = nullptr;
	}

	m_pDataRefPtr = (void*)pData;
	m_DataRefSize = DataSize;
}

