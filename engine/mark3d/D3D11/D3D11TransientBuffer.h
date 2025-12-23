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

	BOOL Init(size_t BufferSize);
	void Shutdown() noexcept;
	void Reset() noexcept;

	D3D11Buffer* AllocVB(size_t AllocSize);
	D3D11Buffer* AllocIB(size_t AllocSize);
	D3D11Buffer* AllocCB(size_t AllocSize);

	__FORCEINLINE ID3D11Buffer* INL_GetVB() const noexcept { return m_pVBPool[m_FrameIndex_VB]; }
	__FORCEINLINE ID3D11Buffer* INL_GetIB() const noexcept { return m_pIBPool[m_FrameIndex_IB]; }
	__FORCEINLINE ID3D11Buffer* INL_GetCB() const noexcept { return m_pCBPool[m_FrameIndex_CB]; }

private:
	D3D11RenderDevice* m_pRenderDevice = nullptr;
	ID3D11Buffer* m_pVBPool[MAX_TRANSIENT_BUFFERS] = { nullptr };
	ID3D11Buffer* m_pIBPool[MAX_TRANSIENT_BUFFERS] = { nullptr };
	ID3D11Buffer* m_pCBPool[MAX_TRANSIENT_BUFFERS] = { nullptr };
	size_t m_BufferSize = 0;
	size_t m_FrameIndex_VB = 0;
	size_t m_FrameIndex_IB = 0;
	size_t m_FrameIndex_CB = 0;
	size_t m_CurrOffset_VB = 0;
	size_t m_CurrOffset_IB = 0;
	size_t m_CurrOffset_CB = 0;
};


#endif // __D3D11_TRANSIENT_BUFFER_H__
