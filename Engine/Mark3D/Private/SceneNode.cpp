#include "pch.h"
#include "SceneNode.h"


namespace mark
{
	namespace
	{
		template<typename T>
		int32_t FindPointerIndex(const sys_vector<T*>& Items, const T* pItem) noexcept
		{
			for (uint32_t i = 0; i < Items.size(); ++i)
			{
				if (Items[i] == pItem)
					return static_cast<int32_t>(i);
			}

			return -1;
		}
	}

	SceneNode::~SceneNode() noexcept
	{
		DetachAllChildren(true);
		DetachAllObjects();

		if (SceneNode* pParent = INL_AsSceneNode(m_pParent))
		{
			const int32_t ChildIndex = pParent->GetChildIndex(this);
			if (ChildIndex >= 0)
				pParent->m_lstChildren.erase(pParent->m_lstChildren.begin() + ChildIndex);
		}

		m_pParent = nullptr;
	}

	void SceneNode::SetParent(ISceneNode* pParent, bool KeepWorldTransform) noexcept
	{
		if (m_pParent == pParent)
			return;

		SceneNode* pNewParent = INL_AsSceneNode(pParent);
		if (pParent && (!pNewParent || pParent == this || INL_IsAncestorOf(pNewParent)))
			return;

		INL_UpdateWorldTransform();
		const MATRIX4 WorldMatrix = m_Transform.GetWorldMatrix();

		if (SceneNode* pOldParent = INL_AsSceneNode(m_pParent))
		{
			const int32_t ChildIndex = pOldParent->GetChildIndex(this);
			if (ChildIndex >= 0)
				pOldParent->m_lstChildren.erase(pOldParent->m_lstChildren.begin() + ChildIndex);
		}

		m_pParent = pNewParent;

		if (pNewParent && pNewParent->GetChildIndex(this) < 0)
			pNewParent->m_lstChildren.push_back(this);

		if (KeepWorldTransform)
		{
			if (pNewParent)
			{
				pNewParent->INL_UpdateWorldTransform();

				MATRIX4 ParentWorldInverse;
				mat4_inverse(pNewParent->m_Transform.GetWorldMatrix(), ParentWorldInverse);

				MATRIX4 LocalMatrix;
				mat4_mul(WorldMatrix, ParentWorldInverse, LocalMatrix);
				m_Transform.SetLocalMatrix(LocalMatrix);
			}
			else
			{
				m_Transform.SetLocalMatrix(WorldMatrix);
			}
		}
		else
		{
			m_Transform.MarkDirty();
		}

		INL_MarkTransformDirtyRecursive();
	}

	ISceneNode* SceneNode::GetParent() const noexcept
	{
		return m_pParent;
	}

	bool SceneNode::AttachChild(ISceneNode* pChild, bool KeepWorldTransform) noexcept
	{
		SceneNode* pChildNode = INL_AsSceneNode(pChild);
		if (!pChildNode || pChildNode == this || INL_IsAncestorOf(pChildNode))
			return false;

		if (pChildNode->GetParent() == this)
			return true;

		pChildNode->SetParent(this, KeepWorldTransform);
		return pChildNode->GetParent() == this;
	}

	bool SceneNode::DetachChild(ISceneNode* pChild, bool KeepWorldTransform) noexcept
	{
		SceneNode* pChildNode = INL_AsSceneNode(pChild);
		if (!pChildNode || pChildNode->GetParent() != this)
			return false;

		pChildNode->SetParent(nullptr, KeepWorldTransform);
		return pChildNode->GetParent() == nullptr;
	}

	bool SceneNode::DetachChildAt(uint32_t ChildIndex, bool KeepWorldTransform) noexcept
	{
		if (ChildIndex >= m_lstChildren.size())
			return false;

		return DetachChild(m_lstChildren[ChildIndex], KeepWorldTransform);
	}

	void SceneNode::DetachAllChildren(bool KeepWorldTransform) noexcept
	{
		while (!m_lstChildren.empty())
		{
			SceneNode* pChild = INL_AsSceneNode(m_lstChildren.back());
			if (!pChild)
			{
				m_lstChildren.pop_back();
				continue;
			}

			pChild->SetParent(nullptr, KeepWorldTransform);
		}
	}

