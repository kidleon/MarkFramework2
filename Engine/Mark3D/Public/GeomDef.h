#pragma once
#include "unknown_ptr.h"


namespace mark
{
	enum class GPU_BUFFER_LAYOUT : uint32_t
	{
		MERGED,
		SEPARATE,
	};


	struct IModelAsset;

	enum class GPUGEOMETRY_CREATE_FLAGS : uint32_t
	{
		ASYNC_LOAD = 0x1,
		GENERATE_TANGENT = 0x2,
		HAS_MODEL_ASSET = 0x4,
	};

	struct GPUGeometryCreateDesc
	{
		unknown_ptr<IModelAsset> pModelAsset;
		GPU_BUFFER_LAYOUT BufferLayout = GPU_BUFFER_LAYOUT::MERGED;
		uint32_t VertexFormats = 0;
		uint32_t CreateFlags = 0;
	};

	struct IResource : public Unknown
	{
		virtual bool IsLoaded() const noexcept = 0;
	};

	struct IGPUGeometry : public IResource
	{
	};

	struct IModelInstance : public Unknown
	{
	};

}
