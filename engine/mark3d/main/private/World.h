#ifndef __WORLD_IMPL_H__
#define __WORLD_IMPL_H__


class Scene;

class World final : public IWorld
{
	constexpr static size_t MAX_WORLD_NAME_LENGTH = 64;

public:
	World(const char* szWorldName);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IWorld interface
	virtual int32 AddScene(IScene* pNewScene) noexcept final;

	virtual void RemoveScene(IScene* pScene) noexcept final;
	virtual void RemoveSceneByIndex(size_t Index) noexcept final;
	virtual void RemoveSceneByName(const char* szSceneName) noexcept final;
	virtual void RemoveAllScene() noexcept final;

	virtual size_t GetNumScene() const noexcept final;
	virtual IScene* GetSceneByIndex(size_t Index) noexcept final;
	virtual IScene* GetSceneByName(const char* szSceneName) noexcept final;

	__FORCEINLINE const char* INL_GetName() const noexcept { return m_szName; }

	void UpdateSceneTransform() noexcept;

private:
	World() = delete;
	virtual ~World() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	TArray<Scene*, TA_POOL> m_SceneList;

	char m_szName[MAX_WORLD_NAME_LENGTH] = { 0 };

};


#endif // __WORLD_IMPL_H__
