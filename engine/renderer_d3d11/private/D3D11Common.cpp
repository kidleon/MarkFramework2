#include "pch.h"
#include "D3D11Common.h"
#include "D3D11RenderQueuePool.h"


static volatile ULONG64 CURRENT_UID = 1; // 전역 고유 식별자 카운터
static D3D11RenderQueuePool* RENDER_QUEUE_POOL = nullptr; // 전역 렌더 큐 풀
static RENDER_SETTINGS RENDER_SETTINGS_DATA; // 전역 렌더링 설정 데이터

BOOL D3D11_COMMON::Init() noexcept
{
	if (!RENDER_QUEUE_POOL)
	{
		RENDER_QUEUE_POOL = D3D11_NEW(D3D11RenderQueuePool)();
		RENDER_QUEUE_POOL->Init();
	}

	return TRUE;
}

void D3D11_COMMON::Shutdown() noexcept
{
	// 종료 코드 작성
	if (RENDER_QUEUE_POOL)
	{
		D3D11_DELETE(RENDER_QUEUE_POOL, D3D11RenderQueuePool);
		RENDER_QUEUE_POOL = nullptr;
	}

	CURRENT_UID = 0;
}

UINT64 D3D11_COMMON::GetUID() noexcept
{
	return interlock_increment_ul64(&CURRENT_UID, MEMORY_ORDER_RELAXED);
}

const RENDER_SETTINGS& D3D11_COMMON::GetRenderSettings() noexcept
{
	return RENDER_SETTINGS_DATA;
}

void D3D11_COMMON::SetRenderSettings(const RENDER_SETTINGS& Settings) noexcept
{
	RENDER_SETTINGS_DATA = Settings;
}