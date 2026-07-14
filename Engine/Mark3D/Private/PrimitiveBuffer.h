#pragma once


namespace mark
{
	class RenderSystem;

	class PrimitiveBuffer
	{
	public:
		~PrimitiveBuffer() noexcept;

		bool Create(
			RenderSystem* pRenderSystem,
			uint32_t VertexFormats,
			uint32_t VertexCount,
			INDEX_FORMAT IndexFormat,
			uint32_t IndexCount
		);

		void Cleanup();

		bool UpdateVertexDataImmediate(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize);
		bool UpdateVertexData(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize);

		bool UpdateIndexDataImmediate(const void* pData, size_t DataSize);
		bool UpdateIndexData(const void* pData, size_t DataSize);

		// 구간 공유용 부분 업데이트. StartVertex/StartIndex는 요소(정점/인덱스) 단위 오프셋.
		bool UpdateVertexDataAtVertex(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize, uint32_t StartVertex);
		bool UpdateIndexDataAtIndex(const void* pData, size_t DataSize, uint32_t StartIndex);

		inline uint32_t INL_GetVertexFormat() const { return m_VertexFormat; }
		inline bool INL_HasVertexFormat(VERTEX_FORMAT VertexFormat) const { return (m_VertexFormat & static_cast<uint32_t>(VertexFormat)) != 0; }

		inline uint32_t INL_GetVertexCount() const { return m_VertexCount; }
		inline uint32_t INL_GetIndexCount() const { return m_IndexCount; }

		inline INDEX_FORMAT INL_GetIndexFormat() const { return m_IndexFormat; }
		inline uint32_t INL_GetIndexStride() const { return (m_IndexFormat == INDEX_FORMAT::UINT32) ? sizeof(uint32_t) : sizeof(uint16_t); }

	private:
		uint32_t m_VertexFormat = 0;
		uint32_t m_VertexCount = 0;

		INDEX_FORMAT m_IndexFormat = INDEX_FORMAT::UINT16;
		uint32_t m_IndexCount = 0;

		unknown_ptr<IGPUBuffer> m_pVBs[(int)VERTEX_FORMAT_INDEX::EMAX] = {};
		unknown_ptr<IGPUBuffer> m_pIB = nullptr;

	};

}
