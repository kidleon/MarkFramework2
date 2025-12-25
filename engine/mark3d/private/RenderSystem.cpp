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
}

long RenderSystem::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long RenderSystem::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		delete this;
	}
	return NewRefCnt;
}

long RenderSystem::RefCnt()
{
	return m_RefCnt;
}
