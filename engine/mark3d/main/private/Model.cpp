#include "pch.h"
#include "Model.h"
#include "ModelAsset.h"
#include "Assets.h"
#include "Mark3DImpl.h"
#include "temp_pool.h"


Model::Model(UINT64 ID, uint32 VertexFormat, IPrimitiveBuffer* pPrimitiveBuffer)
	: m_pPrimitiveBuffer(pPrimitiveBuffer)
	, m_VertexFormat(VertexFormat)
{
}

Model::~Model() noexcept
{
	CHECK_RELEASE(m_pPrimitiveBuffer);
}

long Model::AddRef()
{
	interlock_increment_l(&m_RefCnt, MEMORY_ORDER_RELAXED);
	return m_RefCnt;
}

long Model::Release()
{
	long NewRefCnt = interlock_decrement_l(&m_RefCnt, MEMORY_ORDER_ACQ_REL);
	if (!NewRefCnt)
	{
		CORE_POOL_DELETE(this, Model);
	}

	return NewRefCnt;
}

long Model::RefCnt()
{
	return m_RefCnt;
}

UINT64 Model::GetID() const noexcept
{
	return m_ID;
}

ASSET_TYPE Model::GetAssetType() const noexcept
{
	return ASSET_TYPE::MODEL;
}

LOAD_STAT Model::GetLoadStat() const noexcept
{
	return m_LoadStat;
}

size_t Model::GetNumMesh() const noexcept
{
	return m_lstMeshData.size();
}

size_t Model::GetNumSubMesh(int32 MeshIndex) const noexcept
{
	if (MeshIndex < 0 || MeshIndex >= (int32)m_lstMeshData.size())
	{
		SYS_LOG_E("Model::GetNumSubMesh - Mesh index is out of range.");
		return 0;
	}

	return m_lstMeshData[MeshIndex].NumSubMesh;
}

size_t Model::GetNumSubMesh(NameHash Name) const noexcept
{
	int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::GetNumSubMesh - Mesh with the specified name does not exist.");
		return 0;
	}

	return m_lstMeshData[MeshIndex].NumSubMesh;
}

int32 Model::AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 NumVertex, uint32 NumIndex) noexcept
{
	int32 MeshIndex = FindMeshIndex(Name);
	if (-1 != MeshIndex)
	{
		SYS_LOG_E("Model::AddMesh - A mesh with the same name already exists.");
		return -1;
	}

	int32 PrimitiveIndex = m_pPrimitiveBuffer->AddPrimitive(PrimitiveType, NumVertex, NumIndex);
	if (-1 == PrimitiveIndex)
	{
		SYS_LOG_E("Model::AddMesh - Failed to add primitive to primitive buffer.");
		return -1;
	}

	MESH_DATA NewMeshData = {};
	NewMeshData.Name = Name;
	NewMeshData.PrimitiveIndex = PrimitiveIndex;
	NewMeshData.MaxVertex = NumVertex;
	NewMeshData.MaxIndex = NumIndex;
	NewMeshData.NumVertex = 0;
	NewMeshData.NumSubMesh = 1;
	NewMeshData.SubMeshes[0].NumIndex = NumIndex;

	m_lstMeshData.push_back(NewMeshData);

	return (int32)(m_lstMeshData.size() - 1);
}

int32 Model::AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 NumVertex, uint32 NumIndices, uint32* pNumIndices) noexcept
{
	int32 MeshIndex = FindMeshIndex(Name);
	if (-1 != MeshIndex)
	{
		SYS_LOG_E("Model::AddMesh - A mesh with the same name already exists.");
		return -1;
	}

	int32 PrimitiveIndex = m_pPrimitiveBuffer->AddPrimitive(PrimitiveType, NumVertex, NumIndices, pNumIndices);
	if (-1 == PrimitiveIndex)
	{
		SYS_LOG_E("Model::AddMesh - Failed to add primitive to primitive buffer.");
		return -1;
	}

	uint32 TotalIndexCount = 0;
	for (uint32 i = 0; i < NumIndices; i++)
		TotalIndexCount += pNumIndices[i];

	MESH_DATA NewMeshData = {};
	NewMeshData.Name = Name;
	NewMeshData.PrimitiveIndex = PrimitiveIndex;
	NewMeshData.MaxVertex = NumVertex;
	NewMeshData.MaxIndex =	TotalIndexCount;
	NewMeshData.NumVertex = 0;
	NewMeshData.NumSubMesh = NumIndices;
	for (uint32 i = 0; i < NumIndices; i++)
		NewMeshData.SubMeshes[i].NumIndex = pNumIndices[i];

	m_lstMeshData.push_back(NewMeshData);

	return (int32)(m_lstMeshData.size() - 1);
}

