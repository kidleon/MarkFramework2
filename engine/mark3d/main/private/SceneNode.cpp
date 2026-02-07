#include "pch.h"
#include "SceneNode.h"
#include "SceneNodePool.h"


SceneNode::SceneNode()
	: m_Active(FALSE)
	, m_LayerMask(0x00)
	, m_LinkedChildNode(FALSE)
	, m_LocalTransform{}
	, m_WorldTransform{}
	, m_pParentNode(nullptr)
	, m_pScene(nullptr)
	, m_pSceneObjects{}
	, m_NumSceneObjects(0)
	, m_LinkNode{}
	, m_PoolLinkNode{}
{
	m_LinkNode.data = this;
	m_PoolLinkNode.data = this;
	m_LocalTransform.Scale = { 1.0f, 1.0f, 1.0f };
	m_LocalTransform.Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	init_linked_list(&m_ChildNodeList);
}

SceneNode::~SceneNode() noexcept
{
	Reset(FALSE);
}

long SceneNode::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long SceneNode::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (NewRefCnt == 1)
	{
		SceneNodePool::GetInstance()->Release(this);
		return 1;
	}

	if (NewRefCnt == 0)
	{
		CORE_DELETE(this, SceneNode);
	}
	return NewRefCnt;
}

long SceneNode::RefCnt()
{
	return m_RefCnt;
}

IScene* SceneNode::GetScene() noexcept
{
	return m_pScene;
}

void SceneNode::SetName(const char* szName) noexcept
{
	m_Name = NameHash(szName);
}

void SceneNode::SetLocalTransform(const LOCAL_TRANSFORM& Transform) noexcept
{
	m_LocalTransform = Transform;
}

const LOCAL_TRANSFORM& SceneNode::GetLocalTransform() const noexcept
{
	return m_LocalTransform;
}

const MATRIX4& SceneNode::GetWorldTM() const noexcept
{
	return m_WorldTransform.TM;
}

void SceneNode::SetActive(BOOL Active) noexcept
{
	m_Active = Active;
}

BOOL SceneNode::IsActive() const noexcept
{
	return m_Active;
}

void SceneNode::SetParent(ISceneNode* pParent) noexcept
{
	if (m_pParentNode)
	{
		m_pParentNode->RemoveChildNode(this);
		m_pParentNode = nullptr;
	}

	m_pParentNode = static_cast<SceneNode*>(pParent);

	if (m_pParentNode)
	{
		if (!linked_list_exists(m_pParentNode->INL_GetChildNodeList(), &m_LinkNode))
		{
			linked_list_push_back(m_pParentNode->INL_GetChildNodeList(), &m_LinkNode);
		}
	}
}

ISceneNode* SceneNode::GetParent() noexcept
{
	return m_pParentNode;
}

size_t SceneNode::GetNumChilds() const noexcept
{
	return m_ChildNodeList.size;
}

LINKED_LIST* SceneNode::GetChildList() noexcept
{
	return &m_ChildNodeList;
}

void SceneNode::AddSceneObject(ISceneObject* pSceneObject) noexcept
{
	if (m_NumSceneObjects >= MAX_SCENE_OBJECTS)
	{
		SYS_LOG_E("SceneNode::AddSceneObject: Exceeded maximum number of scene objects per node.");
		return;
	}

	m_pSceneObjects[m_NumSceneObjects++] = pSceneObject;
}

size_t SceneNode::GetNumSceneObjects() const noexcept
{
	return m_NumSceneObjects;
}

ISceneObject* SceneNode::GetSceneObjectAtIndex(int32 Index) noexcept
{
	if (Index < 0 || MAX_SCENE_OBJECTS <= Index)
		return nullptr;

	return m_pSceneObjects[Index];
}

void SceneNode::Reset(BOOL Recursive)
{
	for(size_t i = 0; i < m_NumSceneObjects; ++i)
	{
		CHECK_RELEASE(m_pSceneObjects[i]);
	}

	if (Recursive)
	{
		LINK_NODE* pChildNode = linked_list_pop_front(&m_ChildNodeList);
		while (pChildNode)
		{
			SceneNode* pSceneNode = (SceneNode*)pChildNode->data;
			pSceneNode->Reset(Recursive);
			pChildNode = linked_list_pop_front(&m_ChildNodeList);
		}
	}
	
	m_Active = FALSE;
	m_LayerMask = 0x00;
	m_LinkedChildNode = FALSE;
	m_pParentNode = nullptr;
	m_LinkNode.next = m_LinkNode.prev = nullptr;
	m_pScene = nullptr;
	m_NumSceneObjects = 0;
	init_linked_list(&m_ChildNodeList);
	m_LocalTransform.Position = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_LocalTransform.Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_LocalTransform.Scale = { 1.0f, 1.0f, 1.0f };
	m_LocalTransform.TM = mat4_ident();
	m_WorldTransform = m_LocalTransform;
}

void SceneNode::RemoveChildNode(SceneNode* pChild)
{
	if (!m_pParentNode)
		return;

	linked_list_remove_node(&m_ChildNodeList, pChild->INL_GetLinkNode());
}