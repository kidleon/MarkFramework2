#pragma once
#include "GeomDef.h"


namespace mark
{
	struct SurfaceMaterialBlock;

	class SurfaceMaterial : public ISurfaceMaterial
	{
		static constexpr size_t MAX_PASS = 8;

	public:

	private:
		std::atomic<uint32_t> m_RefCount = 1;

		uint32_t m_NumPasses = 0;
		SurfaceMaterialBlock* m_pBlocks[MAX_PASS];
	};
}