	uint32_t SceneNode::GetChildCount() const noexcept
	{
		return static_cast<uint32_t>(m_lstChildren.size());
	}

	ISceneNode* SceneNode::GetChild(uint32_t ChildIndex) const noexcept
	{
		if (ChildIndex >= m_lstChildren.size())
			return nullptr;

		return m_lstChildren[ChildIndex];
	}

	int32_t SceneNode::GetChildIndex(const ISceneNode* pChild) const noexcept
	{
		return FindPointerIndex(m_lstChildren, pChild);
	}

	Transform& SceneNode::GetTransform() noexcept
	{
		return m_Transform;
	}

	const Transform& SceneNode::GetTransform() const noexcept
	{
		return m_Transform;
	}

	bool SceneNode::AttachObject(ISceneObject* pObject) noexcept
	{
		if (!pObject || GetObjectIndex(pObject) >= 0)
			return false;

		m_lstObjects.push_back(pObject);
		return true;
	}

	bool SceneNode::DetachObject(ISceneObject* pObject) noexcept
	{
		const int32_t ObjectIndex = GetObjectIndex(pObject);
		if (ObjectIndex < 0)
			return false;

		m_lstObjects.erase(m_lstObjects.begin() + ObjectIndex);
		return true;
	}

	bool SceneNode::DetachObjectAt(uint32_t ObjectIndex) noexcept
	{
		if (ObjectIndex >= m_lstObjects.size())
			return false;

		m_lstObjects.erase(m_lstObjects.begin() + ObjectIndex);
		return true;
	}

	void SceneNode::DetachAllObjects() noexcept
	{
		m_lstObjects.clear();
	}

	uint32_t SceneNode::GetObjectCount() const noexcept
	{
		return static_cast<uint32_t>(m_lstObjects.size());
	}

	ISceneObject* SceneNode::GetObject(uint32_t ObjectIndex) const noexcept
	{
		if (ObjectIndex >= m_lstObjects.size())
			return nullptr;

		return m_lstObjects[ObjectIndex];
	}

	int32_t SceneNode::GetObjectIndex(const ISceneObject* pObject) const noexcept
	{
		return FindPointerIndex(m_lstObjects, pObject);
	}

	void SceneNode::SetEnabled(bool Enabled) noexcept
	{
		m_Enabled = Enabled;
	}

	bool SceneNode::IsEnabled() const noexcept
	{
		return m_Enabled;
	}

	void SceneNode::INL_UpdateWorldTransform() noexcept
	{
		if (SceneNode* pParent = INL_AsSceneNode(m_pParent))
		{
			pParent->INL_UpdateWorldTransform();
			m_Transform.UpdateWorldMatrix(&pParent->m_Transform.GetWorldMatrix());
		}
		else
		{
			m_Transform.UpdateWorldMatrix();
		}

		for (ISceneNode* pChild : m_lstChildren)
		{
			if (SceneNode* pChildNode = INL_AsSceneNode(pChild))
				pChildNode->INL_UpdateWorldTransform();
		}
	}

	void SceneNode::INL_MarkTransformDirtyRecursive() noexcept
	{
		m_Transform.MarkDirty();

		for (ISceneNode* pChild : m_lstChildren)
		{
			if (SceneNode* pChildNode = INL_AsSceneNode(pChild))
				pChildNode->INL_MarkTransformDirtyRecursive();
		}
	}

	void SceneNode::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void SceneNode::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(SceneNode, this);
		}
	}

	bool SceneNode::INL_IsAncestorOf(const SceneNode* pNode) const noexcept
	{
		for (const SceneNode* pParent = INL_AsSceneNode(m_pParent); pParent; pParent = INL_AsSceneNode(pParent->m_pParent))
		{
			if (pParent == pNode)
				return true;
		}

		return false;
	}

	SceneNode* SceneNode::INL_AsSceneNode(ISceneNode* pNode) noexcept
	{
		return static_cast<SceneNode*>(pNode);
	}

	const SceneNode* SceneNode::INL_AsSceneNode(const ISceneNode* pNode) noexcept
	{
		return static_cast<const SceneNode*>(pNode);
	}
}
