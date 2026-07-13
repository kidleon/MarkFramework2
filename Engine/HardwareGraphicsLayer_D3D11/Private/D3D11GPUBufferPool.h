#pragma once


namespace mark
{
	class D3D11GPUBuffer;
	class D3D11HardwareGraphicsLayer;

	class D3D11GPUBufferPool
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
			sys_vector<D3D11GPUBuffer*> OriginalBuffers;
			sys_deque<D3D11GPUBuffer*> FreeBuffers;
			spin_lock_t SpinLock = { 0 };

			GPUBufferPageGroup()
			{
				OriginalBuffers.reserve(1024);
				SpinLock.stat = 0;
			}
		};

	public:
		~D3D11GPUBufferPool() noexcept;

		void Initialize(D3D11HardwareGraphicsLayer* pHardwareGraphicsLayer);

		/**
		 * @brief 풀에서 GPU 버퍼를 꺼냅니다 (없으면 새 페이지 생성).
		 * @return [Pool-owned] 반환된 포인터는 AddRef된 상태입니다. 호출자는 다 쓰고 나면
		 *         반드시 D3D11GPUBufferPool::Release(pBuffer)로 풀에 반납해야 합니다.
		 *         (IGPUBuffer::Release()를 직접 호출하면 풀의 OriginalBuffers에 stale 엔트리가 남아
		 *         사실상 leak이 됩니다.) 실패 시 nullptr.
		 */
		[[nodiscard]] D3D11GPUBuffer* Acquire(
			BUFFER_TYPE BufferType,
			BUFFER_USAGE BufferUsage,
			size_t BufferSize
		);

		/**
		 * @brief Acquire()로 받은 GPU 버퍼를 풀로 반납합니다.
		 *        호출 후 pBuffer는 풀 내부의 FreeBuffers에 들어가며 호출자는 더 이상 접근하면 안 됩니다.
		 */
		void Release(D3D11GPUBuffer* pBuffer);

	private:
		void Shutdown();
		bool CreateBufferPage(
			GPU_BUFFER_TYPE BufferType,
			GPU_BUFFER_USAGE BufferUsage,
			GPU_BUFFER_SIZE BufferSize
		);

		int32_t MapBufferSize(size_t BufferSize);

	private:
		D3D11HardwareGraphicsLayer* m_pHardwareGraphicsLayer = nullptr;

		GPUBufferPageGroup m_VBPool[(int)BUFFER_USAGE::EMAX][(int)GPU_BUFFER_SIZE::EMAX];
		GPUBufferPageGroup m_IBPool[(int)BUFFER_USAGE::EMAX][(int)GPU_BUFFER_SIZE::EMAX];
		GPUBufferPageGroup m_CBPool[(int)BUFFER_USAGE::EMAX][(int)GPU_BUFFER_SIZE::EMAX];

	};
}
