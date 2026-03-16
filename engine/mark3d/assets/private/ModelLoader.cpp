#include "pch.h"
#include "ModelLoader.h"
#include "ModelAsset.h"
#include "temp_pool.h"
#include "fbx_loader.h"
#include "AsyncAssetOp.h"
#include "Model.h"
#include "Mark3DImpl.h"



BOOL LoadTexture(IAssets* pAssets, IFileSystem* pFileSystem, const char* szRelativePath, ITexture2D** ppOut);

BOOL LoadModelFromFile(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	const char* szRelativePath,
	ModelAsset* pModelAsset,
	Model* pModel
)
{
	if (!pModelAsset)
	{
		SYS_LOG_E("LoadModelFromFile - Invalid model asset pointer.");
		return FALSE;
	}

	if (!pModel)
	{
		SYS_LOG_E("LoadModelFromFile - Invalid model pointer.");
		return FALSE;
	}

	uint32 ModelAttrib = pModelAsset->INL_GetModelAttrib();

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
		
		NumSubMesh = T_MIN(NumSubMesh, Model::MAX_SUBMESH_PER_MESH);

		if (1 == NumSubMesh)
		{
			NumIndex = pModelAsset->GetNumIndices((int32)i, 0);

			int32 MeshIndex = pModel->AddMesh(
				NameHash(szMeshName), 
				PRIMITIVE_TYPE::TRIANGLE_LIST, 
				(uint32)NumVertex, 
				(uint32)NumIndex
			);

			if (-1 == MeshIndex)
			{
				SYS_LOG_E("Model::CreateMesh - Failed to add mesh.");
				continue;
			}

			FLOAT3* pPositions = pModelAsset->GetPositions((int32)i);
			if (pPositions)
				pModel->SetPosition(MeshIndex, pPositions, (uint32)NumVertex);

			FLOAT3* pNormals = pModelAsset->GetNormals((int32)i);
			if (pNormals)
				pModel->SetNormal(MeshIndex, pNormals, (uint32)NumVertex);

			FLOAT2* pTexCoords = pModelAsset->GetTexCoords((int32)i);
			if (pTexCoords)
				pModel->SetTexCoord(MeshIndex, pTexCoords, (uint32)NumVertex);

			FLOAT4* pColors = pModelAsset->GetColor((int32)i);
			if (pColors)
				pModel->SetColor(MeshIndex, pColors, (uint32)NumVertex);

			FLOAT3* pTangents = pModelAsset->GetTangent((int32)i);
			if (pTangents)
				pModel->SetTangent(MeshIndex, pTangents, (uint32)NumVertex);

			FLOAT3* pBinormals = pModelAsset->GetBinormal((int32)i);
			if (pBinormals)
				pModel->SetBinormal(MeshIndex, pBinormals, (uint32)NumVertex);

			uint32* pIndices = pModelAsset->GetIndices((int32)i, 0);
			pModel->SetIndex(MeshIndex, pIndices, (uint32)NumIndex);
		}
		else
		{
			uint32 NumIndexArray = (uint32)NumSubMesh;
			uint32 NumIndices[Model::MAX_SUBMESH_PER_MESH] = {};

			for (int32 s = 0; s < (int32)NumIndexArray; s++)
				NumIndices[s] = (uint32)pModelAsset->GetNumIndices((int32)i, s);

			int32 MeshIndex = pModel->AddMesh(
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
				pModel->SetPosition(MeshIndex, pPositions, (uint32)NumVertex);

			FLOAT3* pNormals = pModelAsset->GetNormals((int32)i);
			if (pNormals)
				pModel->SetNormal(MeshIndex, pNormals, (uint32)NumVertex);

			FLOAT2* pTexCoords = pModelAsset->GetTexCoords((int32)i);
			if (pTexCoords)
				pModel->SetTexCoord(MeshIndex, pTexCoords, (uint32)NumVertex);

			FLOAT4* pColors = pModelAsset->GetColor((int32)i);
			if (pColors)
				pModel->SetColor(MeshIndex, pColors, (uint32)NumVertex);

			FLOAT3* pTangents = pModelAsset->GetTangent((int32)i);
			if (pTangents)
				pModel->SetTangent(MeshIndex, pTangents, (uint32)NumVertex);

			FLOAT3* pBinormals = pModelAsset->GetBinormal((int32)i);
			if (pBinormals)
				pModel->SetBinormal(MeshIndex, pBinormals, (uint32)NumVertex);

			uint32* pIndicesData[Model::MAX_SUBMESH_PER_MESH] = {};
			for (int32 s = 0; s < (int32)NumSubMesh; s++)
				pIndicesData[s] = pModelAsset->GetIndices((int32)i, s);

			const uint32** ppIndicesData = (const uint32**)pIndicesData;
			uint32* pNumIndices = NumIndices;

			pModel->SetIndex(MeshIndex, NumIndexArray, ppIndicesData, pNumIndices);
		}
	}

	// Load Material
	IAssets* pAssets = nullptr;
	Mark3DImpl::Get()->GetAssetsInterface(&pAssets);

	IRenderSystem* pRenderSystem = nullptr;
	Mark3DImpl::Get()->GetRenderSystemInterface(&pRenderSystem);

	size_t NumMaterials = pModelAsset->GetNumMaterials();

	char szPath[MAX_FILE_LENGTH] = {};
	get_path(szRelativePath, szPath, MAX_FILE_LENGTH);

	char szFinalPath[MAX_FILE_LENGTH] = {};
	char szTempPath[MAX_FILE_LENGTH] = {};

	// Current Path: Assets/Models/ModelName/TextureName.png
	// Material Path : Assets/Models/ModelName/Material/TextureName.png
	// Common Path : Assets/Models/Common/TextureName.png
	for (size_t m = 0; m < NumMaterials; m++)
	{
		ITexture2D* pDiffuseTexture = nullptr;

		if (pModelAsset->HasDiffuseTexture((int32)m))
		{
			const char* szDiffuse = pModelAsset->GetMaterialDiffuse((int32)m);
			if (!LoadTexture(pAssets, pFileSystem, szDiffuse, &pDiffuseTexture))
			{
				SYS_LOG_W("LoadModelFromFile - Diffuse texture file not found: %s", szDiffuse);
				continue;
			}
		}

		ITexture2D* pNormalTexture = nullptr;

		if (pModelAsset->HasNormalTexture((int32)m))
		{
			const char* szNormal = pModelAsset->GetMaterialNormal((int32)m);
			if (!LoadTexture(pAssets, pFileSystem, szNormal, &pNormalTexture))
			{
				SYS_LOG_W("LoadModelFromFile - Normal texture file not found: %s", szNormal);
				continue;
			}
		}

		ITexture2D* pSpecularTexture = nullptr;
		
		if (pModelAsset->HasSpecularTexture((int32)m))
		{
			const char* szSpecular = pModelAsset->GetMaterialSpecular((int32)m);
			if (!LoadTexture(pAssets, pFileSystem, szSpecular, &pSpecularTexture))
			{
				SYS_LOG_W("LoadModelFromFile - Specular texture file not found: %s", szSpecular);
				continue;
			}
		}

		ITexture2D* pEmissiveTexture = nullptr;

		if (pModelAsset->HasEmissiveTexture((int32)m))
		{
			const char* szEmissive = pModelAsset->GetMaterialEmissive((int32)m);
			if (!LoadTexture(pAssets, pFileSystem, szEmissive, &pEmissiveTexture))
			{
				SYS_LOG_W("LoadModelFromFile - Emissive texture file not found: %s", szEmissive);
				continue;
			}
		}

		// Create Material
		ISurfaceMaterial* pMaterial = nullptr;
		pRenderSystem->CreateSurfaceMaterial(&pMaterial);


	}


	return TRUE;
}

BOOL LoadTexture(IAssets* pAssets, IFileSystem* pFileSystem, const char* szRelativePath, ITexture2D** ppOut)
{
	char szPath[MAX_FILE_LENGTH] = {};
	get_path(szRelativePath, szPath, MAX_FILE_LENGTH);

	char szFinalPath[MAX_FILE_LENGTH] = {};
	char szTempPath[MAX_FILE_LENGTH] = {};

	combine_path(szPath, szRelativePath, szFinalPath, MAX_FILE_LENGTH);

	if (!pFileSystem->ExistFile(szFinalPath))
	{
		combine_path(szPath, "material", szTempPath, MAX_FILE_LENGTH);
		combine_path(szTempPath, szRelativePath, szFinalPath, MAX_FILE_LENGTH);

		if (!pFileSystem->ExistFile(szFinalPath))
		{
			combine_path("assets/models/common", szRelativePath, szFinalPath, MAX_FILE_LENGTH);
			if (!pFileSystem->ExistFile(szFinalPath))
			{
				SYS_LOG_W("LoadTexture - Texture file not found: %s", szRelativePath);
				return FALSE;
			}
		}
	}

	return pAssets->Load(szFinalPath, ppOut);
}