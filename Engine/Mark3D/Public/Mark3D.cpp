#include "pch.h"
#include "Mark3D.h"
#include "core.h"


namespace mark
{
	struct EngineInternalData
	{
		// 엔진 내부에서 사용할 데이터 구조체 정의
#if defined(__TARGET_OS_WINDOWS)
		HMODULE RenderSystemModule = nullptr;
		HWND WindowHandle = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)
	};

	static EngineInternalData s_InternalData;

	bool Mark3D::Initialize(const EngineCreateDesc& CreateDesc)
	{
		initialaize_core_service(
			1024 * 1024 * 32 // 32MB 임시 버퍼
		);
		
		LOG("Mark3D core memory initialized.");

#if defined(__TARGET_OS_WINDOWS)
		if (!CreateDesc.WindowHandle)
		{
			LOG_ERR("Invalid window handle provided in EngineCreateDesc.");
			return false;
		}

		if(CreateDesc.PreferredGraphicsAPI == GraphicsAPI::D3D11)
		{
			LOG("Initializing Mark3D with Direct3D 11...");

			// D3D11 초기화 코드 작성
			HMODULE hModule = LoadLibraryA("./RenderSystem_D3D11.dll");
			if (!hModule)
			{
				LOG_ERR("Failed to load RenderSystem_D3D11.dll.");
				return false;
			}

			s_InternalData.RenderSystemModule = hModule;
		}
		else
		{
			LOG_ERR("Unsupported graphics API specified in EngineCreateDesc.");
			return false;
		}
#endif // #if defined(__TARGET_OS_WINDOWS)

		

		return true;
	}

	void Mark3D::Shutdown()
	{
		shutdown_core_service();
	}

}
