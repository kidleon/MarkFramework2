#pragma once


namespace mark
{
	class SceneNode;

	class Scene final : public IScene
	{
	public:
		Scene(IWorld* pWorld = nullptr, const char* Name = nullptr) noexcept;
		~Scene() noexcept;

		void AddRef() override;
		void Release() override;

		void SetName(const char* Name) noexcept override;
		[[nodiscard]] const char* GetName() const noexcept override;

		void SetOrder(int32_t Order) noexcept override;
		[[nodiscard]] int32_t GetOrder() const noexcept override;

		void SetActive(bool Active) noexcept override;
		[[nodiscard]] bool IsActive() const noexcept override;

		void SetVisible(bool Visible) noexcept override;
		[[nodiscard]] bool IsVisible() const noexcept override;

		IWorld* GetWorld() const noexcept override;

		ISceneNode* CreateSceneNode() noexcept override;
		void DestroySceneNode(ISceneNode* pNode) noexcept override;

		int32_t GetNumSceneNode() const noexcept override;
		ISceneNode* GetSceneNode(const char* Name) const noexcept override;
		ISceneNode* GetSceneNode(uint32_t NodeID) const noexcept override;
		ISceneNode* GetSceneNodeByIndex(int32_t Index) const noexcept override;
		ISceneNode* GetRootSceneNode() const noexcept override;

	private:
		SceneNode* INL_CreateSceneNode(const char* Name) noexcept;
		void INL_DestroySceneNodeRecursive(SceneNode* pNode) noexcept;
		void INL_RemoveSceneNode(SceneNode* pNode) noexcept;

	private:
		std::atomic<int64_t> m_RefCount{ 1 };
		IWorld* m_pWorld = nullptr;
		char m_Name[64] = { 0 };
		int32_t m_Order = 0;
		bool m_Active = true;
		bool m_Visible = true;
		uint32_t m_NextNodeID = 1;
		SceneNode* m_pRootSceneNode = nullptr;
		sys_vector<SceneNode*> m_lstSceneNodes;
		TMemoryBlockPool<SceneNode> m_SceneNodePool;

	};
}
