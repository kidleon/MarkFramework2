#ifndef __MARK3D_IMPL_H__
#define __MARK3D_IMPL_H__

#include "Mark3D.h"


class SceneNodePool;

class Mark3DImpl final : public IMark3D
{
public:
	virtual BOOL Initialize(const void* pCreateDesc) final;
	virtual void Shutdown() final;

	virtual BOOL CreateWorld(const char* szWorldName, IWorld** ppOut) final;
	virtual BOOL CreateScene(IWorld* pWorld, const char* szSceneName, IScene** ppOut) final;
	virtual BOOL CreateSceneNode(IScene* pScene, const char* szNodeName, ISceneNode** ppOut) final;
	virtual void ReleaseSceneNode(ISceneNode* pNode) final;

	// SceneObject's APIs
	virtual BOOL CreateModel(NameHash ModelName, size_t MaxVertex, size_t MaxIndex, IModel** ppOut) final;

private:
	virtual ~Mark3DImpl() noexcept;

private:
	SceneNodePool* m_pSceneNodePool = nullptr;
	IWorld* m_pWorld = nullptr;

};

#endif // __MARK3D_IMPL_H__
