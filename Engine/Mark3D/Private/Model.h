#pragma once


namespace mark
{
	class Model final : public IModel
	{
	public:
		void AddRef();
		void Release();

		int32_t AddPrimitive(uint32_t VertexCount, uint32_t IndexCount);
		int32_t AddPrimitive(uint32_t VertexCount, uint32_t* IndexCountArray, size_t NumIndexCountArray);

		int32_t UpdateVeretx(int32_t PrimitiveIndex, VERTEX_FORMAT VertexFormat, const void* pVertexData, size_t DataSize);
		int32_t UpdateIndex(int32_t PrimitiveIndex, INDEX_FORMAT IndexFormat, const void* pIndexData, size_t DataSize);

	private:
		virtual ~Model() noexcept;

	private:
		mutable std::atomic<uint32_t> m_RefCount{ 1 };

		PrimitiveBuffer* m_pPrimitiveBuffers = nullptr;

	};
}
