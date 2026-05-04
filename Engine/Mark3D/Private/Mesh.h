#pragma once


namespace mark
{
	class PrimitiveBuffer;

	class Mesh final : public IMesh
	{
		constexpr static size_t MAX_SUBSET = 8;

		struct Primitive
		{
			PRIMITIVE_TYPE PrimitiveType;
			uint32_t VertexStart;
			uint32_t IndexStart;
			uint32_t VertexCount;
			uint32_t IndexCount;
			uint32_t NumSubsets;
			uint32_t SubsetIndexCounts[MAX_SUBSET];
			uint32_t MaterialIndex;
		};

	public:
		Mesh();

		void AddRef();
		void Release();

		bool Create(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount);

		int32_t AddPrimitive(uint32_t VertexCount, uint32_t IndexCount);
		int32_t AddPrimitive(uint32_t VertexCount, uint32_t* IndexCountArray, size_t NumIndexCountArray);
		uint32_t GetNumPrimitives() const noexcept;

		bool UpdateVertex(VERTEX_FORMAT VertexFormat, const void* pVertexData, size_t DataSize);
		bool UpdateIndex(const void* pIndexData, size_t DataSize);

	private:
		virtual ~Mesh() noexcept;

	private:
		mutable std::atomic<uint64_t> m_RefCount{ 1 };
		sys_vector<Primitive> m_lstPrimitives;

		PrimitiveBuffer* m_pPrimitiveBuffers = nullptr;

	};
}
