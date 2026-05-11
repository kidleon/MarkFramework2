#pragma once


namespace mark
{
	class GPUBufferPool
	{
		enum class GPU_BUFFER_TYPE {
			VERTEX_BUFFER, INDEX_BUFFER, CONSTANT_BUFFER, EMAX
		};

		enum class GPU_BUFFER_USAGE {
			DEFAULT, DYNAMIC, EMAX
		};

		enum class GPU_BUFFER_SIZE : uint8_t {
			SIZE_256, SIZE_512, SIZE_1024, SIZE_2048,
			SIZE_4096, SIZE_8192, SIZE_16384, SIZE_32768,
			SIZE_65536, SIZE_131072, SIZE_262144, SIZE_524288,
			SIZE_1048576, SIZE_2097152, EMAX
		};

		constexpr static size_t GPU_BUFFER_SIZES[] = {
			256, 512, 1024, 2048,
			4096, 8192, 16384, 32768,
			65536, 131072, 262144, 524288,
			1048576, 2097152
		};

		constexpr static size_t GPUBUFFER_COUNT_PER_PAGE[] = {
			128, 128, 128, 128,
			64, 64, 64, 64,
			32, 32, 32, 32,
			16, 16
		};

		struct GPUBufferPageGroup
		{
			sys_vector<IGPUBuffer*> OriginalBuffers;
			sys_deque<IGPUBuffer*> FreeBuffers;
			spin_lock_t SpinLock = { 0 };

			GPUBufferPageGroup()
			{
				OriginalBuffers.reserve(1024);
				SpinLock.stat = 0;
			}
		};

	public:
		~GPUBufferPool() noexcept;

		void Initialize(IHardwareGraphicsLayer* pHardwareGraphicsLayer);

		IGPUBuffer* Acquire(
			BUFFER_TYPE BufferType,
			BUFFER_USAGE BufferUsage,
			size_t BufferSize
		);

		void Release(IGPUBuffer* pBuffer);

	private:
		void Shutdown();
		bool CreateBufferPage(
			GPU_BUFFER_TYPE BufferType,
			GPU_BUFFER_USAGE BufferUsage,
			GPU_BUFFER_SIZE BufferSize
		);

		int32_t MapBufferSize(size_t BufferSize);

	private:
		IHardwareGraphicsLayer* m_pHardwareGraphicsLayer = nullptr;

		GPUBufferPageGroup m_VBPool[(int)BUFFER_USAGE::EMAX][(int)GPU_BUFFER_SIZE::EMAX];
		GPUBufferPageGroup m_IBPool[(int)BUFFER_USAGE::EMAX][(int)GPU_BUFFER_SIZE::EMAX];
		GPUBufferPageGroup m_CBPool[(int)BUFFER_USAGE::EMAX][(int)GPU_BUFFER_SIZE::EMAX];

	};
}
