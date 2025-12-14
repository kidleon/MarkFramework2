#ifndef __CONSTANT_BUFFER_POOL_H__
#define __CONSTANT_BUFFER_POOL_H__


class ConstantBuffer;

class ConstantBufferPool
{
	enum class CBUFFER_INDEX : size_t
	{
		CB_64B = 0,
		CB_128B,
		CB_256B,
		CB_512B,
		CB_1K,
		CB_2K,
		CB_4K,
		CB_8K,
		CB_16K,
		CB_32K,
		CB_64K,

		EMAX
	};

	enum class CBUFFER_SIZE : size_t
	{
		CB_SIZE_64B = 64,
		CB_SIZE_128B = 128,
		CB_SIZE_256B = 256,
		CB_SIZE_512B = 512,
		CB_SIZE_1K = 1024,
		CB_SIZE_2K = 2048,
		CB_SIZE_4K = 4096,
		CB_SIZE_8K = 8192,
		CB_SIZE_16K = 16384,
		CB_SIZE_32K = 32768,
		CB_SIZE_64K = 65536
	};

	enum class CBUFFER_CAPA : size_t
	{
		CB_CAPA_64B = 32,
		CB_SIZE_128B = 32,
		CB_SIZE_256B = 32,
		CB_SIZE_512B = 16,
		CB_SIZE_1K = 16,
		CB_SIZE_2K = 16,
		CB_SIZE_4K = 16,
		CB_SIZE_8K = 16,
		CB_SIZE_16K = 8,
		CB_SIZE_32K = 8,
		CB_SIZE_64K = 8
	};

	struct CBUFFER_POOL
	{
		void* pData;
		size_t BufferSize;
		LINK_NODE LinkNode;
	};

public:
	ConstantBufferPool() = default;
	~ConstantBufferPool();

	void Init();
	void* Alloc(size_t BufferSize);
	void Release(size_t BufferSize, void* pCBufferData);
	void Clear();

private:
	void AllocPool(CBUFFER_INDEX Index);

private:
	LINKED_LIST m_CBufferPool[(size_t)CBUFFER_INDEX::EMAX] = {};
};


#endif // __CONSTANT_BUFFER_POOL_H__
