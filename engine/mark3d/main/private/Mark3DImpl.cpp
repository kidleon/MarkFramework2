#include "pch.h"
#include "Mark3DImpl.h"
#include "SceneNodePool.h"
#include "SceneNode.h"
#include "Assets.h"
#include "RenderSystemFactory.h"


BOOL __stdcall CreateAndInitEngineModule(const MARK3D_CREATE_DESC& CreateDesc, IMark3D** ppMark3D)
{
	Mark3DImpl* pImpl = CORE_NEW(Mark3DImpl);

	if (!pImpl->Initialize(CreateDesc))
	{
		CHECK_RELEASE(pImpl);
		(*ppMark3D) = nullptr;
		return FALSE;
	}

	(*ppMark3D) = pImpl;

	return TRUE;
}


Mark3DImpl::~Mark3DImpl() noexcept
{
	Shutdown();
}

long Mark3DImpl::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Mark3DImpl::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, Mark3DImpl);
	}
	return NewRefCnt;
}

long Mark3DImpl::RefCnt()
{
	return m_RefCnt;
}

BOOL Mark3DImpl::Initialize(const MARK3D_CREATE_DESC& CreateDesc)
{
	m_pAssets = CORE_NEW(Assets);
	if (!m_pAssets->Init(CreateDesc.szRootPath))
	{
		CHECK_RELEASE(m_pAssets);

		return FALSE;
	};

#if defined(__TARGET_OS_WINDOWS)

	// 윈도우 핸들 유효성 검사
	if (!CreateDesc.hWnd || !IsWindow(CreateDesc.hWnd))
	{
		SYS_LOG_E("Mark3DImpl::Initialize: Invalid window handle provided.");
		CHECK_RELEASE(m_pAssets);

		return FALSE;
	}

	// 렌더링 API 지원 여부 검사
	if (CreateDesc.RenderAPI != RENDER_API::D3D11 &&
		CreateDesc.RenderAPI != RENDER_API::D3D12)
	{
		SYS_LOG_E("Mark3DImpl::Initialize: Unsupported render API provided for Windows platform.");
		CHECK_RELEASE(m_pAssets);

		return FALSE;
	}

	RENDERER_CREATE_DESC RenderCreateDesc = {};
	RenderCreateDesc.hWnd = CreateDesc.hWnd;
	RenderCreateDesc.RenderAPI = CreateDesc.RenderAPI;
	RenderCreateDesc.ScreenWidth = CreateDesc.ScreenWidth;
	RenderCreateDesc.ScreenHeight = CreateDesc.ScreenHeight;
	RenderCreateDesc.Fullscreen = CreateDesc.Fullscreen;

	if (!CreateAndInitRenderModule(RenderCreateDesc, &m_pRenderSystem))
	{
		SYS_LOG_E("Mark3DImpl::Initialize: Failed to create and initialize render module.");
		CHECK_RELEASE(m_pAssets);

		return FALSE;
	}

#endif // defined(__TARGET_OS_WINDOWS)

	m_pSceneNodePool = CORE_NEW(SceneNodePool);
	m_pSceneNodePool->Initialize(64);

	return TRUE;
}

void Mark3DImpl::Shutdown()
{
	CHECK_RELEASE(m_pWorld);
	CORE_DELETE(m_pSceneNodePool, SceneNodePool);
	CHECK_RELEASE(m_pAssets);

	CHECK_RELEASE(m_pRenderSystem);

	CleanupRenderModule();
}

BOOL Mark3DImpl::GetAssetsInterface(IAssets** ppOut)
{
	if (!ppOut) return FALSE;

	(*ppOut) = m_pAssets;

	if (m_pAssets)
		m_pAssets->AddRef();

	return TRUE;
}

BOOL Mark3DImpl::GetRenderSystemInterface(IRenderSystem** ppOut)
{
	(*ppOut) = m_pRenderSystem;

	if (m_pRenderSystem)
		m_pRenderSystem->AddRef();

	return TRUE;
}

BOOL Mark3DImpl::CreateWorld(const char* szWorldName, IWorld** ppOut)
{
	CHECK_RELEASE(m_pWorld);

    // Implementation here
	return TRUE;
}

BOOL Mark3DImpl::CreateScene(IWorld* pWorld, const char* szSceneName, IScene** ppOut)
{
    // Implementation here
	return TRUE;
}

BOOL Mark3DImpl::CreateSceneNode(IScene* pScene, const char* szNodeName, ISceneNode** ppOut)
{
    // Implementation here
	SceneNode* pSceneNode = m_pSceneNodePool->Acquire();
	if (!pSceneNode)
	{
		(*ppOut) = nullptr;
		return FALSE;
	}

	pSceneNode->SetName(szNodeName);
	(*ppOut) = pSceneNode;

	return TRUE;
}

void Mark3DImpl::ReleaseSceneNode(ISceneNode* pNode)
{
	if (!pNode)
		return;

	SceneNode* pSceneNode = static_cast<SceneNode*>(pNode);
	m_pSceneNodePool->Release(pSceneNode);
}

BOOL Mark3DImpl::CreateModel(NameHash ModelName, size_t MaxVertex, size_t MaxIndex, IModel** ppOut)
{
	// Implementation here
	return TRUE;
}