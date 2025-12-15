#include "pch.h"
#include "ConstantBuffer.h"
#include "ConstantBufferBlock.h"
#include "ConstantBufferBlockPool.h"
#include "Log.h"


ConstantBuffer::~ConstantBuffer() noexcept
{
	if (m_pCBufferBlock)
	{
		ConstantBufferBlockPool::Release(m_pCBufferBlock);
		m_pCBufferBlock = nullptr;
	}

	m_pDataRefPtr = nullptr;
	m_DataRefSize = 0;
}

void ConstantBuffer::OnDestroy()
{
	MARK_POOL_DELETE(this, ConstantBuffer);
}

void ConstantBuffer::UpdateData(const void* pData, size_t DataSize)
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

		ConstantBufferBlockPool::Release(m_pCBufferBlock);
		m_pCBufferBlock = nullptr;
	}

	m_pCBufferBlock = ConstantBufferBlockPool::Alloc(DataSize);
	if (!m_pCBufferBlock)
	{
		SYS_LOG_E("ConstantBuffer::UpdateData - Failed to allocate constant buffer block of size %zu.", DataSize);
		return;
	}

	memcpy(m_pCBufferBlock->pData, pData, DataSize);
}

void ConstantBuffer::UpdateDataRef(const void* pData, size_t DataSize)
{
	if (!pData || (DataSize == 0))
		return;

	// 참조 모드로 전환
	if (m_pCBufferBlock)
	{
		ConstantBufferBlockPool::Release(m_pCBufferBlock);
		m_pCBufferBlock = nullptr;
	}

	m_pDataRefPtr = (void*)pData;
	m_DataRefSize = DataSize;
}

