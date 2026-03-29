#include "pch.h"
#include "ModelLoader.h"
#include "ModelAsset.h"
#include "temp_pool.h"
#include "os_file.h"
#include "fbx_loader.h"
#include "AsyncAssetOp.h"
#include "Assets.h"
#include "Mark3DImpl.h"


BOOL LoadModelFromModelAsset(
	HANDLE hTempPool,
	IFileSystem* pFileSystem,
	IAssets* pAssets,
	IRenderSystem* pRenderSystem,
	const char* szRelativePath,
	IModelAsset* pModelAsset,
	IModel* pModel
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

	UINT32 ModelAttrib = pModelAsset->GetModelAttrib();

	// Load Mesh
	if (!(ModelAttrib & (UINT32)MODEL_ATTRIB::MESH))
	{
		SYS_LOG_E("Model::CreateMesh - Model asset does not contain mesh data.");
		return FALSE;
	}

	size_t NumMesh = pModelAsset->GetNumMesh();

	for (size_t i = 0; i < NumMesh; i++)
	{
		size_t NumSubMesh = pModelAsset->GetNumSubMesh((int32)i);

		NumSubMesh = T_MIN(NumSubMesh, MAX_MESH_PART);

		INT32 PrimitiveIndex = -1;

		const char* szMeshName = pModelAsset->GetMeshName((int32)i);
		UINT32 NumVertex = (UINT32)pModelAsset->GetNumVertices((int32)i);
		UINT32 NumIndex = (UINT32)pModelAsset->GetNumIndices((int32)i);

		INT32 MeshIndex = pModel->AddMesh(
			NameHash(szMeshName), 
			PRIMITIVE_TYPE::TRIANGLE_LIST, 
			NumVertex, 
			NumIndex
		);

		if (-1 == MeshIndex)
		{
			SYS_LOG_E("LoadModelFromModelAsset - Failed to add mesh to model.");
			continue;
		}

		IMesh* pMesh = static_cast<IMesh*>(pModel->GetMesh((UINT32)MeshIndex));
		if (!pMesh)
		{
			SYS_LOG_E("LoadModelFromModelAsset - Failed to get mesh from model.");
			continue;
		}

		FLOAT3* pPositions = pModelAsset->GetPositions((int32)i);
		FLOAT3* pNormals = pModelAsset->GetNormals((int32)i);
		FLOAT2* pTexCoords = pModelAsset->GetTexCoords((int32)i);
		FLOAT4* pColors = pModelAsset->GetColor((int32)i);
		FLOAT3* pTangents = pModelAsset->GetTangent((int32)i);
		FLOAT3* pBinormals = pModelAsset->GetBinormal((int32)i);

		if (pPositions)
			pMesh->SetPosition(pPositions, (UINT32)pModelAsset->GetNumVertices((int32)i));

		if (pNormals)
			pMesh->SetNormal(pNormals, (UINT32)pModelAsset->GetNumVertices((int32)i));

		if (pTexCoords)
			pMesh->SetTexCoord(pTexCoords, (UINT32)pModelAsset->GetNumVertices((int32)i));

		if (pColors)
			pMesh->SetColor(pColors, (UINT32)pModelAsset->GetNumVertices((int32)i));

		if (pTangents)
			pMesh->SetTangent(pTangents, (UINT32)pModelAsset->GetNumVertices((int32)i));

		if (pBinormals)
			pMesh->SetBinormal(pBinormals, (UINT32)pModelAsset->GetNumVertices((int32)i));

		Assets* pAssetsImpl = static_cast<Assets*>(pAssets);

		char szRelativeOnlyPath[MAX_FILE_LENGTH] = {};
		char szFullPath[MAX_FILE_LENGTH] = {};

		get_path(szRelativePath, szRelativeOnlyPath, sizeof(szRelativeOnlyPath));

		if (1 == NumSubMesh)
		{
			UINT32* pIndices = pModelAsset->GetIndices((int32)i, 0);
			UINT32 NumIndices = (UINT32)pModelAsset->GetNumIndices((int32)i, 0);
			pMesh->SetIndex(pIndices, NumIndices);

			ISurfaceMaterial* pSurfaceMaterial = nullptr;
			pRenderSystem->CreateSurfaceMaterial(&pSurfaceMaterial);
			pMesh->SetMaterial(0, pSurfaceMaterial);
			

			int32 MaterialIndex = pModelAsset->GetMaterialIndex((int32)i, 0);
			if (0 <= MaterialIndex)
			{
				if (pModelAsset->HasDiffuseTexture(MaterialIndex))
				{
					temppool_clear(hTempPool);

					combine_path(
						szRelativeOnlyPath,
						pModelAsset->GetMaterialDiffuse(MaterialIndex), 
						szFullPath,
						MAX_FILE_LENGTH
					);

					ITexture2D* pDiffuseTexture = nullptr;
					pAssetsImpl->Load(
						hTempPool,
						szFullPath,
						TRUE,
						&pDiffuseTexture
					);

					pSurfaceMaterial->SetDiffuseTexture(0, pDiffuseTexture);
				}

				if (pModelAsset->HasNormalTexture(MaterialIndex))
				{
					temppool_clear(hTempPool);

					combine_path(
						szRelativeOnlyPath,
						pModelAsset->GetMaterialNormal(MaterialIndex),
						szFullPath,
						MAX_FILE_LENGTH
					);

					ITexture2D* pNormalTexture = nullptr;
					pAssetsImpl->Load(
						hTempPool,
						szFullPath,
						FALSE,
						&pNormalTexture
					);
					pSurfaceMaterial->SetNormalTexture(0, pNormalTexture);
				}

				if (pModelAsset->HasSpecularTexture(MaterialIndex))
				{
					temppool_clear(hTempPool);

					combine_path(
						szRelativeOnlyPath,
						pModelAsset->GetMaterialSpecular(MaterialIndex),
						szFullPath,
						MAX_FILE_LENGTH
					);

					ITexture2D* pSpecularTexture = nullptr;
					pAssetsImpl->Load(
						hTempPool,
						szFullPath,
						TRUE,
						&pSpecularTexture
					);
					pSurfaceMaterial->SetSpecularTexture(0, pSpecularTexture);
				}

				if (pModelAsset->HasEmissiveTexture(MaterialIndex))
				{
					temppool_clear(hTempPool);

					combine_path(
						szRelativeOnlyPath,
						pModelAsset->GetMaterialEmissive(MaterialIndex),
						szFullPath,
						MAX_FILE_LENGTH
					);

					ITexture2D* pEmissiveTexture = nullptr;
					pAssetsImpl->Load(
						hTempPool,
						szFullPath,
						TRUE,
						&pEmissiveTexture
					);
					pSurfaceMaterial->SetEmissiveTexture(0, pEmissiveTexture);
				}

				CHECK_RELEASE(pSurfaceMaterial);
			}
		}
		else if(1 < NumSubMesh)
		{
			UINT32 NumIndexArray = (UINT32)NumSubMesh;
			UINT32 NumIndices[MAX_MESH_PART] = {};
			for (int32 s = 0; s < (int32)NumSubMesh; s++)
			{
				size_t NumIndex = pModelAsset->GetNumIndices((int32)i, s);
				NumIndices[s] = (UINT32)NumIndex;
			}

			UINT32* pIndicesData[MAX_MESH_PART] = {};
			for (int32 s = 0; s < (int32)NumSubMesh; s++)
			{
				pIndicesData[s] = pModelAsset->GetIndices((int32)i, s);
			}

			pMesh->SetIndex(NumIndexArray, (const UINT32**)(pIndicesData), (UINT32*)NumIndices);

			for (int32 s = 0; s < (int32)NumSubMesh; s++)
			{
				ISurfaceMaterial* pSurfaceMaterial = nullptr;
				pRenderSystem->CreateSurfaceMaterial(&pSurfaceMaterial);

				pMesh->SetMaterial(s, pSurfaceMaterial);

				int32 MaterialIndex = pModelAsset->GetMaterialIndex((int32)i, s);
				if (0 <= MaterialIndex)
				{
					if (pModelAsset->HasDiffuseTexture(MaterialIndex))
					{
						temppool_clear(hTempPool);

						combine_path(
							szRelativeOnlyPath,
							pModelAsset->GetMaterialDiffuse(MaterialIndex),
							szFullPath,
							MAX_FILE_LENGTH
						);
	
						ITexture2D* pDiffuseTexture = nullptr;
						pAssetsImpl->Load(
							hTempPool,
							szFullPath,
							TRUE,
							&pDiffuseTexture
						);
						pSurfaceMaterial->SetDiffuseTexture(0, pDiffuseTexture);
					}

					if (pModelAsset->HasNormalTexture(MaterialIndex))
					{
						temppool_clear(hTempPool);

						combine_path(
							szRelativeOnlyPath,
							pModelAsset->GetMaterialNormal(MaterialIndex),
							szFullPath,
							MAX_FILE_LENGTH
						);

						ITexture2D* pNormalTexture = nullptr;
						pAssetsImpl->Load(
							hTempPool,
							szFullPath,
							FALSE,
							&pNormalTexture
						);
						pSurfaceMaterial->SetNormalTexture(0, pNormalTexture);
					}

					if (pModelAsset->HasSpecularTexture(MaterialIndex))
					{
						temppool_clear(hTempPool);

						combine_path(
							szRelativeOnlyPath,
							pModelAsset->GetMaterialSpecular(MaterialIndex),
							szFullPath,
							MAX_FILE_LENGTH
						);

						ITexture2D* pSpecularTexture = nullptr;
						pAssetsImpl->Load(
							hTempPool,
							szRelativeOnlyPath,
							TRUE,
							&pSpecularTexture
						);
						pSurfaceMaterial->SetSpecularTexture(0, pSpecularTexture);
					}

					if (pModelAsset->HasEmissiveTexture(MaterialIndex))
					{
						temppool_clear(hTempPool);

						combine_path(
							szRelativeOnlyPath,
							pModelAsset->GetMaterialEmissive(MaterialIndex),
							szFullPath,
							MAX_FILE_LENGTH
						);

						ITexture2D* pEmissiveTexture = nullptr;
						pAssetsImpl->Load(
							hTempPool,
							szRelativeOnlyPath,
							TRUE,
							&pEmissiveTexture
						);
						pSurfaceMaterial->SetEmissiveTexture(0, pEmissiveTexture);
					}

					CHECK_RELEASE(pSurfaceMaterial);
				}
			}
			
		}
	}

	return TRUE;
}


BOOL AsyncLoadModelFromModelAsset(HANDLE temppool_handle, void* pArg)
{
	AsyncAssetOp* pAsyncOp = (AsyncAssetOp*)pArg;
	if (!pAsyncOp->pAsset) return FALSE;

	BOOL Result = LoadModelFromModelAsset(
		temppool_handle,
		pAsyncOp->pFileSystem,
		pAsyncOp->pAssets,
		pAsyncOp->pRenderSystem,
		pAsyncOp->szRelativePath,
		reinterpret_cast<IModelAsset*>(pAsyncOp->Argument1),
		static_cast<IModel*>(pAsyncOp->pAsset)
	);

	if (!Result)
	{
		SYS_LOG_E("AsyncLoadModelFromModelAsset - Failed to load model from model asset.");
	}

	return Result;
}