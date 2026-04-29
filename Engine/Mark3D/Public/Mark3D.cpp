#include "pch.h"
#include "Mark3D.h"
#include "core.h"



namespace mark
{
	bool Mark3D::Initialize()
	{
		initialaize_core_service(
			1024 * 1024 * 32 // 32MB 임시 버퍼
		);
		
		LOG("Mark3D core memory initialized.");

		return true;
	}

	void Mark3D::Shutdown()
	{
		shutdown_core_service();
	}

}
