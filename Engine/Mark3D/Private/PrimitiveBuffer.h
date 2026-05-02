#pragma once


namespace mark
{
	class PrimitiveBuffer
	{
	public:
		~PrimitiveBuffer() noexcept;

		void UpdateVertexDataImmediate(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize);
		void UpdateVertexData(VERTEX_FORMAT VertexFormat, const void* pData, size_t DataSize);

		void UpdateIndexDataImmediate(INDEX_FORMAT IndexFormat, const void* pData, size_t DataSize);
		void UpdateIndexData(INDEX_FORMAT IndexFormat, const void* pData, size_t DataSize);

		inline uint32_t INL_GetVertexFormat() const { return m_VertexFormat; }
		inline bool INL_HasVertexFormat(VERTEX_FORMAT VertexFormat) const { return (m_VertexFormat & static_cast<uint32_t>(VertexFormat)) != 0; }

	private:
		uint32_t m_VertexFormat = 0;

		IGPUBuffer* m_pVBs[(int)VERTEX_FORMAT_INDEX::MAX] = {};
		IGPUBuffer* m_pIB = nullptr;

	};

}
