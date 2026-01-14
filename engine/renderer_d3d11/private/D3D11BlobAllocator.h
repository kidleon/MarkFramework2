#pragma once


class D3D11_BLOB;
class D3D11BlobPool;

class D3D11BlobAllocator
{
	enum BLOB_SIZE
	{
		BLOB_16K = 0,
		BLOB_32K,
		BLOB_64K,
		BLOB_128K,
		BLOB_256K,
		BLOB_512K,
		BLOB_1MB,
		BLOB_4MB,
		BLOB_SIZE_COUNT
	};

	static D3D11BlobAllocator* s_pInstance;

public:
	D3D11BlobAllocator();
	~D3D11BlobAllocator();

	BOOL Init();
	void Shutdown();

	D3D11_BLOB* Acquire(size_t BufferSize);
	void Release(D3D11_BLOB* pCB);

	static D3D11BlobAllocator* Get() noexcept { return s_pInstance; }

private:
	D3D11BlobPool* m_pPools[BLOB_SIZE_COUNT] = {};

};


