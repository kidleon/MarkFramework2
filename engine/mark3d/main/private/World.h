#ifndef __WORLD_IMPL_H__
#define __WORLD_IMPL_H__


class Scene;

class World final : public IWorld
{
public:
	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IWorld interface
	virtual int32 AddScene(const char* szSceneName, IScene* pNewScene) noexcept final;

	virtual void RemoveScene(IScene* pScene) noexcept final;
	virtual void RemoveSceneByIndex(size_t Index) noexcept final;
	virtual void RemoveSceneByName(const char* szSceneName) noexcept final;
	virtual void RemoveAllScene() noexcept final;

	virtual size_t GetNumScene() const noexcept final;
	virtual IScene* GetSceneByIndex(size_t Index) noexcept final;
	virtual IScene* GetSceneByName(const char* szSceneName) noexcept final;

private:
	virtual ~World() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	TArray<Scene*, TA_POOL> m_SceneList;

};


#endif // __WORLD_IMPL_H__