int32 Model::FindMeshIndex(NameHash Name) const noexcept
{
	if (Name.empty())
		return -1;

	for (size_t i = 0; i < m_lstMeshData.size(); i++)
	{
		if (m_lstMeshData[i].Name == Name)
			return (int32)i;
	}

	return -1;
}

void Model::SetMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept
{
	if (MeshIndex < 0 || MeshIndex >= (int32)m_lstMeshData.size())
	{
		SYS_LOG_E("Model::SetMaterial - Mesh index is out of range.");
		return;
	}
	/*
	if (m_lstMeshData[MeshIndex].SubMeshes[0].pMaterial != pSurfaceMaterial)
	{
		if (m_lstMeshData[MeshIndex].SubMeshes[0].pMaterial)
			m_lstMeshData[MeshIndex].SubMeshes[0].pMaterial->Release();

		if (pSurfaceMaterial)
			pSurfaceMaterial->AddRef();

		m_lstMeshData[MeshIndex].SubMeshes[0].pMaterial = pSurfaceMaterial;
	}
	*/
}

void Model::SetMaterial(NameHash Name, ISurfaceMaterial* pSurfaceMaterial) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetMaterial - Cannot find mesh by name.");
		return;
	}

	return SetMaterial(MeshIndex, pSurfaceMaterial);
}

void Model::SetMaterial(int32 MeshIndex, int32 SubMeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept
{
	if (MeshIndex < 0 || MeshIndex >= (int32)m_lstMeshData.size())
	{
		SYS_LOG_E("Model::SetMaterial - Mesh index is out of range.");
		return;
	}

	if (SubMeshIndex < 0 || SubMeshIndex >= (int32)m_lstMeshData[MeshIndex].NumSubMesh)
	{
		SYS_LOG_E("Model::SetMaterial - Sub-mesh index is out of range.");
		return;
	}

	if (m_lstMeshData[MeshIndex].SubMeshes[SubMeshIndex].pMaterial != pSurfaceMaterial)
	{
		if (m_lstMeshData[MeshIndex].SubMeshes[SubMeshIndex].pMaterial)
			m_lstMeshData[MeshIndex].SubMeshes[SubMeshIndex].pMaterial->Release();

		if (pSurfaceMaterial)
			pSurfaceMaterial->AddRef();

		m_lstMeshData[MeshIndex].SubMeshes[SubMeshIndex].pMaterial = pSurfaceMaterial;
	}
}

void Model::SetMaterial(NameHash Name, int32 SubMeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetMaterial - Cannot find mesh by name.");
		return;
	}

	return SetMaterial(MeshIndex, SubMeshIndex, pSurfaceMaterial);
}

void Model::SetPosition(int32 MeshIndex, FLOAT3* pPositions, UINT32 NumPosition) noexcept
{
	m_lstMeshData[MeshIndex].NumVertex = NumPosition;
	m_pPrimitiveBuffer->UpdatePosition(m_lstMeshData[MeshIndex].PrimitiveIndex, pPositions, NumPosition);
}

