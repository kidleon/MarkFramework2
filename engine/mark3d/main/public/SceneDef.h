#ifndef __SCENE_DEF_H__
#define __SCENE_DEF_H__


struct IScene;
struct ISceneNode;
struct ISceneObject;

struct IWorld : public IUNKNOWN
{
	virtual size_t GetNumScene() const noexcept = 0;
	virtual IScene* GetScene(size_t Index) noexcept = 0;
};

//------------------------------------------------------------------------------
struct IScene : public IUNKNOWN
{
	virtual IWorld* GetWorld() noexcept = 0;
	
	virtual void SetActive(BOOL Active) noexcept = 0;
	virtual BOOL IsActive() const noexcept = 0;

	virtual ISceneNode* GetRootSceneNode() noexcept = 0;

};

//------------------------------------------------------------------------------
struct ISceneNode : public IPRIVATE_UNKNOWN
{
	virtual IScene* GetScene() noexcept = 0;

	virtual void SetName(const char* szName) noexcept = 0;

	virtual void SetLocalTransform(const LOCAL_TRANSFORM& Transform) noexcept = 0;
	virtual const LOCAL_TRANSFORM& GetLocalTransform() const noexcept = 0;
	virtual const MATRIX4& GetWorldTM() const noexcept = 0;

	virtual void SetActive(BOOL Active) noexcept = 0;
	virtual BOOL IsActive() const noexcept = 0;

	virtual void SetParent(ISceneNode* pParent) noexcept = 0;
	virtual ISceneNode* GetParent() noexcept = 0;

	virtual size_t GetNumChilds() const noexcept = 0;
	virtual LINKED_LIST* GetChildList() noexcept = 0;
	
	virtual void AddSceneObject(ISceneObject* pSceneObject) noexcept = 0;
	virtual size_t GetNumSceneObjects() const noexcept = 0;
	virtual ISceneObject* GetSceneObjectAtIndex(int32 Index) noexcept = 0;

};


#endif // __SCENE_DEF_H__
