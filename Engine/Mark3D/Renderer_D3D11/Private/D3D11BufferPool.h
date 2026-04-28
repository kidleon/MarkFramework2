#pragma once


namespace mark
{
	struct D3D11Buffer;

	class D3D11BufferPool
	{
	public:
		static bool Initialize();

		static void Shutdown();

		static D3D11Buffer* Acquire(
			BUFFER_TYPE BufferType,
			BUFFER_USAGE Usage,
			size_t BufferSize
		) noexcept;

		static void Release(D3D11Buffer* pBuffer);

	};
}
