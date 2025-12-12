#ifndef __D3D11_CONSTANT_BUFFER_POOL_H__
#define __D3D11_CONSTANT_BUFFER_POOL_H__


class D3D11RenderDevice;
struct D3D11ConstantBuffer;

class D3D11ConstantBufferPool
{
public:
	D3D11ConstantBufferPool() = default;
	~D3D11ConstantBufferPool() noexcept;

	BOOL Init(D3D11RenderDevice* pRenderDevice);
	void Shutdown() noexcept;

	D3D11ConstantBuffer* Allocate(size_t AllocSize);
	void Release(D3D11ConstantBuffer* pBuffer);

private:
	BOOL CreateD3D11ConstantBuffer(ID3D11Device* pDevice, LINKED_LIST* pStoreList, size_t SizeInBytes, size_t Count);

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;
	LINKED_LIST m_FreeList[POOL_BLOCK_TYPE_COUNT] = {};
	LINKED_LIST m_UsedList[POOL_BLOCK_TYPE_COUNT] = {};

};

#endif // __D3D11_CONSTANT_BUFFER_POOL_H__
