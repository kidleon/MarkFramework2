#include "pch.h"
#include "D3D11ConstantBufferAllocator.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11ConstantBufferPool.h"


D3D11ConstantBufferAllocator* D3D11ConstantBufferAllocator::s_pInstance = nullptr;

static size_t CB_COUNT_TABLE[] =
{
	64,		// CB_256B
	64,		// CB_512B
	32,		// CB_1KB
	32,		// CB_4KB
	16,		// CB_16KB
	8		// CB_64KB
};

static size_t CB_SIZE_TABLE[] =
{
	256,		// CB_256B
	512,		// CB_512B
	1024,		// CB_1KB
	4096,		// CB_4KB
	16384,		// CB_16KB
	65536		// CB_64KB
};

D3D11ConstantBufferAllocator::D3D11ConstantBufferAllocator()
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11ConstantBufferAllocator::~D3D11ConstantBufferAllocator()
{
	Shutdown();

	if (s_pInstance == this)
		s_pInstance = nullptr;
}

BOOL D3D11ConstantBufferAllocator::Init(D3D11RenderDevice* pRenderDevice)
{
	m_pRenderDevice = pRenderDevice;

	for (size_t i = 0; i < CB_SIZE_COUNT; ++i)
	{
		m_pPools[i] = D3D11_NEW(D3D11ConstantBufferPool)();
		m_pPools[i]->Init(m_pRenderDevice, CB_SIZE_TABLE[i], CB_COUNT_TABLE[i], FALSE);
	}

	for (size_t f = 0; f < MAX_FRAME_POOL; ++f)
	{
		for (size_t i = 0; i < CB_SIZE_COUNT; ++i)
		{
			m_pTempPools[f][i] = D3D11_NEW(D3D11ConstantBufferPool)();
			m_pTempPools[f][i]->Init(m_pRenderDevice, CB_SIZE_TABLE[i], CB_COUNT_TABLE[i], TRUE);
		}
	}
	
	return TRUE;
}

void D3D11ConstantBufferAllocator::Shutdown()
{
	for (size_t i = 0; i < CB_SIZE_COUNT; ++i)
	{
		if (m_pPools[i])
		{
			D3D11_DELETE(m_pPools[i], D3D11ConstantBufferPool);
			m_pPools[i] = nullptr;
		}
	}

	for (size_t f = 0; f < MAX_FRAME_POOL; ++f)
	{
		for (size_t i = 0; i < CB_SIZE_COUNT; ++i)
		{
			if (m_pTempPools[f][i])
			{
				D3D11_DELETE(m_pTempPools[f][i], D3D11ConstantBufferPool);
				m_pTempPools[f][i] = nullptr;
			}
		}
	}
	
	m_pRenderDevice = nullptr;
}

static inline int32 get_cb_size_index(size_t size)
{
	constexpr size_t SIZE_COUNT = sizeof(CB_SIZE_TABLE) / sizeof(size_t);

	for (int i = 0; i < SIZE_COUNT; ++i)
	{
		if (size <= CB_SIZE_TABLE[i])
			return i;
	}

	return -1;
}

D3D11ConstantBuffer* D3D11ConstantBufferAllocator::Acquire(size_t BufferSize)
{
	int32 cb_size_idx = get_cb_size_index(BufferSize);
	if (-1 == cb_size_idx)
		return nullptr;

	return m_pPools[cb_size_idx]->Acquire();
}

void D3D11ConstantBufferAllocator::Release(D3D11ConstantBuffer* pCB)
{
	if (!pCB) return;

	size_t buffer_size = pCB->INL_GetBufferSize();

	int32 cb_size_idx = get_cb_size_index(buffer_size);
	if (-1 == cb_size_idx)
		return;

	m_pPools[cb_size_idx]->Release(pCB);
}

D3D11ConstantBuffer* D3D11ConstantBufferAllocator::AcquireTemp(size_t BufferSize)
{
	int32 cb_size_idx = get_cb_size_index(BufferSize);
	if (-1 == cb_size_idx)
		return nullptr;

	return m_pTempPools[m_CurrentFrameIndex][cb_size_idx]->Acquire();
}

void D3D11ConstantBufferAllocator::ResetTemp()
{
	for (size_t i = 0; i < CB_SIZE_COUNT; ++i)
	{
		m_pTempPools[m_CurrentFrameIndex][i]->Reset();
	}

	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAME_POOL;
}