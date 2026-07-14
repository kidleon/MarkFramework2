#pragma once
#include "PrimitiveBuffer.h"


namespace mark
{
	class Model : public IModel
	{
	public:
		Model(PrimitiveBuffer* pPrimitiveBuffer);

		virtual void AddRef() override;
		virtual void Release() override;

		virtual bool IsLoaded() const noexcept override;

		virtual void SetEnable(bool Enable) noexcept final;
		virtual bool IsEnable() const noexcept final;

		virtual void SetVisible(bool Visible) noexcept final;
		virtual bool IsVisible() const noexcept final;

		virtual ISceneNode* GetSceneNode() const noexcept override;

		virtual int32_t CreateMesh(
			PRIMITIVE_TYPE PrimitiveType,
			uint32_t VertexFormats,
			uint32_t VertexSize,
			uint32_t IndexSize
		) override;

		virtual int32_t CreateMesh(
			NameHash MeshName,
			PRIMITIVE_TYPE PrimitiveType,
			uint32_t VertexFormats,
			uint32_t VertexSize,
			uint32_t IndexSize
		) override;

		virtual int32_t CreateSubMesh(
			int32_t MeshIndex,
			uint32_t VertexSize,
			uint32_t IndexSize
		) override;

		virtual int32_t CreateSubMesh(
			NameHash MeshName,
			uint32_t VertexSize,
			uint32_t IndexSize
		) override;

		virtual void SetVisible(
			int32_t MeshIndex,
			bool Visible
		) noexcept override;

		virtual void SetVisible(
			NameHash MeshName,
			bool Visible
		) noexcept override;

		virtual void UpdateVertex(
			int32_t MeshIndex,
			VERTEX_FORMAT VertexFormat,
			const void* pData,
			size_t DataSize
		) override;

		virtual void UpdateVertex(
			NameHash MeshName,
			VERTEX_FORMAT VertexFormat,
			const void* pData,
			size_t DataSize
		) override;

		virtual void UpdateIndex(
			int32_t MeshIndex,
			const void* pData,
			size_t DataSize
		) override;

		virtual void UpdateIndex(
			NameHash MeshName,
			const void* pData,
			size_t DataSize
		) override;

		virtual void UpdateIndex(
			int32_t MeshIndex,
			int32_t SubMeshIndex,
			const void* pData,
			size_t DataSize
		) override;

		virtual void UpdateIndex(
			NameHash MeshName,
			int32_t SubMeshIndex,
			const void* pData,
			size_t DataSize
		) override;

		virtual void LinkMeshToMaterial(
			int32_t MeshIndex,
			int32_t MaterialSlot
		) override;

		virtual void LinkMeshToMaterial(
			NameHash MeshName,
			int32_t MaterialSlot
		) override;

		virtual void LinkMeshToMaterial(
			int32_t MeshIndex,
			int32_t SubMeshIndex,
			int32_t MaterialSlot
		) override;

		virtual void LinkMeshToMaterial(
			NameHash MeshName,
			int32_t SubMeshIndex,
			int32_t MaterialSlot
		) override;

		virtual void SetSurfaceMaterial(
			int32_t MaterialSlot,
			ISurfaceMaterial* pMaterial
		) override;

		[[nodiscard]] virtual ISurfaceMaterial* GetSurfaceMaterial(
			int32_t MaterialSlot
		) const noexcept override;

		[[nodiscard]] virtual int32_t GetMeshIndexByName(
			NameHash MeshName
		) const noexcept override;

		[[nodiscard]] virtual int32_t GetNumSubMesh() const noexcept override;

		[[nodiscard]] virtual int32_t GetNumSubMeshByName(
			NameHash MeshName
		) const noexcept override;

		[[nodiscard]] virtual int32_t GetNumSubMeshByIndex(
			int32_t MeshIndex
		) const noexcept override;

		[[nodiscard]] virtual int32_t GetMaterialSlotCount() const override;

	private:
		virtual ~Model() noexcept;

		static constexpr uint32_t MAX_MESH = 8;
		static constexpr uint32_t MAX_SUB_MESH = 8;
		static constexpr uint32_t MAX_MATERIAL_SLOT = 8;

		struct SUB_MESH
		{
			uint32_t MaterialSlot;
			uint32_t StartIndex;
			uint32_t IndexCount;
		};

		struct MESH
		{
			NameHash Name;
			PRIMITIVE_TYPE PrimitiveType;

			uint32_t VertexFormats;
			uint32_t ReservedVertexStart;
			uint32_t ReservedIndexStart;
			uint32_t ReservedVertexCount;
			uint32_t ReservedIndexCount;

			uint32_t VertexCount;
			uint32_t IndexCount;

			int32_t MaterialSlot;

			BOOL Visible;

			mtl::fixed_vector<SUB_MESH, MAX_SUB_MESH> SubMeshes;
		};

		[[nodiscard]] MESH* GetMesh(int32_t MeshIndex) noexcept;
		[[nodiscard]] MESH* GetMesh(NameHash MeshName) noexcept;

	private:
		std::atomic<int32_t> m_RefCount{ 1 };
		std::atomic<BOOL> m_Loaded{ FALSE };

		BOOL m_Enable = TRUE;
		BOOL m_Visible = TRUE;
		PrimitiveBuffer* m_pPrimitiveBuffer = nullptr;
		ISceneNode* m_pSceneNode = nullptr;

		uint32_t m_ReservedVertexCursor = 0;
		uint32_t m_ReservedIndexCursor = 0;

		mtl::fixed_vector<MESH, MAX_MESH> m_lstMeshes;
		mtl::fixed_vector<ISurfaceMaterial*, MAX_MATERIAL_SLOT> m_lstMaterials;

	};
}
