#ifndef __I_RENDER_CAMERA_H__
#define __I_RENDER_CAMERA_H__

#include "IAssetImpl.h"


interface IRenderCamera : public IASSET_IMPL<ASSET_TYPE::RENDER_CAMERA>
{
	virtual void SetClearTarget(UINT32 ClearBuffers,
								const FLOAT4 & ClearColor,
								FLOAT Depth,
								UINT32 Stencil) noexcept = 0;

	virtual void SetCameraMode(CAMERA_MODE CameraMode) noexcept = 0;

	virtual void SetPerspective(FLOAT Fovy,
								FLOAT Aspect,
								FLOAT NearZ,
								FLOAT FarZ) noexcept = 0;

	virtual void SetOrtho(FLOAT ViewWidth,
						  FLOAT ViewHeight,
						  FLOAT NearZ,
						  FLOAT FarZ) noexcept = 0;

	virtual void SetView(const FLOAT3& EyePos,
						 const FLOAT3& EyeDir,
						 const FLOAT3& EyeUp) = 0;

	virtual void LookAt(const FLOAT3& EyePos, const FLOAT3& Target) noexcept = 0;

	virtual void SetViewportLayer(INT8 Layer) noexcept = 0;
};


#endif // __I_RENDER_CAMERA_H__
