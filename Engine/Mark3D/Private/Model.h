#pragma once


namespace mark
{
	class PrimitiveBuffer;

	class Model : public IModel
	{
	public:
		Model(PrimitiveBuffer* pPrimitiveBuffer);

		virtual void SetEnable(bool Enable) noexcept override;
		virtual bool IsEnable() const noexcept override;

		virtual void SetVisible(bool Visible) noexcept override;
		virtual bool IsVisible() const noexcept override;

		virtual ISceneNode* GetSceneNode() const noexcept override;

		virtual int32_t AddPrimitive(
			PRIMITIVE_TYPE PrimitiveType,
			uint32_t VertexCount,
			uint32_t IndexCount,
			uint32_t MaterialSlot
		) noexcept override;

		virtual int32_t GetPrimitiveCount() const noexcept override;
		virtual void ClearPrimitive() noexcept override;

		virtual bool UpdateVertexData(
			int32_t PrimitiveIndex,
			uint32_t VertexFormat,
			const void* pData,
			size_t DataSize
		) noexcept override;

		virtual bool UpdateIndexData(
			int32_t PrimitiveIndex,
			const void* pData,
			size_t DataSize
		) noexcept override;

		virtual void SetSurfaceMaterial(
			uint32_t MaterialSlot,
			ISurfaceMaterial* pMaterial
		) noexcept override;

		[[nodiscard]] virtual ISurfaceMaterial* GetSurfaceMaterial(
			uint32_t MaterialSlot
		) const noexcept override;

	private:
		Model() = delete;
		virtual ~Model() noexcept;

	private:
		PrimitiveBuffer* m_pPrimitiveBuffer = nullptr;

	};
}
