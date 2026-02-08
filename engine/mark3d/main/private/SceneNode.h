#ifndef __SCENE_NODE_H__
#define __SCENE_NODE_H__

#include "SceneDef.h"


class SceneNode final : public ISceneNode
{
public:
	friend class SceneNodePool;

	// ISceneNode interface
	virtual IScene* GetScene() noexcept final;

	virtual void SetName(const char* szName) noexcept final;

	virtual void SetLocalTransform(const LOCAL_TRANSFORM& Transform) noexcept final;
	virtual const LOCAL_TRANSFORM& GetLocalTransform() const noexcept final;
	virtual const MATRIX4& GetWorldTM() const noexcept final;

	virtual void SetActive(BOOL Active) noexcept final;
	virtual BOOL IsActive() const noexcept final;

	virtual void SetParent(ISceneNode* pParent) noexcept final;
	virtual ISceneNode* GetParent() noexcept final;

	virtual size_t GetNumChilds() const noexcept final;
	virtual LINKED_LIST* GetChildList() noexcept final;
	
	virtual void AddSceneObject(ISceneObject* pSceneObject) noexcept final;
	virtual size_t GetNumSceneObjects() const noexcept final;
	virtual ISceneObject* GetSceneObjectAtIndex(int32 Index) noexcept final;

	__FORCEINLINE LINKED_LIST* INL_GetChildNodeList() noexcept { return &m_ChildNodeList; }
	__FORCEINLINE LINK_NODE* INL_GetLinkNode() noexcept { return &m_LinkNode; }
	__FORCEINLINE LINK_NODE* INL_GetPoolLinkNode() noexcept { return &m_PoolLinkNode; }
	__FORCEINLINE const NameHash INL_GetName() const noexcept { return m_Name; }

	void Reset(BOOL Recursive);

private:
	SceneNode();
	SceneNode(const SceneNode& Other) = delete;
	SceneNode(SceneNode&& Other) noexcept = delete;
	virtual ~SceneNode() noexcept;

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	void RemoveChildNode(SceneNode* pChild);

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	NameHash m_Name;
	BOOL m_Active;
	UINT32 m_LayerMask;
	BOOL m_LinkedChildNode;
	LOCAL_TRANSFORM m_LocalTransform;
	LOCAL_TRANSFORM m_WorldTransform;

	SceneNode* m_pParentNode;
	LINKED_LIST m_ChildNodeList;

	IScene* m_pScene;

	static constexpr size_t MAX_SCENE_OBJECTS = 8;
	ISceneObject* m_pSceneObjects[MAX_SCENE_OBJECTS];
	size_t m_NumSceneObjects;

	LINK_NODE m_LinkNode;

	LINK_NODE m_PoolLinkNode;

};



#endif // __SCENE_NODE_H__
