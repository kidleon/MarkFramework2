#pragma once
#include "TMemoryBlockPool.h"


namespace mark
{
	class Scene;

	class SceneNode final : public ISceneNode
	{
		friend class Scene;
		friend class TMemoryBlockPool<SceneNode>;

	public:
		~SceneNode() noexcept;

		void SetName(const char* Name) noexcept override;
		[[nodiscard]] const char* GetName() const noexcept override;
		[[nodiscard]] uint32_t GetNodeID() const noexcept override;
		[[nodiscard]] IScene* GetScene() const noexcept override;

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

		void SetDirty(bool Dirty) noexcept override;
		[[nodiscard]] bool IsDirty() const noexcept override;

		void INL_UpdateWorldTransform() noexcept;
		void INL_MarkTransformDirtyRecursive() noexcept;

	private:
		SceneNode(TMemoryBlockPool<SceneNode>* pPool) noexcept;

		void AddRef() override;
		void Release() override;

		void INL_Reset(Scene* pScene, uint32_t NodeID, const char* Name) noexcept;
		void INL_SetScene(Scene* pScene) noexcept;
		inline void SetMemoryBlockId(memory_block_id BlockId) noexcept { m_BlockId = BlockId; }
		[[nodiscard]] inline memory_block_id GetMemoryBlockId() const noexcept { return m_BlockId; }

		[[nodiscard]] bool INL_IsAncestorOf(const SceneNode* pNode) const noexcept;
		[[nodiscard]] static SceneNode* INL_AsSceneNode(ISceneNode* pNode) noexcept;
		[[nodiscard]] static const SceneNode* INL_AsSceneNode(const ISceneNode* pNode) noexcept;

	private:
		std::atomic<int64_t> m_RefCount{ 0 };
		TMemoryBlockPool<SceneNode>* m_pPool = nullptr;
		memory_block_id m_BlockId{};
		uint32_t m_NodeID = 0;
		Scene* m_pScene = nullptr;
		char m_Name[64] = { 0 };
		ISceneNode* m_pParent = nullptr;
		mtl::fixed_vector<ISceneNode*, 16> m_lstChildren;
		mtl::fixed_vector<ISceneObject*, 8> m_lstObjects;
		Transform m_Transform;
		bool m_Enabled = true;
		bool m_Dirty = true;
	};
}
