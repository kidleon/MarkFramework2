#pragma once


namespace mark
{
	class SceneNode final : public ISceneNode
	{
	public:
		SceneNode() noexcept = default;
		~SceneNode() noexcept;

		void SetParent(ISceneNode* pParent, bool KeepWorldTransform = true) noexcept override;
		[[nodiscard]] ISceneNode* GetParent() const noexcept override;

		bool AttachChild(ISceneNode* pChild, bool KeepWorldTransform = true) noexcept override;
		bool DetachChild(ISceneNode* pChild, bool KeepWorldTransform = true) noexcept override;
		bool DetachChildAt(uint32_t ChildIndex, bool KeepWorldTransform = true) noexcept override;
		void DetachAllChildren(bool KeepWorldTransform = true) noexcept override;

		[[nodiscard]] uint32_t GetChildCount() const noexcept override;
		[[nodiscard]] ISceneNode* GetChild(uint32_t ChildIndex) const noexcept override;
		[[nodiscard]] int32_t GetChildIndex(const ISceneNode* pChild) const noexcept override;

		[[nodiscard]] Transform& GetTransform() noexcept override;
		[[nodiscard]] const Transform& GetTransform() const noexcept override;

		bool AttachObject(ISceneObject* pObject) noexcept override;
		bool DetachObject(ISceneObject* pObject) noexcept override;
		bool DetachObjectAt(uint32_t ObjectIndex) noexcept override;
		void DetachAllObjects() noexcept override;

		[[nodiscard]] uint32_t GetObjectCount() const noexcept override;
		[[nodiscard]] ISceneObject* GetObject(uint32_t ObjectIndex) const noexcept override;
		[[nodiscard]] int32_t GetObjectIndex(const ISceneObject* pObject) const noexcept override;

		void SetEnabled(bool Enabled) noexcept override;
		[[nodiscard]] bool IsEnabled() const noexcept override;

		void INL_UpdateWorldTransform() noexcept;
		void INL_MarkTransformDirtyRecursive() noexcept;

	private:
		void AddRef() override;
		void Release() override;

		[[nodiscard]] bool INL_IsAncestorOf(const SceneNode* pNode) const noexcept;
		[[nodiscard]] static SceneNode* INL_AsSceneNode(ISceneNode* pNode) noexcept;
		[[nodiscard]] static const SceneNode* INL_AsSceneNode(const ISceneNode* pNode) noexcept;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		ISceneNode* m_pParent = nullptr;
		sys_vector<ISceneNode*> m_lstChildren;
		sys_vector<ISceneObject*> m_lstObjects;
		Transform m_Transform;
		bool m_Enabled = true;
	};
}
