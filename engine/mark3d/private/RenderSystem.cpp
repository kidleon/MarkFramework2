#include "pch.h"
#include "RenderSystem.h"


RenderSystem* RenderSystem::m_pInstance = nullptr;

static void ReportMemoryLeak(
	const char* type,
	const char* file,
	int line,
	const char* func,
	size_t size
)
{
	SYS_LOG_E("Memory Leak Detected: Type=%s, Size=%zu bytes, Location=%s:%d (%s)", type, size, file, line, func);
}

RenderSystem::RenderSystem()
{
	if (!m_pInstance)
		m_pInstance = this;
}

RenderSystem::~RenderSystem() noexcept
{
	if (m_pInstance == this)
		m_pInstance = nullptr;

	Shutdown();
}

void RenderSystem::OnDestroy()
{
	delete this;
}

BOOL RenderSystem::Initialize(
	HWND hWnd,
	uint32 ScreenWidth,
	uint32 ScreenHeight,
	int32 MinLogLevel,
	BOOL Fullscreen
)
{
	if (!Heap_Init(1024 * 1024 * 10, TRUE, ReportMemoryLeak)) // 10MB 임시 풀 생성
		return FALSE;

	log_init(MinLogLevel, LOG_TYPE_SYSTEM | LOG_TYPE_APPLICATION, LOG_OUTPUT_CONSOLE);

	SYS_LOG_I(
		"Mark3D Engine Created: %ux%u, Fullscreen=%s",
		ScreenWidth,
		ScreenHeight,
		(Fullscreen) ? "TRUE" : "FALSE"
	);

	SYS_LOG_I("Log system initialized: MinLogLevel=%u", pDesc->MinLogLevel);

#if defined(__MARK3D_RENDERSYSTEM_D3D11__)
	SYS_LOG_I("Render System: Direct3D 11");
#elif defined(__MARK3D_RENDERSYSTEM_D3D12__)
	SYS_LOG_I("Render System: Direct3D 12");
#elif defined(__MARK3D_RENDERSYSTEM_VULKAN__)
	SYS_LOG_I("Render System: Vulkan");
#elif defined(__MARK3D_RENDERSYSTEM_OPENGL__)
	SYS_LOG_I("Render System: OpenGL");
#elif defined(__MARK3D_RENDERSYSTEM_OPENGLES__)
	SYS_LOG_I("Render System: OpenGL ES");
#elif defined(__MARK3D_RENDERSYSTEM_METAL__)
	SYS_LOG_I("Render System: Metal");
#else
	SYS_LOG_I("Render System: Unknown");
#endif

	return TRUE;
}

void RenderSystem::Shutdown()
{
}
