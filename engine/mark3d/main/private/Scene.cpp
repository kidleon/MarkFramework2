#include "pch.h"
#include "Scene.h"
#include "World.h"
#include "SceneNode.h"


Scene::Scene(const char* szName, IWorld* pWorld)
	: m_pWorld(pWorld)
{
	fstrlcpy(m_szName, szName, MAX_SCENE_NAME_LENGTH - 1);
}

Scene::~Scene() noexcept
{
}

long Scene::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Scene::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, Scene);
	}
	return NewRefCnt;
}

long Scene::RefCnt()
{
	return m_RefCnt;
}

IWorld* Scene::GetWorld() noexcept
{
	return m_pWorld;
}

const char* Scene::GetName() const noexcept
{
	return m_szName;
}

void Scene::SetActive(BOOL Active) noexcept
{
	m_Active = Active;
}

BOOL Scene::IsActive() const noexcept
{
	return m_Active;
}

void Scene::AddSceneNode(ISceneNode* pSceneNode) noexcept
{
	if (!pSceneNode)
		return;

	UINT64 InstanceID = pSceneNode->GetInstanceID();
	auto it = m_SceneNodeInstanceIDSet.find(InstanceID);
	if (it != m_SceneNodeInstanceIDSet.end())
	{
		SYS_LOG_E("SceneNode with instance ID '%llu' already exists in the scene.", InstanceID);
		return;
	}

	m_SceneNodeList.push_back(static_cast<SceneNode*>(pSceneNode));
	m_SceneNodeInstanceIDSet.insert(InstanceID);
}

void Scene::AddSceneNode(ISceneNode* pParentNode, ISceneNode* pSceneNode) noexcept
{
	if (!pSceneNode)
		return;

	// Parent Node가 존재하지 않는 경우, Scene의 최상위 노드로 추가
	if (!pParentNode)
	{
		AddSceneNode(pSceneNode);
		return;
	}

	// Parent Node가 존재하는 경우, Parent Node의 자식으로 추가
	UINT64 InstanceID = pSceneNode->GetInstanceID();
	auto it = m_SceneNodeInstanceIDSet.find(InstanceID);
	if (it != m_SceneNodeInstanceIDSet.end())
	{
		SYS_LOG_E("SceneNode with instance ID '%llu' already exists in the scene.", InstanceID);
		return;
	}

	SceneNode* pParentNodeImpl = static_cast<SceneNode*>(pParentNode);
	SceneNode* pSceneNodeImpl = static_cast<SceneNode*>(pSceneNode);

	pSceneNodeImpl->SetParent(pParentNodeImpl);
}

void Scene::RemoveSceneNode(ISceneNode* pSceneNode) noexcept
{
	if (!pSceneNode)
		return;

	UINT64 InstanceID = pSceneNode->GetInstanceID();
	auto it = m_SceneNodeInstanceIDSet.find(InstanceID);
	if (it == m_SceneNodeInstanceIDSet.end())
	{
		SYS_LOG_E("SceneNode with instance ID '%llu' does not exist in the scene.", InstanceID);
		return;
	}

	SceneNode* pSceneNodeImpl = static_cast<SceneNode*>(pSceneNode);

	SceneNode* pParentNode = pSceneNodeImpl->INL_GetParentNode();
	if (pParentNode)
		pParentNode->RemoveChildNode(pSceneNodeImpl);
	else
	{
		// 최상위 노드인 경우, Scene에서 직접 제거
		auto it2 = m_SceneNodeList.find([pSceneNodeImpl](SceneNode* node) { return node == pSceneNodeImpl; });
		if (it2 != m_SceneNodeList.end())
		{
			m_SceneNodeList.erase(it2);
		}

		m_SceneNodeInstanceIDSet.erase(InstanceID);
	}
}

void Scene::UpdateTransform() noexcept
{
	for (size_t i = 0; i < m_SceneNodeList.size(); i++)
	{
		SceneNode* pSceneNode = m_SceneNodeList[i];
		pSceneNode->ComputeTransform(nullptr);
	}
}
