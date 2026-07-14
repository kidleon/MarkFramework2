#pragma once


namespace mark
{
	class D3D11GPUBuffer final : public IGPUBuffer
	{
	public:
		D3D11GPUBuffer(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pD3D11Buffer) noexcept;

		void AddRef();
		void Release();

		BUFFER_TYPE GetBufferType() const noexcept;
		BUFFER_USAGE GetBufferUsage() const noexcept;
		size_t GetBufferSize() const noexcept;
		void* GetNativePointer() const noexcept;

		bool UpdateBuffer(const void* pData, size_t DataSize, size_t* pWrittenOffset);
		bool UpdateBufferRegion(const void* pData, size_t DataSize, size_t DstOffset);

		inline ID3D11Buffer* INL_GetD3D11Buffer() const noexcept { return m_pD3D11Buffer; }
		inline const D3D11_BUFFER_DESC& INL_GetBufferDesc() const noexcept { return m_BufferDesc; }

	private:
		virtual ~D3D11GPUBuffer() noexcept;

	private:
		mutable std::atomic<int64_t> m_RefCount{ 1 };
		ID3D11DeviceContext* m_pDeviceContext = nullptr;
		ID3D11Buffer* m_pD3D11Buffer = nullptr;
		D3D11_BUFFER_DESC m_BufferDesc = {};
		size_t m_DynamicOffset = 0;
		BOOL m_Cached = FALSE;

	};
}
