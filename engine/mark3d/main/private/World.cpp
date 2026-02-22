#include "pch.h"
#include "World.h"
#include "Scene.h"


World::World(const char* szWorldName)
{
	if (szWorldName)
		fstrlcpy(m_szName, szWorldName, MAX_WORLD_NAME_LENGTH - 1);
}

World::~World() noexcept
{
	RemoveAllScene();
}

long World::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long World::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, World);
	}
	return NewRefCnt;
}

long World::RefCnt()
{
	return m_RefCnt;
}

int32 World::AddScene(IScene* pNewScene) noexcept
{
	if (!pNewScene)
		return -1;

	Scene* pSceneImpl = static_cast<Scene*>(pNewScene);

	for (size_t i = 0; i < m_SceneList.size(); i++)
	{
		Scene* pScene = m_SceneList[i];

		if (fstrcmp(pScene->INL_GetName(), pSceneImpl->INL_GetName()) == 0)
		{
			SYS_LOG_E("Scene with name '%s' already exists in the world.", szSceneName);
			return -1;
		}
	}

	Scene* pNewSceneImpl = static_cast<Scene*>(pNewScene);
	m_SceneList.push_back(pNewSceneImpl);

	return static_cast<int32>(m_SceneList.size() - 1);
}

void World::RemoveScene(IScene* pScene) noexcept
{
	if (!pScene)
		return;

	Scene* pSceneImpl = static_cast<Scene*>(pScene);

	auto it = m_SceneList.find([pSceneImpl](Scene* scene) { return scene == pSceneImpl; });
	if (it != m_SceneList.end())
		m_SceneList.erase(it);
}

void World::RemoveSceneByIndex(size_t Index) noexcept
{
	if (Index >= m_SceneList.size())
		return;

	Scene* pScene = m_SceneList[Index];

	m_SceneList.erase(Index);
}

void World::RemoveSceneByName(const char* szSceneName) noexcept
{
	if (!szSceneName)
		return;

	for (size_t i = 0; i < m_SceneList.size(); i++)
	{
		Scene* pScene = m_SceneList[i];
		if (fstrcmp(pScene->INL_GetName(), szSceneName) == 0)
		{
			m_SceneList.erase(i);
			return;
		}
	}
}

void World::RemoveAllScene() noexcept
{
	if (m_SceneList.empty())
		return;

	for (size_t i = 0; i < m_SceneList.size(); i++)
	{
		Scene* pScene = m_SceneList[i];
		pScene->Release();
	}
	m_SceneList.clear();
}

size_t World::GetNumScene() const noexcept
{
	return m_SceneList.size();
}

IScene* World::GetSceneByIndex(size_t Index) noexcept
{
	if (Index >= m_SceneList.size())
		return nullptr;
	return m_SceneList[Index];
}

IScene* World::GetSceneByName(const char* szSceneName) noexcept
{
	if (!szSceneName)
		return nullptr;

	for (size_t i = 0; i < m_SceneList.size(); i++)
	{
		Scene* pScene = m_SceneList[i];
		if (fstrcmp(pScene->INL_GetName(), szSceneName) == 0)
		{
			return pScene;
		}
	}

	return nullptr;
}

void World::UpdateSceneTransform() noexcept
{
	for (size_t i = 0; i < m_SceneList.size(); i++)
	{
		Scene* pScene = m_SceneList[i];
		if (pScene->INL_IsActive())
		{
			pScene->UpdateTransform();
		}
	}
}