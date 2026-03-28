#include "pch.h"
#include "Mark3DImpl.h"
#include "SceneNodePool.h"
#include "SceneNode.h"
#include "Assets.h"
#include "RenderSystemFactory.h"
#include "World.h"
#include "Scene.h"
#include "ModelAsset.h"


void CoreMemoryReporter(
	const char* type,
	const char* file,
	int line,
	const char* func,
	size_t size
)
{
	char szTemp[512] = { '\0' };
	sprintf(szTemp, "Memory Report: Type = %s, Size = %zu bytes, Location = %s(%d) %s\n",
		type,
		size,
		file,
		line,
		func
	);
	OutputDebugStringA(szTemp);
}

BOOL __stdcall CreateAndInitEngineModule(const MARK3D_CREATE_DESC& CreateDesc, IMark3D** ppMark3D)
{
	CoreHeap_Init(
		1024 * 1024 * 10, // 10MB의 임시 메모리 풀 크기
		CoreMemoryReporter
	);

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


Mark3DImpl* Mark3DImpl::s_pInstance = nullptr;

Mark3DImpl::Mark3DImpl()
{
	if (!s_pInstance)
		s_pInstance = this;
}

Mark3DImpl::~Mark3DImpl() noexcept
{
	Shutdown();

	if (s_pInstance == this)
		s_pInstance = nullptr;
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
	log_init(LOG_LEVEL_DEBUG, LOG_TYPE_SYSTEM, LOG_OUTPUT_CONSOLE | LOG_OUTPUT_OUTPUT_DEBUG_STRING);

	

#if defined(__TARGET_OS_WINDOWS)

	// 윈도우 핸들 유효성 검사
	if (!CreateDesc.hWnd || !IsWindow(CreateDesc.hWnd))
	{
		SYS_LOG_E("Mark3DImpl::Initialize: Invalid window handle provided.");
		return FALSE;
	}

	// 렌더링 API 지원 여부 검사
	if (CreateDesc.RenderAPI != RENDER_API::D3D11 &&
		CreateDesc.RenderAPI != RENDER_API::D3D12)
	{
		SYS_LOG_E("Mark3DImpl::Initialize: Unsupported render API provided for Windows platform.");

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

		return FALSE;
	}

	m_pAssets = CORE_NEW(Assets);
	if (!m_pAssets->Init(m_pRenderSystem, CreateDesc.szRootPath))
	{
		CHECK_RELEASE(m_pAssets);
		CHECK_RELEASE(m_pRenderSystem);

		return FALSE;
	};

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

	log_shutdown();

	CleanupRenderModule();

	CoreHeap_Shutdown();
}

BOOL Mark3DImpl::GetAssetsInterface(IAssets** ppOut)
{
	if (!ppOut) return FALSE;

	(*ppOut) = m_pAssets;

	return TRUE;
}

BOOL Mark3DImpl::GetRenderSystemInterface(IRenderSystem** ppOut)
{
	if (!ppOut) return FALSE;

	(*ppOut) = m_pRenderSystem;

	//if (m_pRenderSystem)
		//m_pRenderSystem->AddRef();

	return TRUE;
}

BOOL Mark3DImpl::CreateWorld(const char* szWorldName, IWorld** ppOut)
{
	CHECK_RELEASE(m_pWorld);

	m_pWorld = CORE_NEW(World)(szWorldName);
	(*ppOut) = m_pWorld;

	return TRUE;
}

BOOL Mark3DImpl::CreateScene(IWorld* pWorld, const char* szSceneName, IScene** ppOut)
{
	if(!pWorld || !ppOut)
		return FALSE;

	World* pWorldImpl = static_cast<World*>(pWorld);

	Scene* pScene = CORE_NEW(Scene)(szSceneName, pWorld);

	pWorld->AddScene(pScene);

	(*ppOut) = pScene;

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

	TArray<SceneNode*, TA_TEMP> SceneNodeList;

	pSceneNode->GetSceneNodeList(SceneNodeList);

	for (size_t i = 0; i < SceneNodeList.size(); i++)
	{
		SceneNode* pChildNode = SceneNodeList[i];
		m_pSceneNodePool->Release(pSceneNode);
	}
}

IWorld* Mark3DImpl::GetWorld() noexcept
{
	if (m_pWorld)
		m_pWorld->AddRef();

	return m_pWorld;
}

BOOL Mark3DImpl::CreateModel(IModelAsset* pModelAsset, IModel** ppOut)
{
	if (!pModelAsset || !ppOut)
		return FALSE;

	ModelAsset* pModelAssetImpl = static_cast<ModelAsset*>(pModelAsset);

	uint32 ModelAttrib = pModelAssetImpl->INL_GetModelAttrib();
	if (ModelAttrib & (uint32)MODEL_ATTRIB::MESH)
	{
	}

	return TRUE;
}

void Mark3DImpl::UpdateCPU()
{
	CoreHeap_TempReset();

	if (m_pWorld)
	{
		m_pWorld->UpdateSceneTransform();
	}
}
