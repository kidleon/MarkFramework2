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

}

void Scene::AddSceneNode(ISceneNode* pParentNode, ISceneNode* pSceneNode) noexcept
{

}

void Scene::RemoveSceneNode(ISceneNode* pSceneNode) noexcept
{

}

ISceneNode* Scene::GetRootSceneNode() noexcept
{
	return m_pRootSceneNode;
}