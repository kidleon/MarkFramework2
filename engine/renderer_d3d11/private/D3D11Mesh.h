#pragma once


class D3D11Mesh final : public IMesh
{
public:
	constexpr static UINT32 MAX_MESH_PART = 8;

public:
	D3D11Mesh(
		IPrimitiveBuffer* pPrimitiveBuffer,
		INT32 PrimitiveIndex,
		UINT32 TotalVertexCount,
		UINT32 TotalIndexCount
	);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IMesh interface
	virtual void SetName(const char* szName) noexcept final;
	virtual const NameHash& GetName() const noexcept final;

	virtual void SetPosition(const FLOAT3* pPositions, UINT32 NumPosition) noexcept final;
	virtual void SetNormal(const FLOAT3* pNormals, UINT32 NumNormal) noexcept final;
	virtual void SetTexCoord(const FLOAT2* pTexCoords, UINT32 NumTexCoord) noexcept final;
	virtual void SetColor(const FLOAT4* pColors, UINT32 NumColor) noexcept final;
	virtual void SetTangent(const FLOAT3* pTangents, UINT32 NumTangent) noexcept final;
	virtual void SetBinormal(const FLOAT3* pBinormals, UINT32 NumBinormal) noexcept final;

	virtual void SetIndex(const UINT32* pIndices, UINT32 NumIndices) noexcept final;
	virtual void SetIndex(UINT32 NumMeshParts, const UINT32** ppIndices, UINT32* pNumIndices) noexcept final;
	virtual UINT32 GetNumMeshPart() const noexcept final;
	virtual UINT32 GetNumIndex(INT32 MeshPartIndex) const noexcept final;

	virtual void SetMaterial(INT32 MeshPartIndex, ISurfaceMaterial* pMaterial) noexcept final;
	virtual void GetMaterial(INT32 MeshPartIndex, ISurfaceMaterial** ppMaterial) noexcept final;
	virtual ISurfaceMaterial* GetMaterial(INT32 MeshPartIndex) noexcept final;

	__FORCEINLINE const NameHash& INL_GetNameHash() const noexcept { return m_NameHash; }
	__FORCEINLINE UINT32 INL_GetTotalVertexCount() const noexcept { return m_TotalVertexCount; }
	__FORCEINLINE UINT32 INL_GetTotalIndexCount() const noexcept { return m_TotalIndexCount; }

	__FORCEINLINE UINT32 INL_GetNumMeshPart() const noexcept { return m_NumMeshPart; }
	__FORCEINLINE UINT32 INL_GetNumIndices(INT32 MeshPartIndex) const noexcept { return m_pNumIndices[MeshPartIndex]; }

	__FORCEINLINE INT32 INL_GetPrimitiveIndex() const noexcept { return m_PrimitiveIndex; }
	__FORCEINLINE IPrimitiveBuffer* INL_GetPrimitiveBuffer() const noexcept { return m_pPrimitiveBuffer; }
	__FORCEINLINE ISurfaceMaterial* INL_GetMaterial(INT32 MeshPartIndex) const noexcept { return m_pMaterials[MeshPartIndex]; }

private:
	D3D11Mesh() = delete;
	virtual ~D3D11Mesh() noexcept;

private:
	volatile long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	NameHash m_NameHash;

	INT32 m_PrimitiveIndex = 0;
	UINT32 m_TotalVertexCount = 0;
	UINT32 m_TotalIndexCount = 0;
	UINT32 m_NumMeshPart = 0;
	UINT32 m_UsedVertexCount = 0;
	UINT32 m_pNumIndices[MAX_MESH_PART] = { 0 };
	IPrimitiveBuffer* m_pPrimitiveBuffer = nullptr;
	ISurfaceMaterial* m_pMaterials[MAX_MESH_PART] = { nullptr };

};


