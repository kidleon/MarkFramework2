#include "pch.h"
#include "Mark3D.h"
#include "Engine.h"


namespace mark
{
	bool CreateMark3DEngine(
		const EngineCreateDesc& CreateDesc,
		IMark3D** ppOut
	)
	{
		if (!ppOut)
		{
			LOG_ERR("Output pointer is null in CreateMark3DEngine.");
			return false;
		}

		Engine* pEngine = CORE_NEW(Engine);
		if (!pEngine)
		{
			LOG_ERR("Failed to allocate memory for Mark3D Engine.");
			return false;
		}

		if (!pEngine->Initialize(CreateDesc))
		{
			LOG_ERR("Failed to initialize Mark3D Engine.");
			CHECK_RELEASE(pEngine);

			return false;
		}

		*ppOut = pEngine;

		return true;
	}
}
