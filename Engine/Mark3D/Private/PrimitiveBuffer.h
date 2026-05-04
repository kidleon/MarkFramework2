#pragma once


namespace mark
{
	class PrimitiveBuffer
	{
	public:
		~PrimitiveBuffer() noexcept;

		bool Create(uint32_t VertexFormats, uint32_t VertexCount, INDEX_FORMAT IndexFormat, uint32_t IndexCount);

		bool UpdateVertexDataImmediate(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize);
		bool UpdateVertexData(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize);

		bool UpdateIndexDataImmediate(const void* pData, size_t DataSize);
		bool UpdateIndexData(const void* pData, size_t DataSize);

		inline uint32_t INL_GetVertexFormat() const { return m_VertexFormat; }
		inline bool INL_HasVertexFormat(VERTEX_FORMAT VertexFormat) const { return (m_VertexFormat & static_cast<uint32_t>(VertexFormat)) != 0; }

		inline uint32_t INL_GetVertexCount() const { return m_VertexCount; }
		inline uint32_t INL_GetIndexCount() const { return m_IndexCount; }

	private:
		void Cleanup();

	private:
		uint32_t m_VertexFormat = 0;
		uint32_t m_VertexCount = 0;

		INDEX_FORMAT m_IndexFormat = INDEX_FORMAT::UINT16;
		uint32_t m_IndexCount = 0;

		unknown_ptr<IGPUBuffer> m_pVBs[(int)VERTEX_FORMAT_INDEX::EMAX] = {};
		unknown_ptr<IGPUBuffer> m_pIB = nullptr;

	};

}
