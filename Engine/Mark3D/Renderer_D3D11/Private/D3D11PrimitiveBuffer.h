#pragma once


namespace mark
{
	struct D3D11Buffer;

	class D3D11PrimitiveBuffer
	{
		static constexpr int32_t MAX_PRIMITIVES = 8;
		static constexpr int32_t MAX_SUB_MESH = 8; // 하나의 프리미티브에 최대 4개의 인덱스 버퍼 (서브메시)
		static constexpr uint32_t MAX_BIND_VB = (uint32_t)VERTEX_FORMAT_INDEX::MAX;

	public:
		D3D11PrimitiveBuffer() = default;
		~D3D11PrimitiveBuffer();

		void Reset();
		int32_t AddPrimitive(PRIMITIVE_TYPE PrimitiveType, uint32_t VertexCount, uint32_t IndexCount);
		inline uint32_t GetNumPrimitives() const { return m_NumPrimitives; }

		bool UpdatePosition(int32_t PrimitiveIndex, const FLOAT3* pPositions, uint32_t NumPosition);
		bool UpdateNormal(int32_t PrimitiveIndex, const FLOAT3* pNormals, uint32_t NumNormal);
		bool UpdateColor(int32_t PrimitiveIndex, const FLOAT4* pColors, uint32_t NumColor);
		bool UpdateTangent(int32_t PrimitiveIndex, const FLOAT3* pTangents, uint32_t NumTangent);
		bool UpdateTexCoord(int32_t PrimitiveIndex, int32_t TexCoordIndex, const FLOAT2* pTexCoords, uint32_t NumTexCoord);
		bool UpdateIndex(int32_t PrimitiveIndex, void* pIndexData, uint32_t NumIndices, uint32_t IndexStride);
		bool UpdateSubMeshIndex(int32_t PrimitiveIndex, uint32_t SubMeshIndex, void* pIndexData, uint32_t NumIndices, uint32_t IndexStride);

	private:
		struct PRIMITIVE_DESC
		{
			PRIMITIVE_TYPE PrimitiveType;
			uint32_t VertexStart;
			uint32_t VertexCount;
			uint32_t IndexStart;
			uint32_t IndexCount;

			uint32_t NumIndices;
			uint32_t IndexStarts[MAX_SUB_MESH];
			uint32_t IndexCounts[MAX_SUB_MESH];
		};

		uint32_t m_MaxVertexCount = 0;
		uint32_t m_MaxIndexCount = 0;
		uint32_t m_VertexFormat = 0;

		uint32_t m_NumPrimitives = 0;
		PRIMITIVE_DESC m_Primitives[MAX_PRIMITIVES] = {};

		uint32_t m_CurrentVertexCount = 0;
		uint32_t m_CurrentIndexCount = 0;

		D3D11Buffer* m_pBindVBs[MAX_BIND_VB] = { nullptr };

	};
}
