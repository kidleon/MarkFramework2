#pragma once
#include "memory_block_pool.h"


namespace mark
{
	class SurfaceMaterial;

	class SurfaceMaterialPool
	{
	public:
		~SurfaceMaterialPool() noexcept;

		[[nodiscard]] bool Initialize();
		void Shutdown();

		SurfaceMaterial* Acquire();
		void Release(SurfaceMaterial* pMaterial);

	private:
		HANDLE m_hMemoryBlockPool = nullptr;
		std::atomic<int32_t> m_OutstandingCount{ 0 };

	};
}
