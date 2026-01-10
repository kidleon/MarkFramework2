#pragma once


class D3D11RenderDevice;

class D3D11ConstantBufferPool
{
public:
	D3D11ConstantBufferPool() = default;
	~D3D11ConstantBufferPool();

	BOOL Init(
		D3D11RenderDevice* pRenderDevice, 
		size_t BufferSize, 
		size_t InitialCount
	);
	void Shutdown();

	void Reset();
	D3D11ConstantBuffer* Acquire();

private:
	void AllocBuffer(
		D3D11RenderDevice* pRenderDevice,
		size_t BufferSize,
		size_t Count
	);

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;
	LINKED_LIST m_FreeList = {};
	LINKED_LIST m_UsedList = {};
	size_t m_BufferSize = 0;
	size_t m_InitialCount = 0;

};
