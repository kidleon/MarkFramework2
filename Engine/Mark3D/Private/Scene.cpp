#include "pch.h"
#include "Scene.h"
#include "SceneNode.h"


namespace mark
{
	Scene::Scene(IWorld* pWorld, const char* Name) noexcept
		: m_pWorld(pWorld)
	{
		SetName(Name ? Name : "");
		m_lstSceneNodes.reserve(1024);
		m_SceneNodePool.Initialize();
		m_pRootSceneNode = INL_CreateSceneNode("Root");
	}

	Scene::~Scene() noexcept
	{
		while (!m_lstSceneNodes.empty())
			INL_DestroySceneNodeRecursive(m_lstSceneNodes.back());

		m_pRootSceneNode = nullptr;
		m_SceneNodePool.Shutdown();
	}

	void Scene::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void Scene::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(Scene, this);
		}
	}

	void Scene::SetName(const char* Name) noexcept
	{
		safe_strcpy(m_Name, sizeof(m_Name), Name ? Name : "");
	}

	const char* Scene::GetName() const noexcept
	{
		return m_Name;
	}

	void Scene::SetOrder(int32_t Order) noexcept
	{
		m_Order = Order;
	}

	int32_t Scene::GetOrder() const noexcept
	{
		return m_Order;
	}

	void Scene::SetActive(bool Active) noexcept
	{
		m_Active = Active;
	}

	bool Scene::IsActive() const noexcept
	{
		return m_Active;
	}

	void Scene::SetVisible(bool Visible) noexcept
	{
		m_Visible = Visible;
	}

	bool Scene::IsVisible() const noexcept
	{
		return m_Visible;
	}

	IWorld* Scene::GetWorld() const noexcept
	{
		return m_pWorld;
	}

	ISceneNode* Scene::CreateSceneNode() noexcept
	{
		SceneNode* pNode = INL_CreateSceneNode(nullptr);
		if (pNode && m_pRootSceneNode)
			m_pRootSceneNode->AttachChild(pNode, false);

		return pNode;
	}

	void Scene::DestroySceneNode(ISceneNode* pNode) noexcept
	{
		SceneNode* pSceneNode = SceneNode::INL_AsSceneNode(pNode);
		if (!pSceneNode || pSceneNode->GetScene() != this || pSceneNode == m_pRootSceneNode)
			return;

		INL_DestroySceneNodeRecursive(pSceneNode);
	}

	int32_t Scene::GetNumSceneNode() const noexcept
	{
		return static_cast<int32_t>(m_lstSceneNodes.size());
	}

	ISceneNode* Scene::GetSceneNode(const char* Name) const noexcept
	{
		if (!Name || !Name[0])
			return nullptr;

		for (SceneNode* pNode : m_lstSceneNodes)
		{
			if (pNode && safe_strcmp(pNode->GetName(), Name) == 0)
				return pNode;
		}

		return nullptr;
	}

	ISceneNode* Scene::GetSceneNode(uint32_t NodeID) const noexcept
	{
		for (SceneNode* pNode : m_lstSceneNodes)
		{
			if (pNode && pNode->GetNodeID() == NodeID)
				return pNode;
		}

		return nullptr;
	}

	ISceneNode* Scene::GetSceneNodeByIndex(int32_t Index) const noexcept
	{
		if (Index < 0 || Index >= static_cast<int32_t>(m_lstSceneNodes.size()))
			return nullptr;

		return m_lstSceneNodes[static_cast<size_t>(Index)];
	}

	ISceneNode* Scene::GetRootSceneNode() const noexcept
	{
		return m_pRootSceneNode;
	}

	SceneNode* Scene::INL_CreateSceneNode(const char* Name) noexcept
	{
		SceneNode* pNode = m_SceneNodePool.Acquire();
		if (!pNode)
			return nullptr;

		const uint32_t NodeID = m_NextNodeID++;
		pNode->INL_Reset(this, NodeID, Name ? Name : "");
		pNode->AddRef();
		m_lstSceneNodes.push_back(pNode);

		return pNode;
	}

	void Scene::INL_DestroySceneNodeRecursive(SceneNode* pNode) noexcept
	{
		if (!pNode || pNode->GetScene() != this)
			return;

		while (pNode->GetChildCount() > 0)
		{
			SceneNode* pChildNode = SceneNode::INL_AsSceneNode(pNode->GetChild(pNode->GetChildCount() - 1));
			INL_DestroySceneNodeRecursive(pChildNode);
		}

		INL_RemoveSceneNode(pNode);
		pNode->INL_SetScene(nullptr);
		pNode->Release();
	}

	void Scene::INL_RemoveSceneNode(SceneNode* pNode) noexcept
	{
		if (!pNode)
			return;

		for (auto it = m_lstSceneNodes.begin(); it != m_lstSceneNodes.end(); ++it)
		{
			if (*it == pNode)
			{
				m_lstSceneNodes.erase(it);
				return;
			}
		}
	}
}
