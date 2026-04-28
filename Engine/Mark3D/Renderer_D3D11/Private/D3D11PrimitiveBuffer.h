#pragma once


namespace mark
{
	struct PrimitiveBuffer
	{
		struct SubMesh
		{
			uint32_t IndexStart;
			uint32_t IndexCount;
			uint32_t MaterialID;
		};

		struct Primitive
		{
			PRIMITIVE_TYPE PrimitiveType;
			uint32_t VertexStart;
			uint32_t VertexCount;
			uint32_t IndexStart;
			uint32_t IndexCount;
			sys_vector<SubMesh> SubMeshes; // 서브메쉬 정보
		};

		sys_vector<Primitive> lstPrimitives; // 프리미티브 정보
	};

	struct D3D11PrimitiveBuffer
	{
		sys_vector<D3D11Buffer*> m_pBindVBs; // 바인딩된 버텍스 버퍼들
		D3D11Buffer* m_pBindIB = nullptr; // 바인딩된 인덱스 버퍼
	};
}
