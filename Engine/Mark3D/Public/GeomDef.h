#pragma once
#include "Transform.h"
#include "unknown_ptr.h"


namespace mark
{
	enum class GPU_BUFFER_LAYOUT : uint32_t
	{
		MERGED,
		SEPARATE,
	};


	struct IModelAsset;

	enum class GPUGEOMETRY_CREATE_FLAGS : uint32_t
	{
		ASYNC_LOAD = 0x1,
		GENERATE_TANGENT = 0x2,
		HAS_MODEL_ASSET = 0x4,
	};

	struct GPUGeometryCreateDesc
	{
		unknown_ptr<IModelAsset> pModelAsset;
		GPU_BUFFER_LAYOUT BufferLayout = GPU_BUFFER_LAYOUT::MERGED;
		uint32_t VertexFormats = 0;
		uint32_t CreateFlags = 0;
	};

	struct IResource : public Unknown
	{
		[[nodiscard]] virtual bool IsLoaded() const noexcept = 0;
	};

	struct IGPUGeometry : public IResource
	{};

	struct ISceneObject;

	struct ISceneNode : public PrivateUnknown
	{
		virtual void SetParent(ISceneNode* pParent, bool KeepWorldTransform = true) noexcept = 0;
		[[nodiscard]] virtual ISceneNode* GetParent() const noexcept = 0;

		virtual bool AttachChild(ISceneNode* pChild, bool KeepWorldTransform = true) noexcept = 0;
		virtual bool DetachChild(ISceneNode* pChild, bool KeepWorldTransform = true) noexcept = 0;
		virtual bool DetachChildAt(uint32_t ChildIndex, bool KeepWorldTransform = true) noexcept = 0;
		virtual void DetachAllChildren(bool KeepWorldTransform = true) noexcept = 0;

		[[nodiscard]] virtual uint32_t GetChildCount() const noexcept = 0;
		[[nodiscard]] virtual ISceneNode* GetChild(uint32_t ChildIndex) const noexcept = 0;
		[[nodiscard]] virtual int32_t GetChildIndex(const ISceneNode* pChild) const noexcept = 0;

		[[nodiscard]] virtual Transform& GetTransform() noexcept = 0;
		[[nodiscard]] virtual const Transform& GetTransform() const noexcept = 0;

		virtual bool AttachObject(ISceneObject* pObject) noexcept = 0;
		virtual bool DetachObject(ISceneObject* pObject) noexcept = 0;
		virtual bool DetachObjectAt(uint32_t ObjectIndex) noexcept = 0;
		virtual void DetachAllObjects() noexcept = 0;

		[[nodiscard]] virtual uint32_t GetObjectCount() const noexcept = 0;
		[[nodiscard]] virtual ISceneObject* GetObject(uint32_t ObjectIndex) const noexcept = 0;
		[[nodiscard]] virtual int32_t GetObjectIndex(const ISceneObject* pObject) const noexcept = 0;

		virtual void SetEnabled(bool Enabled) noexcept = 0;
		[[nodiscard]] virtual bool IsEnabled() const noexcept = 0;
		
	};

	struct ISceneObject : public IResource
	{
		virtual void SetEnable(bool Enable) noexcept = 0;
		[[nodiscard]] virtual bool IsEnable() const noexcept = 0;

		virtual void SetVisible(bool Visible) noexcept = 0;
		[[nodiscard]] virtual bool IsVisible() const noexcept = 0;

		[[nodiscard]] virtual ISceneNode* GetSceneNode() const noexcept = 0;
	};

	struct IModelInstance : public ISceneObject
	{
	};

	struct ISkeleton : public IResource
	{
	};

	struct ISkeletonPose : public Unknown
	{
	};

	struct ISkinnedModelInstance : public IModelInstance
	{
	};


	struct IWorld;

	struct IScene : public Unknown
	{
		virtual void SetName(const char* Name) noexcept = 0;
		[[nodiscard]] virtual const char* GetName() const noexcept = 0;

		virtual void SetOrder(int32_t Order) noexcept = 0;
		[[nodiscard]] virtual int32_t GetOrder() const noexcept = 0;

		virtual void SetActive(bool Active) noexcept = 0;
		[[nodiscard]] virtual bool IsActive() const noexcept = 0;

		virtual void SetVisible(bool Visible) noexcept = 0;
		[[nodiscard]] virtual bool IsVisible() const noexcept = 0;

		virtual IWorld* GetWorld() const noexcept = 0;

		virtual ISceneNode* CreateSceneNode() noexcept = 0;
		virtual void DestroySceneNode(ISceneNode* pNode) noexcept = 0;

		virtual int32_t GetNumSceneNode() const noexcept = 0;
		virtual ISceneNode* GetSceneNode(const char* Name) const noexcept = 0;
		virtual ISceneNode* GetSceneNode(uint32_t NodeID) const noexcept = 0;
		virtual ISceneNode* GetSceneNodeByIndex(int32_t Index) const noexcept = 0;
		virtual ISceneNode* GetRootSceneNode() const noexcept = 0;
		
	};


	struct IWorld : public Unknown
	{
		/**
		* @brief 월드를 생성 합니다. 월드는 씬을 포함하는 최상위 컨테이너입니다. 일반적으로 게임에서 하나의 월드가 존재하며, 여러 씬을 관리할 수 있습니다.
		* @param Name 월드의 이름을 지정합니다. 이 이름은 월드를 식별하는 데 사용됩니다.
		*/
		virtual void BigBang(const char* Name) noexcept = 0;

		/**
		 * @brief 월드를 파괴 합니다. 월드가 파괴되면 해당 월드에 포함된 모든 씬과 씬 노드, 씬 오브젝트도 함께 파괴됩니다. 이 메서드는 월드의 생명주기를 관리하는 데 사용됩니다.
		 * @param Name 
		 */
		virtual void BigRip(const char* Name) noexcept = 0;

		[[nodiscard]] virtual IScene* CreateScene(const char* Name) noexcept = 0;
		[[nodiscard]] virtual IScene* LoadScene(const char* FilePath, bool Additive, bool Async) noexcept = 0;
		virtual void DestroyScene(const char* Name) noexcept = 0;
		[[nodiscard]] virtual IScene* GetScene(const char* Name) const noexcept = 0;

	};

}
