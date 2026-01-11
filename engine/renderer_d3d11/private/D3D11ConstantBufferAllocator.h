#pragma once


class D3D11RenderDevice;
class D3D11ConstantBuffer;
class D3D11ConstantBufferPool;

class D3D11ConstantBufferAllocator
{
	enum CB_SIZE
	{
		CB_256B = 0,
		CB_512B,
		CB_1KB,
		CB_4KB,
		CB_16KB,
		CB_64KB,

		CB_SIZE_COUNT
	};

	static D3D11ConstantBufferAllocator* s_pInstance;

	static constexpr size_t MAX_FRAME_POOL = 4;

public:
	D3D11ConstantBufferAllocator();
	~D3D11ConstantBufferAllocator();

	BOOL Init(D3D11RenderDevice* pRenderDevice);
	void Shutdown();

	D3D11ConstantBuffer* Acquire(size_t BufferSize);
	void Release(D3D11ConstantBuffer* pCB);

	D3D11ConstantBuffer* AcquireTemp(size_t BufferSize);
	void ResetTemp();

	static D3D11ConstantBufferAllocator* Get() noexcept { return s_pInstance; }
	
private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;
	D3D11ConstantBufferPool* m_pPools[CB_SIZE_COUNT] = {};
	D3D11ConstantBufferPool* m_pTempPools[MAX_FRAME_POOL][CB_SIZE_COUNT] = {};
	size_t m_CurrentFrameIndex = 0;
	 
};