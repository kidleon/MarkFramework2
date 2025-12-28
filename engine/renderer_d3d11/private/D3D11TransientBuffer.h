#ifndef __D3D11_TRANSIENT_BUFFER_H__
#define __D3D11_TRANSIENT_BUFFER_H__


class D3D11Buffer;
class D3D11RenderDevice;

class D3D11TransientBuffer
{
	static constexpr size_t MAX_TRANSIENT_BUFFERS = 3;

public:
	D3D11TransientBuffer(D3D11RenderDevice* pRenderDevice);
	~D3D11TransientBuffer() noexcept;

	BOOL Init(
		D3D11_BUFFER_TYPE BufferType,
		size_t BufferSize
	);
	void Shutdown() noexcept;
	void Reset() noexcept;

	D3D11Buffer* Alloc(size_t AllocSize);

	__FORCEINLINE ID3D11Buffer* INL_GetBuffer() const noexcept { return m_pFrameBuffer[m_FrameIndex]; }

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;
	ID3D11Buffer* m_pFrameBuffer[MAX_TRANSIENT_BUFFERS] = { nullptr };
	size_t m_BufferSize = 0;
	size_t m_FrameIndex = 0;
	size_t m_CurrOffset = 0;
	D3D11_BUFFER_TYPE m_BufferType;
};


#endif // __D3D11_TRANSIENT_BUFFER_H__
