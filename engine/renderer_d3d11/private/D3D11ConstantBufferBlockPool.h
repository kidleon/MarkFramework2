#ifndef __D3D11_CONSTANT_BUFFER_POOL_H__
#define __D3D11_CONSTANT_BUFFER_POOL_H__


struct D3D11_CONSTANT_BUFFER_BLOCK;

class D3D11ConstantBufferBlockPool
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
	
public:
	static void Init();
	static void Shutdown();

	static D3D11_CONSTANT_BUFFER_BLOCK* Alloc(size_t BufferSize);
	static void Release(D3D11_CONSTANT_BUFFER_BLOCK* pCBBlock);

private:
	static void AllocPool(CBUFFER_INDEX Index);

private:
	static LINKED_LIST m_CBufferBlockPool[(size_t)CBUFFER_INDEX::EMAX];

};


#endif // __D3D11_CONSTANT_BUFFER_POOL_H__
