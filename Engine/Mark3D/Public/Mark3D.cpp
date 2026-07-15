#include "pch.h"
#include "Mark3D.h"
#include "Engine.h"
#include "core.h"


namespace mark
{
	static bool s_CoreServiceInitialized = false;

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

		if (!s_CoreServiceInitialized)
		{
			initialize_core_memory(
				1024 * 1024 * 32 // 32MB 임시 버퍼
			);
			s_CoreServiceInitialized = true;
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

	void DestroyMark3DEngine(IMark3D* pEngine)
	{
		if (pEngine)
		{
			pEngine->Release();
			pEngine = nullptr;
		}

		shutdown_core_service();
	}
}
