#ifndef __MARK3D_IMPL_H__
#define __MARK3D_IMPL_H__

#include "Mark3D.h"


interface IRenderSystem;
class SceneNodePool;
class Assets;

class Mark3DImpl final : public IMark3D
{
public:
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IMark3D interface
	virtual BOOL Initialize(const MARK3D_CREATE_DESC& CreateDesc) final;
	virtual void Shutdown() final;

	// Asset's APIs
	virtual BOOL GetAssetsInterface(IAssets** ppOut) final;
	virtual BOOL GetRenderSystemInterface(IRenderSystem** ppOut) final;

	// Scene's APIs
	virtual BOOL CreateWorld(const char* szWorldName, IWorld** ppOut) final;
	virtual BOOL CreateScene(IWorld* pWorld, const char* szSceneName, IScene** ppOut) final;
	virtual BOOL CreateSceneNode(IScene* pScene, const char* szNodeName, ISceneNode** ppOut) final;
	virtual void ReleaseSceneNode(ISceneNode* pNode) final;

	// SceneObject's APIs
	virtual BOOL CreateModel(NameHash ModelName, size_t MaxVertex, size_t MaxIndex, IModel** ppOut) final;

private:
	virtual ~Mark3DImpl() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	SceneNodePool* m_pSceneNodePool = nullptr;
	IWorld* m_pWorld = nullptr;

	Assets* m_pAssets = nullptr;
	IRenderSystem* m_pRenderSystem = nullptr;

};

#endif // __MARK3D_IMPL_H__
