#include "pch.h"
#include "World.h"


namespace mark
{
	World::~World() noexcept
	{
		BigRip();
	}

	void World::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void World::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(World, this);
		}
	}

	void World::BigBang(const char* Name) noexcept
	{
		BigRip();
		safe_strcpy(m_Name, sizeof(m_Name), Name ? Name : "");
		m_lstScenes.reserve(16);
	}

	void World::BigRip() noexcept
	{
		while (!m_lstScenes.empty())
		{
			Scene* pScene = m_lstScenes.back();
			m_lstScenes.pop_back();

			if (pScene)
				pScene->Release();
		}
	}

	IScene* World::CreateScene(const char* Name) noexcept
	{
		if (IScene* pExistingScene = GetScene(Name))
		{
			pExistingScene->AddRef();
			return pExistingScene;
		}

		Scene* pScene = CORE_NEW(Scene)(this, Name);
		if (!pScene)
			return nullptr;

		pScene->AddRef();
		m_lstScenes.push_back(pScene);

		return pScene;
	}

	IScene* World::LoadScene(const char* FilePath, bool Additive, bool Async) noexcept
	{
		(void)FilePath;
		(void)Additive;
		(void)Async;
		return nullptr;
	}

	void World::DestroyScene(const char* Name) noexcept
	{
		if (!Name || !Name[0])
			return;

		for (auto it = m_lstScenes.begin(); it != m_lstScenes.end(); ++it)
		{
			Scene* pScene = *it;
			if (pScene && safe_strcmp(pScene->GetName(), Name) == 0)
			{
				m_lstScenes.erase(it);
				pScene->Release();
				return;
			}
		}
	}

	IScene* World::GetScene(const char* Name) const noexcept
	{
		if (!Name || !Name[0])
			return nullptr;

		for (Scene* pScene : m_lstScenes)
		{
			if (pScene && safe_strcmp(pScene->GetName(), Name) == 0)
				return pScene;
		}

		return nullptr;
	}
}
