#ifndef __MODEL_H__
#define __MODEL_H__

#include "GeomDef.h"

class Model : public IModel
{
	constexpr static size_t MAX_MESH_PER_MODEL = 16;

public:
	Model(UINT64 ID, UINT32 VertexFormat, IPrimitiveBuffer* pPrimitiveBuffer);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IModel interface
	virtual UINT32 GetNumMesh() const noexcept final;
	virtual INT32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, UINT32 NumVertex, UINT32 NumIndex) noexcept final;

	virtual IMesh* GetMesh(INT32 MeshIndex) noexcept final;
	virtual IMesh* GetMesh(NameHash Name) noexcept final;

	__FORCEINLINE IPrimitiveBuffer* INL_GetPrimitiveBuffer() const noexcept
	{
		return m_pPrimitiveBuffer;
	}

	__FORCEINLINE UINT32 INL_GetVertexFormat() const noexcept { return m_VertexFormat; }
	__FORCEINLINE UINT32 INL_GetNumMesh() const noexcept { return m_NumMesh; }
	__FORCEINLINE IMesh* INL_GetMesh(UINT32 MeshIndex) const noexcept
	{
		if (MeshIndex >= m_NumMesh)
			return nullptr;
		return m_pMeshes[MeshIndex];
	}

private:
	Model() = delete;
	virtual ~Model() noexcept;

private:
	long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::LOADED;

	UINT32 m_NumMesh = 0;
	IMesh* m_pMeshes[MAX_MESH_PER_MODEL] = { nullptr };
	
	UINT32 m_VertexFormat = 0;
	UINT32 PADDING = 0;
	IPrimitiveBuffer* m_pPrimitiveBuffer = nullptr;
};


#endif // __MODEL_H__