	#pragma once
#include "object_pool.h"

namespace mark
{
	class SurfaceMaterial;
	struct SurfaceMaterialBlock;

	class SurfaceMaterialFactory
	{
	public:
		static SurfaceMaterial* New();
		static void Release(SurfaceMaterial* pMaterial);

		static SurfaceMaterialBlock* NewBlock();
		static void ReleaseBlock(SurfaceMaterialBlock* pBlock);

	private:
		static object_pool<SurfaceMaterial> s_Pool;
		static object_pool<SurfaceMaterialBlock> s_BlockPool;

	};
}
