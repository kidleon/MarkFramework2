#include "pch.h"
#include "SurfaceMaterialFactory.h"
#include "SurfaceMaterial.h"
#include "SurfaceMaterialBlock.h"


namespace mark
{
	object_pool<SurfaceMaterial> SurfaceMaterialFactory::s_Pool;
	object_pool<SurfaceMaterialBlock> SurfaceMaterialFactory::s_BlockPool;

	SurfaceMaterial* SurfaceMaterialFactory::New()
	{
		return s_Pool.acquire();
	}

	void SurfaceMaterialFactory::Release(SurfaceMaterial* pMaterial)
	{
		if (pMaterial)
		{
			pMaterial->~SurfaceMaterial(); // 객체 소멸자 호출
			s_Pool.release(pMaterial); // 객체 풀로 반환
		}
	}

	SurfaceMaterialBlock* SurfaceMaterialFactory::NewBlock()
	{
		return s_BlockPool.acquire();
	}

	void SurfaceMaterialFactory::ReleaseBlock(SurfaceMaterialBlock* pBlock)
	{
		if (pBlock)
		{
			pBlock->~SurfaceMaterialBlock(); // 객체 소멸자 호출
			s_BlockPool.release(pBlock); // 객체 풀로 반환
		}
	}
}

