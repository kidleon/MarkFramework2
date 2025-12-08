#include "pch.h"
#include "Mark3D.h"
#include "Heap.h"
#include "Log.h"
#include "RenderSystemFactory.h"

IMPLEMENTATION_IUNKNOWN_INTERFACE(Mark3D);


Mark3D::Mark3D()
	: m_pRenderSystem(nullptr)
{
}

Mark3D::~Mark3D() noexcept
{
	if (m_pRenderSystem)
	{
		m_pRenderSystem->Release();
		m_pRenderSystem = nullptr;
	}
}

void Mark3D::OnDestroy()
{
	delete this;
}

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

BOOL Mark3D::CreateEngine(const ENGINE_CREATE_DESC* pDesc)
{
	if (!Heap_Init(1024 * 1024 * 10, TRUE, ReportMemoryLeak)) // 10MB 임시 풀 생성
		return FALSE;

	log_init(pDesc->MinLogLevel, LOG_TYPE_SYSTEM | LOG_TYPE_APPLICATION, LOG_OUTPUT_CONSOLE);

	SYS_LOG_I(
		"Mark3D Engine Created: %ux%u, Fullscreen=%s",
		pDesc->ScreenWidth,
		pDesc->ScreenHeight,
		(pDesc->Fullscreen) ? "TRUE" : "FALSE"
	);

	SYS_LOG_I("Log system initialized: MinLogLevel=%u", pDesc->MinLogLevel);

#if defined(__MARK3D_RENDERSYSTEM_D3D11__)
	SYS_LOG_I("Render System: Direct3D 11");
	CreateRenderSystemInstance(&m_pRenderSystem);
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

