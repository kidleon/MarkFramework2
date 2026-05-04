#pragma once


namespace mark
{
	struct IMesh : public Unknown
	{
		// 모델 관련 인터페이스 메서드 선언
		virtual int32_t AddPrimitive(uint32_t VertexCount, uint32_t IndexCount) = 0;
		virtual int32_t AddPrimitive(uint32_t VertexCount, uint32_t* IndexCountArray, size_t NumIndexCountArray) = 0;
		virtual uint32_t GetNumPrimitives() const noexcept = 0;

		virtual bool UpdateVertex(VERTEX_FORMAT VertexFormat, const void* pVertexData, size_t DataSize) = 0;
		virtual bool UpdateIndex(const void* pIndexData, size_t DataSize) = 0;

	};

	struct ISurfaceMaterial : public Unknown
	{
		virtual int32_t AddPass(const char* szPassName) = 0;
		virtual uint32_t GetNumPasses() const noexcept = 0;

		virtual void SetVertexShader(int32_t Pass, IShaderProgram* pVertexShader) = 0;
		virtual void SetPixelShader(int32_t Pass, IShaderProgram* pPixelShader) = 0;

	};

	struct IModel : public Unknown
	{
		// 모델 인스턴스 관련 인터페이스 메서드 선언
		virtual IMesh* CreateMesh(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount) = 0;
		virtual void AttachMesh(IMesh* pMesh) = 0;

		virtual int32_t AddMaterialSlot(const char* szSlotName) = 0;
		virtual void SetMaterialToSlot(int32_t SlotIndex, ISurfaceMaterial* pMaterial) = 0;
		virtual void SetMaterialToSlot(const char* szSlotName, ISurfaceMaterial* pMaterial) = 0;
	};
}
