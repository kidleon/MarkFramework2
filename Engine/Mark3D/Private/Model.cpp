#include "pch.h"
#include "Model.h"
#include "PrimitiveBuffer.h"
#include "RenderSystem.h"
#include "GPUGeometry.h"


namespace mark
{
	Model::Model(PrimitiveBuffer* pPrimitiveBuffer)
		: m_pPrimitiveBuffer(pPrimitiveBuffer)
	{
	}

	Model::~Model() noexcept
	{
		for (ISurfaceMaterial* pMaterial : m_lstMaterials)
		{
			if (pMaterial)
				pMaterial->Release();
		}
		m_lstMaterials.clear();

		CORE_DELETE(PrimitiveBuffer, m_pPrimitiveBuffer);
	}

	void Model::AddRef()
	{
		m_RefCount.fetch_add(1, std::memory_order_relaxed);
	}

	void Model::Release()
	{
		if (m_RefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			CORE_DELETE(Model, this);
		}
	}

	bool Model::IsLoaded() const noexcept
	{
		return m_Loaded.load(std::memory_order_acquire) == TRUE;
	}

	void Model::SetEnable(bool Enable) noexcept
	{
		m_Enable = Enable;
	}

	bool Model::IsEnable() const noexcept
	{
		return m_Enable;
	}

	void Model::SetVisible(bool Visible) noexcept
	{
		m_Visible = Visible;
	}

	bool Model::IsVisible() const noexcept
	{
		return m_Visible;
	}

	ISceneNode* Model::GetSceneNode() const noexcept
	{
		return m_pSceneNode;
	}

	Model::MESH* Model::GetMesh(int32_t MeshIndex) noexcept
	{
		if (MeshIndex < 0 || static_cast<uint32_t>(MeshIndex) >= m_lstMeshes.size())
			return nullptr;

		return &m_lstMeshes[MeshIndex];
	}

	Model::MESH* Model::GetMesh(NameHash MeshName) noexcept
	{
		for (MESH& Mesh : m_lstMeshes)
		{
			if (Mesh.Name == MeshName)
				return &Mesh;
		}

		return nullptr;
	}

	int32_t Model::CreateMesh(
		PRIMITIVE_TYPE PrimitiveType,
		uint32_t VertexFormats,
		uint32_t VertexSize,
		uint32_t IndexSize
	)
	{
		return CreateMesh(NameHash(), PrimitiveType, VertexFormats, VertexSize, IndexSize);
	}

	int32_t Model::CreateMesh(
		NameHash MeshName,
		PRIMITIVE_TYPE PrimitiveType,
		uint32_t VertexFormats,
		uint32_t VertexSize,
		uint32_t IndexSize
	)
	{
		if (!m_pPrimitiveBuffer)
			return -1;

		if (m_lstMeshes.size() >= MAX_MESH)
		{
			SYS_LOG_ERR("Model::CreateMesh - reached MAX_MESH limit.");
			return -1;
		}

		if (VertexFormats == 0 || VertexSize == 0)
		{
			SYS_LOG_ERR("Model::CreateMesh - invalid vertex format or vertex count.");
			return -1;
		}

		// 이 메쉬가 요구하는 포맷은 PrimitiveBuffer 가 실제로 보유한 포맷의 부분집합이어야 함.
		if ((VertexFormats & m_pPrimitiveBuffer->INL_GetVertexFormat()) != VertexFormats)
		{
			SYS_LOG_ERR("Model::CreateMesh - vertex formats are not provided by the PrimitiveBuffer.");
			return -1;
		}

		// PrimitiveBuffer 예약 용량 검증.
		if (m_ReservedVertexCursor + VertexSize > m_pPrimitiveBuffer->INL_GetVertexCount())
		{
			SYS_LOG_ERR("Model::CreateMesh - not enough vertex capacity in PrimitiveBuffer.");
			return -1;
		}

		if (m_ReservedIndexCursor + IndexSize > m_pPrimitiveBuffer->INL_GetIndexCount())
		{
			SYS_LOG_ERR("Model::CreateMesh - not enough index capacity in PrimitiveBuffer.");
			return -1;
		}

		MESH Mesh = {};
		Mesh.Name = MeshName;
		Mesh.PrimitiveType = PrimitiveType;
		Mesh.VertexFormats = VertexFormats;
		Mesh.ReservedVertexStart = m_ReservedVertexCursor;
		Mesh.ReservedIndexStart = m_ReservedIndexCursor;
		Mesh.ReservedVertexCount = VertexSize;
		Mesh.ReservedIndexCount = IndexSize;
		Mesh.VertexCount = 0;
		Mesh.IndexCount = 0;
		Mesh.MaterialSlot = -1;
		Mesh.Visible = TRUE;

		m_ReservedVertexCursor += VertexSize;
		m_ReservedIndexCursor += IndexSize;

		const int32_t MeshIndex = static_cast<int32_t>(m_lstMeshes.size());
		m_lstMeshes.push_back(Mesh);

		return MeshIndex;
	}

