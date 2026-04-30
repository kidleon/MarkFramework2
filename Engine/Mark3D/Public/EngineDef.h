#pragma once


namespace mark
{
	struct EngineCreateDesc
	{
		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

		GraphicsAPI PreferredGraphicsAPI = GraphicsAPI::D3D11;

#if defined(__TARGET_OS_WINDOWS)
		HWND WindowHandle = nullptr;
#endif // #if defined(__TARGET_OS_WINDOWS)
	};
}
