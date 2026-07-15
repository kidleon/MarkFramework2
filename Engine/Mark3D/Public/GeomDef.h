#pragma once
#include "Transform.h"
#include "unknown_ptr.h"


namespace mark
{
	enum class MODEL_LAYOUT : uint32_t
	{
		MERGED, // 모든 메쉬가 단일 PrimitiveBuffer를 공유하는 병합(MERGED) 레이아웃
		SEPARATE, // 각 메쉬가 독립적인 PrimitiveBuffer를 가지는 분리(SEPARATE) 레이아웃
	};


	struct IModelAsset;


	struct IResource : public Unknown
	{
		[[nodiscard]] virtual bool IsLoaded() const noexcept = 0;
	};

	
	struct ISceneNode;
	struct ISurfaceMaterial;

	struct ModelCreateDesc
	{
		uint32_t VertexBufferSize = 0;
		uint32_t IndexBufferSize = 0;
		uint32_t VertexFormats = 0;
		INDEX_FORMAT IndexFormat = INDEX_FORMAT::UINT16;
	};

	struct ISceneObject : public IResource
	{
		virtual void SetEnable(bool Enable) noexcept = 0;
		[[nodiscard]] virtual bool IsEnable() const noexcept = 0;

		virtual void SetVisible(bool Visible) noexcept = 0;
		[[nodiscard]] virtual bool IsVisible() const noexcept = 0;

		[[nodiscard]] virtual ISceneNode* GetSceneNode() const noexcept = 0;
	};

	struct IModel : public ISceneObject
	{
		virtual int32_t CreateMesh(
			PRIMITIVE_TYPE PrimitiveType,
			uint32_t VertexFormats,
			uint32_t VertexSize,
			uint32_t IndexSize
		) = 0;

		virtual int32_t CreateMesh(
			NameHash MeshName,
			PRIMITIVE_TYPE PrimitiveType,
			uint32_t VertexFormats,
			uint32_t VertexSize,
			uint32_t IndexSize
		) = 0;

		virtual int32_t CreateSubMesh(
			int32_t MeshIndex,
			uint32_t VertexSize,
			uint32_t IndexSize
		) = 0;

		virtual int32_t CreateSubMesh(
			NameHash MeshName,
			uint32_t VertexSize,
			uint32_t IndexSize
		) = 0;

		virtual void SetVisible(
			int32_t MeshIndex,
			bool Visible
		) noexcept = 0;

		virtual void SetVisible(
			NameHash MeshName,
			bool Visible
		) noexcept = 0;

		virtual void UpdateVertex(
			int32_t MeshIndex,
			VERTEX_FORMAT VertexFormat,
			const void* pData,
			size_t DataSize
		) = 0;

		virtual void UpdateVertex(
			NameHash MeshName,
			VERTEX_FORMAT VertexFormat,
			const void* pData,
			size_t DataSize
		) = 0;

		virtual void UpdateIndex(
			int32_t MeshIndex,
			const void* pData,
			size_t DataSize
		) = 0;

		virtual void UpdateIndex(
			NameHash MeshName,
			const void* pData,
			size_t DataSize
		) = 0;

		virtual void UpdateIndex(
			int32_t MeshIndex,
			int32_t SubMeshIndex,
			const void* pData,
			size_t DataSize
		) = 0;

		virtual void UpdateIndex(
			NameHash MeshName,
			int32_t SubMeshIndex,
			const void* pData,
			size_t DataSize
		) = 0;

		virtual void LinkMeshToMaterial(
			int32_t MeshIndex,
			int32_t MaterialSlot
		) = 0;

		virtual void LinkMeshToMaterial(
			NameHash MeshName,
			int32_t MaterialSlot
		) = 0;

		virtual void LinkMeshToMaterial(
			int32_t MeshIndex,
			int32_t SubMeshIndex,
			int32_t MaterialSlot
		) = 0;

		virtual void LinkMeshToMaterial(
			NameHash MeshName,
			int32_t SubMeshIndex,
			int32_t MaterialSlot
		) = 0;

		virtual void SetSurfaceMaterial(
			int32_t MaterialSlot,
			ISurfaceMaterial* pMaterial
		) = 0;

		[[nodiscard]] virtual ISurfaceMaterial* GetSurfaceMaterial(
			int32_t MaterialSlot
		) const noexcept = 0;

		[[nodiscard]] virtual int32_t GetMeshIndexByName(
			NameHash MeshName
		) const noexcept = 0;

		[[nodiscard]] virtual int32_t GetNumSubMesh() const noexcept = 0;

		[[nodiscard]] virtual int32_t GetNumSubMeshByName(
			NameHash MeshName
		) const noexcept = 0;

		[[nodiscard]] virtual int32_t GetNumSubMeshByIndex(
			int32_t MeshIndex
		) const noexcept = 0;

		[[nodiscard]] virtual int32_t GetMaterialSlotCount() const = 0;
	};

	struct ISkeleton : public IResource
	{
	};

	struct ISkeletonPose : public Unknown
	{
	};


	struct IWorld;
	struct IScene;

	struct ISceneNode : public PrivateUnknown
	{
		virtual void SetName(const char* Name) noexcept = 0;
		[[nodiscard]] virtual const char* GetName() const noexcept = 0;
		[[nodiscard]] virtual uint32_t GetNodeID() const noexcept = 0;
		[[nodiscard]] virtual IScene* GetScene() const noexcept = 0;

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

		virtual void SetDirty(bool Dirty) noexcept = 0;
		[[nodiscard]] virtual bool IsDirty() const noexcept = 0;

	};

	struct IScene : public PrivateUnknown
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


	struct IWorld : public PrivateUnknown
	{
		virtual void BigBang(const char* Name) noexcept = 0;
		virtual void BigRip() noexcept = 0;

		[[nodiscard]] virtual IScene* CreateScene(const char* Name) noexcept = 0;
		[[nodiscard]] virtual IScene* LoadScene(const char* FilePath, bool Additive, bool Async) noexcept = 0;
		virtual void DestroyScene(const char* Name) noexcept = 0;
		virtual void DestroyScene(IScene* pScene) noexcept = 0;

		[[nodiscard]] virtual IScene* GetScene(const char* Name) const noexcept = 0;

	};

}
