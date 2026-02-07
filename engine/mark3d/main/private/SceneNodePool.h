#ifndef __SCENE_NODE_POOL_H__
#define __SCENE_NODE_POOL_H__


class SceneNode;

class SceneNodePool
{
public:
	~SceneNodePool();

	void Initialize(size_t PageSize);
	void Shutdown();

	SceneNode* Acquire();
	void Release(SceneNode* pNode);

	static inline SceneNodePool* GetInstance() 
	{
		static SceneNodePool instance;
		return &instance;
	}

private:
	void ExpandPool(size_t Count);

private:
	LINKED_LIST m_FreeList = {};
	LINKED_LIST m_UsedList = {};

};

#endif // __SCENE_NODE_POOL_H__
