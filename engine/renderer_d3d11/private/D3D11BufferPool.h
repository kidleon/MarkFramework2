#ifndef __D3D11_BUFFER_POOL_H__
#define __D3D11_BUFFER_POOL_H__


class D3D11Buffer;

class D3D11BufferPool
{
	static D3D11BufferPool* s_pInstance;
	static constexpr size_t BU_COUNT = 2; // 버퍼 Usage 방식 (0: DEFAULT/IMMUTABLE, 1: DYNAMIC/STAGING)

public:
	enum BUFFER_SIZE
	{
		B_1K = 0,
		B_2K,
		B_4K,
		B_8K,
		B_16K,
		B_64K,
		B_128K,
		B_256K,
		B_512K,
		B_1M,
		B_2M,

		B_SIZE_COUNT
	};

public:
	static D3D11BufferPool* Get() noexcept { return s_pInstance; }

	D3D11BufferPool(ID3D11Device* pD3D11Device);
	~D3D11BufferPool();

	D3D11Buffer* AcquireVB(size_t BufferSize, BUFFER_USAGE Usage);
	D3D11Buffer* AcquireIB(size_t BufferSize, BUFFER_USAGE Usage);

	void ReleaseVB(D3D11Buffer* pBuffer);
	void ReleaseIB(D3D11Buffer* pBuffer);

private:
	void ExpandVB(size_t BufferSize, size_t NumBuffers, BUFFER_USAGE Usage);
	void ExpandIB(size_t BufferSize, size_t NumBuffers, BUFFER_USAGE Usage);

private:
	ID3D11Device* m_pD3D11Device = nullptr;

	// 버퍼 풀을 위한 링크드 리스트 헤드
	LINKED_LIST m_VBPool[BU_COUNT][B_SIZE_COUNT] = {};
	LINKED_LIST m_IBPool[BU_COUNT][B_SIZE_COUNT] = {};

	size_t m_AllocatedVBCount[BU_COUNT][B_SIZE_COUNT] = {};
	size_t m_AllocatedIBCount[BU_COUNT][B_SIZE_COUNT] = {};
};

#endif // __D3D11_BUFFER_POOL_H__
