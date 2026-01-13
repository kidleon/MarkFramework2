#pragma once


class D3D11_BLOB;

class D3D11BlobPool
{
public:
	D3D11BlobPool() = default;
	~D3D11BlobPool();

	BOOL Init(
		size_t BufferSize,
		size_t InitialCount
	);

	void Shutdown();

	D3D11_BLOB* Acquire();
	void Release(D3D11_BLOB* pCB);

private:
	void AllocBlob(
		size_t BufferSize,
		size_t Count
	);

private:
	LINKED_LIST m_FreeList = {};
	LINKED_LIST m_UsedList = {};

	size_t m_BufferSize = 0;
	size_t m_InitialCount = 0;
	
};