	int32_t Model::CreateSubMesh(
		int32_t MeshIndex,
		uint32_t VertexSize,
		uint32_t IndexSize
	)
	{
		MESH* pMesh = GetMesh(MeshIndex);
		if (!pMesh)
			return -1;

		(void)VertexSize;

		if (pMesh->SubMeshes.size() >= MAX_SUB_MESH)
		{
			SYS_LOG_ERR("Model::CreateSubMesh - reached MAX_SUB_MESH limit.");
			return -1;
		}

		// 서브메쉬는 부모 메쉬의 인덱스 영역을 순차적으로 분할한다.
		uint32_t StartIndex = 0;
		for (const SUB_MESH& Sub : pMesh->SubMeshes)
			StartIndex += Sub.IndexCount;

		if (StartIndex + IndexSize > pMesh->ReservedIndexCount)
		{
			SYS_LOG_ERR("Model::CreateSubMesh - sub-mesh index range exceeds parent mesh reservation.");
			return -1;
		}

		SUB_MESH SubMesh = {};
		SubMesh.MaterialSlot = static_cast<uint32_t>(-1);
		SubMesh.StartIndex = pMesh->ReservedIndexStart + StartIndex;
		SubMesh.IndexCount = IndexSize;

		const int32_t SubMeshIndex = static_cast<int32_t>(pMesh->SubMeshes.size());
		pMesh->SubMeshes.push_back(SubMesh);

		return SubMeshIndex;
	}

	int32_t Model::CreateSubMesh(
		NameHash MeshName,
		uint32_t VertexSize,
		uint32_t IndexSize
	)
	{
		const int32_t MeshIndex = GetMeshIndexByName(MeshName);
		if (MeshIndex < 0)
			return -1;

		return CreateSubMesh(MeshIndex, VertexSize, IndexSize);
	}

	void Model::SetVisible(
		int32_t MeshIndex,
		bool Visible
	) noexcept
	{
		if (MESH* pMesh = GetMesh(MeshIndex))
			pMesh->Visible = Visible ? TRUE : FALSE;
	}

	void Model::SetVisible(
		NameHash MeshName,
		bool Visible
	) noexcept
	{
		if (MESH* pMesh = GetMesh(MeshName))
			pMesh->Visible = Visible ? TRUE : FALSE;
	}

