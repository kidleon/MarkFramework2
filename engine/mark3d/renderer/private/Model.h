#ifndef __MODEL_H__
#define __MODEL_H__

#include "RenderDef.h"
#include "TArray.h"
using namespace mark;


class Model : public IModel
{
public:
	Model(IPrimitiveBuffer* pPrimitiveBuffer);

	int32 AddMesh(NameHash Name, uint32 VertexFormat, size_t NumVertex, size_t NumIndex) noexcept final;
	void ClearMesh() noexcept final;

	void UpdateVertex(int32 MeshIndex, void* pVertexData, size_t VertexSize) noexcept final;
	void UpdateIndex(int32 MeshIndex, void* pIndexData, size_t IndexSize) noexcept final;
	int32 SetSurfaceMaterial(int32 MeshIndex, ISurfaceMaterial* pSurfaceMaterial) noexcept final;

	int32 FindMeshIndexByName(NameHash Name) const noexcept final;

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

	struct MeshData
	{
		NameHash Name;
		int PrimitiveIndex;
		uint32 VertexFormat;
		uint32 IndexFormat;
		size_t NumVertex;
		size_t NumIndex;
		ISurfaceMaterial* pSurfaceMaterial;

		LINK_NODE LinkNode;
	};

	LINKED_LIST m_MeshList;
	
	IPrimitiveBuffer* m_pPrimitiveBuffer = nullptr;

};


#endif // __MODEL_H__