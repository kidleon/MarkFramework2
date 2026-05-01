#pragma once


namespace mark
{
	class D3D11Buffer;

	class D3D11BufferPool
	{
		struct BufferIndex
		{
			uint16_t PageIndex;
			uint16_t SlotIndex;
		};

		struct BufferPage
		{
			D3D11Buffer* pBufferSlots;
			uint32_t NumBuffers;
			uint32_t PageIndex;
		};

		struct BufferPageGroup
		{
			sys_vector<BufferPage> BufferPages;
			sys_deque<BufferIndex> FreeBuffers;
			spin_lock_t SpinLock;
		};

	public:
		~D3D11BufferPool() noexcept;

		void Initialize(ID3D11Device* pDevice) noexcept { m_pD3D11Device = pDevice; }

		bool GetBuffer(
			BUFFER_TYPE BufferType,
			BUFFER_USAGE BufferUsage,
			size_t BufferSize,
			D3D11Buffer** ppOut
		);

		void ReleaseBuffer(D3D11Buffer* pBuffer);

	private:
		bool CreateBufferPage(
			POOL_BUFFER_TYPE BufferType,
			POOL_BUFFER_USAGE BufferUsage,
			POOL_BUFFER_SIZE BufferSize
		);

		void Shutdown();

	private:
		ID3D11Device* m_pD3D11Device = nullptr;
		BufferPageGroup m_VBPool[(int)POOL_BUFFER_USAGE::EMAX][(int)POOL_BUFFER_SIZE::EMAX];
		BufferPageGroup m_IBPool[(int)POOL_BUFFER_USAGE::EMAX][(int)POOL_BUFFER_SIZE::EMAX];
		BufferPageGroup m_CBPool[(int)POOL_BUFFER_USAGE::EMAX][(int)POOL_BUFFER_SIZE::EMAX];

	};
}