	void Model::UpdateVertex(
		int32_t MeshIndex,
		VERTEX_FORMAT VertexFormat,
		const void* pData,
		size_t DataSize
	)
	{
		if (!m_pPrimitiveBuffer || !pData || DataSize == 0)
			return;

		MESH* pMesh = GetMesh(MeshIndex);
		if (!pMesh)
			return;

		if ((pMesh->VertexFormats & static_cast<uint32_t>(VertexFormat)) == 0)
		{
			SYS_LOG_ERR("Model::UpdateVertex - vertex format not declared for this mesh.");
			return;
		}

		const uint32_t Stride = GetVertexSizeFromFormat(VertexFormat);
		if (Stride == 0)
			return;

		// 이 메쉬가 예약한 정점 구간을 초과하지 않는지 검증.
		if (static_cast<uint32_t>(DataSize) / Stride > pMesh->ReservedVertexCount)
		{
			SYS_LOG_ERR("Model::UpdateVertex - data exceeds reserved vertex range.");
			return;
		}

		// 예약된 시작 정점 오프셋에 부분 업데이트.
		if (!m_pPrimitiveBuffer->UpdateVertexDataAtVertex(VertexFormat, pData, DataSize, pMesh->ReservedVertexStart))
			return;

		pMesh->VertexCount = static_cast<uint32_t>(DataSize) / Stride;
	}

	void Model::UpdateVertex(
		NameHash MeshName,
		VERTEX_FORMAT VertexFormat,
		const void* pData,
		size_t DataSize
	)
	{
		UpdateVertex(GetMeshIndexByName(MeshName), VertexFormat, pData, DataSize);
	}

	void Model::UpdateIndex(
		int32_t MeshIndex,
		const void* pData,
		size_t DataSize
	)
	{
		if (!m_pPrimitiveBuffer || !pData || DataSize == 0)
			return;

		MESH* pMesh = GetMesh(MeshIndex);
		if (!pMesh)
			return;

		const uint32_t Stride = m_pPrimitiveBuffer->INL_GetIndexStride();
		if (Stride == 0)
			return;

		// 예약한 인덱스 구간 초과 검증.
		if (static_cast<uint32_t>(DataSize) / Stride > pMesh->ReservedIndexCount)
		{
			SYS_LOG_ERR("Model::UpdateIndex - data exceeds reserved index range.");
			return;
		}

		// 예약된 시작 인덱스 오프셋에 부분 업데이트.
		if (!m_pPrimitiveBuffer->UpdateIndexDataAtIndex(pData, DataSize, pMesh->ReservedIndexStart))
			return;

		pMesh->IndexCount = static_cast<uint32_t>(DataSize) / Stride;
	}

	void Model::UpdateIndex(
		NameHash MeshName,
		const void* pData,
		size_t DataSize
	)
	{
		UpdateIndex(GetMeshIndexByName(MeshName), pData, DataSize);
	}

	void Model::UpdateIndex(
		int32_t MeshIndex,
		int32_t SubMeshIndex,
		const void* pData,
		size_t DataSize
	)
	{
		if (!m_pPrimitiveBuffer || !pData || DataSize == 0)
			return;

		MESH* pMesh = GetMesh(MeshIndex);
		if (!pMesh)
			return;

		if (SubMeshIndex < 0 || static_cast<uint32_t>(SubMeshIndex) >= pMesh->SubMeshes.size())
			return;

		const uint32_t Stride = m_pPrimitiveBuffer->INL_GetIndexStride();
		if (Stride == 0)
			return;

		SUB_MESH& SubMesh = pMesh->SubMeshes[SubMeshIndex];

		// 서브메쉬가 예약한 인덱스 구간 초과 검증.
		if (static_cast<uint32_t>(DataSize) / Stride > SubMesh.IndexCount)
		{
			SYS_LOG_ERR("Model::UpdateIndex - data exceeds sub-mesh index range.");
			return;
		}

		// 서브메쉬의 절대 시작 인덱스 오프셋에 부분 업데이트.
		if (!m_pPrimitiveBuffer->UpdateIndexDataAtIndex(pData, DataSize, SubMesh.StartIndex))
			return;

		SubMesh.IndexCount = static_cast<uint32_t>(DataSize) / Stride;
	}

	void Model::UpdateIndex(
		NameHash MeshName,
		int32_t SubMeshIndex,
		const void* pData,
		size_t DataSize
	)
	{
		UpdateIndex(GetMeshIndexByName(MeshName), SubMeshIndex, pData, DataSize);
	}

