#ifndef __SCENE_H__
#define __SCENE_H__


struct IWorld;
struct ISceneNode;
class SceneNode;

class Scene final : public IScene
{
	constexpr static size_t MAX_SCENE_NAME_LENGTH = 64;

public:
	Scene(const char* szName, IWorld* pWorld);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IScene interface
	
	virtual const char* GetName() const noexcept final;

	virtual void SetActive(BOOL Active) noexcept final;
	virtual BOOL IsActive() const noexcept final;
	
	virtual IWorld* GetWorld() noexcept final;

	virtual void AddSceneNode(ISceneNode* pSceneNode) noexcept final;
	virtual void AddSceneNode(ISceneNode* pParentNode, ISceneNode* pSceneNode) noexcept final;
	virtual void RemoveSceneNode(ISceneNode* pSceneNode) noexcept final;

	__FORCEINLINE const char* INL_GetName() const noexcept { return m_szName;	}
	__FORCEINLINE BOOL INL_IsActive() const noexcept { return m_Active; }

	void UpdateTransform() noexcept;

private:
	Scene() = delete;
	virtual ~Scene() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	char m_szName[MAX_SCENE_NAME_LENGTH] = { 0 };
	BOOL m_Active = FALSE;
	UINT32 PADDING = 0;

	IWorld* m_pWorld = nullptr;
	TArray<SceneNode*, TA_POOL> m_SceneNodeList;

	THashSet<UINT64, TA_POOL, 256> m_SceneNodeInstanceIDSet;
	
};

#endif // __SCENE_H__
