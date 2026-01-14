#include "pch.h"
#include "D3D11BlobAllocator.h"
#include "D3D11Blob.h"
#include "D3D11BlobPool.h"


D3D11BlobAllocator* D3D11BlobAllocator::s_pInstance = nullptr;

static constexpr size_t BLOB_COUNT_TABLE[] =
{
	32,		// BLOB_16K
	32,		// BLOB_32K
	16,		// BLOB_64K
	16,		// BLOB_128K
	8,		// BLOB_256K
	8,		// BLOB_512K
	4,		// BLOB_1MB
	2		// BLOB_4MB
};

static constexpr size_t BLOB_SIZE_TABLE[] =
{
	16 * 1024,		// BLOB_16K,
	32 * 1024,		// BLOB_32K,
	64 * 1024,		// BLOB_64K,
	128 * 1024,		// BLOB_128K,
	256 * 1024,		// BLOB_256K,
	512 * 1024,		// BLOB_512K,
	1024 * 1024,	// BLOB_1MB,
	4096 * 1024		// BLOB_4MB
};

D3D11BlobAllocator::D3D11BlobAllocator()
{
	if (!s_pInstance)
		s_pInstance = this;
}

D3D11BlobAllocator::~D3D11BlobAllocator()
{
	Shutdown();

	if (s_pInstance == this)
		s_pInstance = nullptr;
}

BOOL D3D11BlobAllocator::Init()
{
	for (size_t i = 0; i < BLOB_SIZE_COUNT; ++i)
	{
		m_pPools[i] = D3D11_NEW(D3D11BlobPool)();
		m_pPools[i]->Init(BLOB_SIZE_TABLE[i], BLOB_COUNT_TABLE[i]);
	}

	return TRUE;
}

void D3D11BlobAllocator::Shutdown()
{
	for (size_t i = 0; i < BLOB_SIZE_COUNT; ++i)
	{
		if (m_pPools[i])
		{
			D3D11_DELETE(m_pPools[i], D3D11BlobPool);
			m_pPools[i] = nullptr;
		}
	}
}

static inline int32 get_blob_size_index(size_t size)
{
	constexpr size_t SIZE_COUNT = sizeof(BLOB_SIZE_TABLE) / sizeof(size_t);

	for (int i = 0; i < SIZE_COUNT; ++i)
	{
		if (size <= BLOB_SIZE_TABLE[i])
			return i;
	}

	return -1;
}

D3D11_BLOB* D3D11BlobAllocator::Acquire(size_t BufferSize)
{
	// If the requested size exceeds the largest pool size, allocate a new blob directly.
	if (BufferSize > BLOB_SIZE_TABLE[BLOB_SIZE_COUNT - 1])
	{
		D3D11_BLOB* pBlob = D3D11_NEW(D3D11_BLOB)(
			D3D11_SYS_ALLOC(BufferSize),
			BufferSize
		);

		return pBlob;
	}

	int32 blob_size_idx = get_blob_size_index(BufferSize);
	if (-1 == blob_size_idx)
		return nullptr;

	return m_pPools[blob_size_idx]->Acquire();
}

void D3D11BlobAllocator::Release(D3D11_BLOB* pBlob)
{
	if (!pBlob) return;

	size_t buffer_size = pBlob->INL_GetSize();

	if (buffer_size > BLOB_SIZE_TABLE[BLOB_SIZE_COUNT - 1])
	{
		// Directly allocated blob, just release it.
		pBlob->Release();
		return;
	}

	int32 blob_size_idx = get_blob_size_index(buffer_size);
	if (-1 == blob_size_idx)
		return;

	m_pPools[blob_size_idx]->Release(pBlob);
}