	void Model::LinkMeshToMaterial(
		int32_t MeshIndex,
		int32_t MaterialSlot
	)
	{
		if (MESH* pMesh = GetMesh(MeshIndex))
			pMesh->MaterialSlot = MaterialSlot;
	}

	void Model::LinkMeshToMaterial(
		NameHash MeshName,
		int32_t MaterialSlot
	)
	{
		if (MESH* pMesh = GetMesh(MeshName))
			pMesh->MaterialSlot = MaterialSlot;
	}

	void Model::LinkMeshToMaterial(
		int32_t MeshIndex,
		int32_t SubMeshIndex,
		int32_t MaterialSlot
	)
	{
		MESH* pMesh = GetMesh(MeshIndex);
		if (!pMesh)
			return;

		if (SubMeshIndex < 0 || static_cast<uint32_t>(SubMeshIndex) >= pMesh->SubMeshes.size())
			return;

		pMesh->SubMeshes[SubMeshIndex].MaterialSlot = static_cast<uint32_t>(MaterialSlot);
	}

	void Model::LinkMeshToMaterial(
		NameHash MeshName,
		int32_t SubMeshIndex,
		int32_t MaterialSlot
	)
	{
		LinkMeshToMaterial(GetMeshIndexByName(MeshName), SubMeshIndex, MaterialSlot);
	}

	void Model::SetSurfaceMaterial(
		int32_t MaterialSlot,
		ISurfaceMaterial* pMaterial
	)
	{
		if (MaterialSlot < 0 || static_cast<uint32_t>(MaterialSlot) >= MAX_MATERIAL_SLOT)
		{
			SYS_LOG_ERR("Model::SetSurfaceMaterial - material slot out of range.");
			return;
		}

		while (m_lstMaterials.size() <= static_cast<uint32_t>(MaterialSlot))
			m_lstMaterials.push_back(nullptr);

		if (m_lstMaterials[MaterialSlot] == pMaterial)
			return;

		if (pMaterial)
			pMaterial->AddRef();

		if (m_lstMaterials[MaterialSlot])
			m_lstMaterials[MaterialSlot]->Release();

		m_lstMaterials[MaterialSlot] = pMaterial;
	}

	ISurfaceMaterial* Model::GetSurfaceMaterial(
		int32_t MaterialSlot
	) const noexcept
	{
		if (MaterialSlot < 0 || static_cast<uint32_t>(MaterialSlot) >= m_lstMaterials.size())
			return nullptr;

		return m_lstMaterials[MaterialSlot];
	}

	int32_t Model::GetMeshIndexByName(
		NameHash MeshName
	) const noexcept
	{
		for (uint32_t i = 0; i < m_lstMeshes.size(); ++i)
		{
			if (m_lstMeshes[i].Name == MeshName)
				return static_cast<int32_t>(i);
		}

		return -1;
	}

	int32_t Model::GetNumSubMesh() const noexcept
	{
		int32_t Count = 0;
		for (const MESH& Mesh : m_lstMeshes)
			Count += static_cast<int32_t>(Mesh.SubMeshes.size());

		return Count;
	}

	int32_t Model::GetNumSubMeshByName(
		NameHash MeshName
	) const noexcept
	{
		for (const MESH& Mesh : m_lstMeshes)
		{
			if (Mesh.Name == MeshName)
				return static_cast<int32_t>(Mesh.SubMeshes.size());
		}

		return 0;
	}

	int32_t Model::GetNumSubMeshByIndex(
		int32_t MeshIndex
	) const noexcept
	{
		if (MeshIndex < 0 || static_cast<uint32_t>(MeshIndex) >= m_lstMeshes.size())
			return 0;

		return static_cast<int32_t>(m_lstMeshes[MeshIndex].SubMeshes.size());
	}

	int32_t Model::GetMaterialSlotCount() const
	{
		return static_cast<int32_t>(m_lstMaterials.size());
	}

}
