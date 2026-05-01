#pragma once


namespace mark
{
	class D3D11Buffer
	{
	public:
		D3D11Buffer(
			ID3D11Buffer* pD3D11Buffer,
			size_t BufferSize,
			BUFFER_TYPE BufferType,
			BUFFER_USAGE BufferUsage,
			BOOL Pooled
		) noexcept;

		~D3D11Buffer() noexcept;

		inline ID3D11Buffer* INL_GetD3D11Buffer() const noexcept { return m_pD3D11Buffer; }
		inline BUFFER_TYPE INL_GetBufferType() const noexcept { return m_BufferType; }
		inline BUFFER_USAGE INL_GetBufferUsage() const noexcept { return m_BufferUsage; }
		inline size_t INL_GetBufferSize() const noexcept { return m_BufferSize; }
		inline BOOL INL_IsPooled() const noexcept { return m_Pooled; }
		inline uint16_t INL_GetPageIndex() const noexcept { return m_PageIndex; }
		inline uint16_t INL_GetSlotIndex() const noexcept { return m_SlotIndex; }
		inline void INL_SetPoolIndex(uint16_t pageIndex, uint16_t slotIndex) noexcept { m_PageIndex = pageIndex; m_SlotIndex = slotIndex; }

	private:
		D3D11Buffer() = delete;

	private:
		ID3D11Buffer* m_pD3D11Buffer;
		size_t m_BufferSize;
		BUFFER_TYPE m_BufferType;
		BUFFER_USAGE m_BufferUsage;
		BOOL m_Pooled;
		uint16_t m_PageIndex = 0;
		uint16_t m_SlotIndex = 0;
	};
}
