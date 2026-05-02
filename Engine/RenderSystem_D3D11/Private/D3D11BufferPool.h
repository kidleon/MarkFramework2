#pragma once


namespace mark
{
	class D3D11HardwareBuffer;

	class D3D11BufferPool
	{
		struct BufferPageGroup
		{
			sys_vector<ID3D11Buffer*> OriginalBuffers;
			sys_deque<ID3D11Buffer*> FreeBuffers;
			spin_lock_t SpinLock;
		};

	public:
		D3D11BufferPool(ID3D11Device* pDevice);
		~D3D11BufferPool() noexcept;

		ID3D11Buffer* Acquire(
			BUFFER_TYPE BufferType,
			BUFFER_USAGE BufferUsage,
			size_t BufferSize
		);

		bool Release(ID3D11Buffer* pBuffer);

	private:
		D3D11BufferPool() = delete;

		bool CreateBufferPage(
			POOL_BUFFER_TYPE BufferType,
			POOL_BUFFER_USAGE BufferUsage,
			POOL_BUFFER_SIZE BufferSize
		);

		void Shutdown();

	private:
		ID3D11Device* m_pD3D11Device = nullptr;
		BufferPageGroup m_VBPool[(int)BUFFER_USAGE::EMAX][(int)POOL_BUFFER_SIZE::EMAX];
		BufferPageGroup m_IBPool[(int)BUFFER_USAGE::EMAX][(int)POOL_BUFFER_SIZE::EMAX];
		BufferPageGroup m_CBPool[(int)BUFFER_USAGE::EMAX][(int)POOL_BUFFER_SIZE::EMAX];

	};
}