void Model::SetNormal(int32 MeshIndex, FLOAT3* pNormals, UINT32 NumNormal) noexcept
{
	if (m_lstMeshData[MeshIndex].NumVertex != NumNormal)
	{
		SYS_LOG_E("Model::SetNormal - The number of normals does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateNormal(m_lstMeshData[MeshIndex].PrimitiveIndex, pNormals, NumNormal);
}

void Model::SetTexCoord(int32 MeshIndex, FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept
{
	if (m_lstMeshData[MeshIndex].NumVertex != NumTexCoord)
	{
		SYS_LOG_E("Model::SetTexCoord - The number of texture coordinates does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateTexCoord0(m_lstMeshData[MeshIndex].PrimitiveIndex, pTexCoords, NumTexCoord);
}

void Model::SetColor(int32 MeshIndex, FLOAT4* pColors, UINT32 NumColor) noexcept
{
	if (m_lstMeshData[MeshIndex].NumVertex != NumColor)
	{
		SYS_LOG_E("Model::SetColor - The number of colors does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateColor(m_lstMeshData[MeshIndex].PrimitiveIndex, pColors, NumColor);
}

void Model::SetTangent(int32 MeshIndex, FLOAT3* pTangents, UINT32 NumTangent) noexcept
{
	if (m_lstMeshData[MeshIndex].NumVertex != NumTangent)
	{
		SYS_LOG_E("Model::SetTangent - The number of tangents does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateTangent(m_lstMeshData[MeshIndex].PrimitiveIndex, pTangents, NumTangent);
}

void Model::SetBinormal(int32 MeshIndex, FLOAT3* pBinormals, UINT32 NumBinormal) noexcept
{
	if (m_lstMeshData[MeshIndex].NumVertex != NumBinormal)
	{
		SYS_LOG_E("Model::SetBinormal - The number of binormals does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateBinormal(m_lstMeshData[MeshIndex].PrimitiveIndex, pBinormals, NumBinormal);
}

void Model::SetPosition(NameHash Name, FLOAT3* pPositions, UINT32 NumPosition) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetPosition - Cannot find mesh by name.");
		return;
	}

	m_lstMeshData[MeshIndex].NumVertex = (uint32)NumPosition;
	m_pPrimitiveBuffer->UpdatePosition(m_lstMeshData[MeshIndex].PrimitiveIndex, pPositions, NumPosition);
}

void Model::SetNormal(NameHash Name, FLOAT3* pNormals, UINT32 NumNormal) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
		return;

	if (m_lstMeshData[MeshIndex].NumVertex != NumNormal)
	{
		SYS_LOG_E("Model::SetNormal - The number of normals does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateNormal(m_lstMeshData[MeshIndex].PrimitiveIndex, pNormals, NumNormal);
}

void Model::SetTexCoord(NameHash Name, FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
		return;

	if (m_lstMeshData[MeshIndex].NumVertex != NumTexCoord)
	{
		SYS_LOG_E("Model::SetTexCoord - The number of texture coordinates does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateTexCoord0(m_lstMeshData[MeshIndex].PrimitiveIndex, pTexCoords, NumTexCoord);
}

void Model::SetColor(NameHash Name, FLOAT4* pColors, UINT32 NumColor) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
		return;

	if (m_lstMeshData[MeshIndex].NumVertex != NumColor)
	{
		SYS_LOG_E("Model::SetColor - The number of colors does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateColor(m_lstMeshData[MeshIndex].PrimitiveIndex, pColors, NumColor);
}

void Model::SetTangent(NameHash Name, FLOAT3* pTangents, UINT32 NumTangent) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
		return;

	if (m_lstMeshData[MeshIndex].NumVertex != NumTangent)
	{
		SYS_LOG_E("Model::SetTangent - The number of tangents does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateTangent(m_lstMeshData[MeshIndex].PrimitiveIndex, pTangents, NumTangent);
}

void Model::SetBinormal(NameHash Name, FLOAT3* pBinormals, UINT32 NumBinormal) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
		return;

	if (m_lstMeshData[MeshIndex].NumVertex != NumBinormal)
	{
		SYS_LOG_E("Model::SetBinormal - The number of binormals does not match the number of vertices in the mesh.");
		return;
	}

	m_pPrimitiveBuffer->UpdateBinormal(m_lstMeshData[MeshIndex].PrimitiveIndex, pBinormals, NumBinormal);
}

void Model::SetIndex(int32 MeshIndex, const uint32* pIndices, UINT32 NumIndex) noexcept
{
	m_lstMeshData[MeshIndex].SubMeshes[0].NumIndex = NumIndex;
	m_pPrimitiveBuffer->UpdateIndex(m_lstMeshData[MeshIndex].PrimitiveIndex, pIndices, NumIndex);
}

void Model::SetIndex(NameHash Name, const uint32* pIndices, UINT32 NumIndex) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
		return;

	m_lstMeshData[MeshIndex].SubMeshes[0].NumIndex = NumIndex;
	m_pPrimitiveBuffer->UpdateIndex(m_lstMeshData[MeshIndex].PrimitiveIndex, pIndices, NumIndex);
}

void Model::SetIndex(int32 MeshIndex, uint32 NumIndex, const uint32** ppIndices, uint32* pNumIndices) noexcept
{
	if (!NumIndex)
	{
		SYS_LOG_E("Model::SetIndex - Number of indices must be greater than zero.");
		return;
	}
	
	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::SetIndex - Invalid mesh index.");
		return;
	}

	m_pPrimitiveBuffer->UpdateIndex(m_lstMeshData[MeshIndex].PrimitiveIndex, NumIndex, ppIndices, pNumIndices);
}

void Model::SetIndex(NameHash Name, uint32 NumIndex, const uint32** ppIndices, uint32* pNumIndices) noexcept
{
	const int32 MeshIndex = FindMeshIndex(Name);
	if (-1 == MeshIndex)
	{
		SYS_LOG_E("Model::SetIndex - Cannot find mesh by name.");
		return;
	}

	if (!NumIndex)
	{
		SYS_LOG_E("Model::SetIndex - Number of indices must be greater than zero.");
		return;
	}

	if (m_lstMeshData.size() <= MeshIndex)
	{
		SYS_LOG_E("Model::SetIndex - Invalid mesh index.");
		return;
	}

	m_pPrimitiveBuffer->UpdateIndex(m_lstMeshData[MeshIndex].PrimitiveIndex, NumIndex, ppIndices, pNumIndices);
}

BOOL Model::LoadMaterial(IModelAsset* pModelAsset) noexcept
{
	if (!pModelAsset)
	{
		SYS_LOG_E("Model::LoadMaterial - Invalid model asset pointer.");
		return FALSE;
	}

	ModelAsset* pModelAssetImpl = static_cast<ModelAsset*>(pModelAsset);
	
	uint32 ModelAttrib = pModelAssetImpl->INL_GetModelAttrib();

	// Load Material
	if (!(ModelAttrib & (uint32)MODEL_ATTRIB::MATERIAL))
	{
		SYS_LOG_E("Model::LoadMaterial - Model asset does not contain material data.");
		return FALSE;
	}
	
	size_t NumMaterial = pModelAsset->GetNumMaterials();
	if (!NumMaterial)
	{
		SYS_LOG_W("Model::LoadMaterial - No materials found in model asset.");
		return TRUE;
	}

	IAssets* pAssets = nullptr;
	Mark3DImpl::Get()->GetAssetsInterface(&pAssets);

	IRenderSystem* pRenderSystem = nullptr;
	Mark3DImpl::Get()->GetRenderSystemInterface(&pRenderSystem);

	char szRelativePath[MAX_PATH] = {};
	pModelAsset->GetRelativePath(szRelativePath, MAX_PATH, TRUE);

	for (size_t i = 0; i < NumMaterial; i++)
	{
		ISurfaceMaterial* pSurfaceMaterial = nullptr;
		if (!pRenderSystem->CreateSurfaceMaterial(&pSurfaceMaterial))
			continue;

		const char* szDiffuseTexture = pModelAsset->GetMaterialDiffuse((uint32)i);
		if (szDiffuseTexture && fstrlen(szDiffuseTexture))
		{
			char szPath[MAX_PATH] = {};
			combine_path(szRelativePath, szDiffuseTexture, szPath, MAX_PATH);
		}

		m_lstMaterials.push_back(pSurfaceMaterial);
	}

	return TRUE;
}

BOOL Model::LoadMesh(IModelAsset* pModelAsset) noexcept
{
	if (!pModelAsset)
	{
		SYS_LOG_E("Model::LoadMesh - Invalid model asset pointer.");
		return FALSE;
	}

	ModelAsset* pModelAssetImpl = static_cast<ModelAsset*>(pModelAsset);
	
	uint32 ModelAttrib = pModelAssetImpl->INL_GetModelAttrib();

	// Load Mesh
	if (ModelAttrib & (uint32)MODEL_ATTRIB::MESH)
	{
		SYS_LOG_E("Model::CreateMesh - Model asset does not contain mesh data.");
		return FALSE;
	}
	
	size_t NumMesh = pModelAsset->GetNumMesh();
	for (size_t i = 0; i < NumMesh; i++)
	{
		size_t NumVertex = pModelAsset->GetNumVertices((int32)i);
		size_t NumIndex = pModelAsset->GetNumIndices((int32)i);

		FLOAT3* pPositions = pModelAsset->GetPositions((int32)i);
		FLOAT3* pNormals = pModelAsset->GetNormals((int32)i);
		FLOAT2* pTexCoords = pModelAsset->GetTexCoords((int32)i);
		FLOAT4* pColors = pModelAsset->GetColor((int32)i);
		FLOAT3* pTangents = pModelAsset->GetTangent((int32)i);
		FLOAT3* pBinormals = pModelAsset->GetBinormal((int32)i);

		UINT32 VertexFormat = 0;
		if (pPositions)
			VertexFormat |= (uint32)VERTEX_FORMAT::POSITION;

		if (pNormals)
			VertexFormat |= (uint32)VERTEX_FORMAT::NORMAL;

		if (pTexCoords)
			VertexFormat |= (uint32)VERTEX_FORMAT::TEXCOORD;

		if (pColors)
			VertexFormat |= (uint32)VERTEX_FORMAT::COLOR;

		if (pTangents)
			VertexFormat |= (uint32)VERTEX_FORMAT::TANGENT;

		if (pBinormals)
			VertexFormat |= (uint32)VERTEX_FORMAT::BINORMAL;

		const char* szMeshName = pModelAsset->GetMeshName((int32)i);

		size_t NumSubMesh = pModelAsset->GetNumSubMesh((int32)i);
		if (1 == NumSubMesh)
		{
			NumIndex = pModelAsset->GetNumIndices((int32)i, 0);

			int32 MeshIndex = AddMesh(NameHash(szMeshName), PRIMITIVE_TYPE::TRIANGLE_LIST, (uint32)NumVertex, (uint32)NumIndex);
			if (-1 == MeshIndex)
			{
				SYS_LOG_E("Model::CreateMesh - Failed to add mesh.");
				continue;
			}

			FLOAT3* pPositions = pModelAsset->GetPositions((int32)i);
			if (pPositions)
				SetPosition(MeshIndex, pPositions, (uint32)NumVertex);

			FLOAT3* pNormals = pModelAsset->GetNormals((int32)i);
			if (pNormals)
				SetNormal(MeshIndex, pNormals, (uint32)NumVertex);

			FLOAT2* pTexCoords = pModelAsset->GetTexCoords((int32)i);
			if (pTexCoords)
				SetTexCoord(MeshIndex, pTexCoords, (uint32)NumVertex);

			FLOAT4* pColors = pModelAsset->GetColor((int32)i);
			if (pColors)
				SetColor(MeshIndex, pColors, (uint32)NumVertex);

			FLOAT3* pTangents = pModelAsset->GetTangent((int32)i);
			if (pTangents)
				SetTangent(MeshIndex, pTangents, (uint32)NumVertex);

			FLOAT3* pBinormals = pModelAsset->GetBinormal((int32)i);
			if (pBinormals)
				SetBinormal(MeshIndex, pBinormals, (uint32)NumVertex);

			pModelAsset->GetMaterialIndex(MeshIndex, 0);
		}
		else
		{
			uint32 NumIndexArray = (uint32)NumSubMesh;
			uint32 NumIndices[MAX_SUBMESH_PER_MESH] = {};

			for (int32 s = 0; s < (int32)NumIndexArray; s++)
				NumIndices[s] = (uint32)pModelAsset->GetNumIndices((int32)i, s);
			
			int32 MeshIndex = AddMesh(
				NameHash(szMeshName),
				PRIMITIVE_TYPE::TRIANGLE_LIST,
				(uint32)NumVertex,
				NumIndexArray,
				NumIndices
			);

			if (-1 == MeshIndex)
			{
				SYS_LOG_E("Model::CreateMesh - Failed to add mesh.");
				continue;
			}

			FLOAT3* pPositions = pModelAsset->GetPositions((int32)i);
			if (pPositions)
				SetPosition(MeshIndex, pPositions, (uint32)NumVertex);

			FLOAT3* pNormals = pModelAsset->GetNormals((int32)i);
			if (pNormals)
				SetNormal(MeshIndex, pNormals, (uint32)NumVertex);

			FLOAT2* pTexCoords = pModelAsset->GetTexCoords((int32)i);
			if (pTexCoords)
				SetTexCoord(MeshIndex, pTexCoords, (uint32)NumVertex);

			FLOAT4* pColors = pModelAsset->GetColor((int32)i);
			if (pColors)
				SetColor(MeshIndex, pColors, (uint32)NumVertex);

			FLOAT3* pTangents = pModelAsset->GetTangent((int32)i);
			if (pTangents)
				SetTangent(MeshIndex, pTangents, (uint32)NumVertex);

			FLOAT3* pBinormals = pModelAsset->GetBinormal((int32)i);
			if (pBinormals)
				SetBinormal(MeshIndex, pBinormals, (uint32)NumVertex);

			uint32* pIndicesData[MAX_SUBMESH_PER_MESH] = {};
			for (int32 s = 0; s < (int32)NumSubMesh; s++)
				pIndicesData[s] = pModelAsset->GetIndices((int32)i, s);

			const uint32** ppIndicesData = (const uint32**)pIndicesData;
			uint32* pNumIndices = NumIndices;

			SetIndex(MeshIndex, NumIndexArray, ppIndicesData, pNumIndices);

			
		}
	}

	return TRUE;
}

void Model::OnApplyMaterials() noexcept
{
	for (size_t i = 0; i < m_lstMeshData.size(); i++)
	{
		for (size_t s = 0; s < m_lstMeshData[i].NumSubMesh; s++)
		{

		}
	}
}