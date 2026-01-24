#pragma once
#include "idgen.h"

class D3D11_COMMON
{
public:
	static BOOL Init() noexcept;
	static void Shutdown() noexcept;
	static UINT64 GetUID() noexcept;
	static const RENDER_SETTINGS& GetRenderSettings() noexcept;
	static void SetRenderSettings(const RENDER_SETTINGS& Settings) noexcept;

};