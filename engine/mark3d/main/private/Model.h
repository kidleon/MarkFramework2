#ifndef __MODEL_H__
#define __MODEL_H__

#include "GeomDef.h"
#include "TArray.h"
using namespace mark;


class Model : public IModel
{
public:
	Model(UINT64 ID, IPrimitiveBuffer* pPrimitiveBuffer);

	// IUNKNOWN interface
	virtual long AddRef() final;
	virtual long Release() final;
	virtual long RefCnt() final;

	// IAsset interface
	virtual UINT64 GetID() const noexcept final;
	virtual ASSET_TYPE GetAssetType() const noexcept final;
	virtual LOAD_STAT GetLoadStat() const noexcept final;

	// IModel interface
	virtual int32 AddMesh(NameHash Name, PRIMITIVE_TYPE PrimitiveType, uint32 VertexFormat, size_t IndexStride, size_t NumVertex, size_t NumIndex) noexcept final;
	virtual void ClearMesh() noexcept final;

	virtual void UpdateVertex(int32 MeshIndex, void* pVertexData, size_t VertexSize) noexcept final;
	virtual void UpdateVertex(NameHash Name, void* pVertexData, size_t VertexSize) noexcept final;

	virtual void UpdateIndex(int32 MeshIndex, void* pIndexData, size_t IndexSize) noexcept final;
	virtual void UpdateIndex(NameHash Name, void* pIndexData, size_t IndexSize) noexcept final;

	virtual void SetMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept final;
	virtual void SetMaterial(NameHash Name, ISurfaceMaterial* pSurfaceMaterial) noexcept final;

	virtual int32 FindMeshIndexByName(NameHash Name) const noexcept final;
	virtual ISurfaceMaterial* GetMaterial(int32 MeshIndex) noexcept final;
	virtual ISurfaceMaterial* GetMaterial(NameHash Name) noexcept final;

	virtual void SetActiveMesh(int32 MeshIndex, BOOL Active) noexcept final;
	virtual void SetActiveMesh(NameHash Name, BOOL Active) noexcept final;

	virtual BOOL IsActiveMesh(int32 MeshIndex) const noexcept final;
	virtual BOOL IsActiveMesh(NameHash Name) const noexcept final;

	virtual size_t GetNumMesh() const noexcept final;

private:
	Model() = delete;
	virtual ~Model() noexcept;

protected:
	size_t CalulateVertexStride(uint32 VertexFormat) const noexcept;

private:
	long m_RefCnt = 1;
#if defined(__TARGET_OS_WINDOWS)
	unsigned PADDING_OR_RESERVED = 0;
#endif // defined(__TARGET_OS_WINDOWS)

	UINT64 m_ID = 0;
	LOAD_STAT m_LoadStat = LOAD_STAT::LOADED;

	struct MeshData
	{
		NameHash Name;
		int PrimitiveIndex;
		BOOL Active;
		uint32 VertexFormat;
		uint32 VertexStride;
		uint32 IndexStride;
		uint32 MaxVertex;
		uint32 MaxIndex;
		uint32 NumVertex;
		uint32 NumIndex;
		ISurfaceMaterial* pSurfaceMaterial;
	};

	TArray<MeshData, TA_POOL> m_lstMeshData;
	
	IPrimitiveBuffer* m_pPrimitiveBuffer = nullptr;

};


#endif // __MODEL_H__