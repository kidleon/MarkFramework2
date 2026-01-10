#pragma once


class D3D11ConstantBuffer;

class D3D11ConstantBufferAllocator
{
public:
	D3D11ConstantBufferAllocator();
	~D3D11ConstantBufferAllocator();

	BOOL Init();
	void Shutdown();

	D3D11ConstantBuffer* Acquire(size_t BufferSize);
	D3D11ConstantBuffer* AcquireTemp(size_t BufferSize);

	void Release(D3D11ConstantBuffer* pCB);
	void ResetTemp();
	
};