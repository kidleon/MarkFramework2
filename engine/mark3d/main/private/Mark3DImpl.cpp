#include "pch.h"
#include "Mark3DImpl.h"
#include "SceneNodePool.h"
#include "SceneNode.h"


Mark3DImpl::~Mark3DImpl() noexcept
{
	Shutdown();
}

BOOL Mark3DImpl::Initialize(const void* pCreateDesc)
{
	m_pSceneNodePool = CORE_NEW(SceneNodePool);
	m_pSceneNodePool->Initialize(64);

	return TRUE;
}

void Mark3DImpl::Shutdown()
{
	CORE_DELETE(m_pSceneNodePool, SceneNodePool);
}

BOOL Mark3DImpl::CreateWorld(const char* szWorldName, IWorld** ppOut)
{
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

BOOL Mark3DImpl::CreateModel(NameHash ModelName, size_t MaxVertex, size_t MaxIndex, IModel** ppOut)
{
	// Implementation here
	return TRUE;
}