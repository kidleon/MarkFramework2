#pragma once
#include "mathlib.h"


struct alignas(16) D3D11_CAMERA_CONSTANT
{
	MATRIX4 ViewMatrix;
	MATRIX4 InvViewMatrix;
	MATRIX4 ProjectionMatrix;
	//FLOAT4 CameraPosition;
};

struct alignas(16) D3D11_OBJECT_CONSTANT
{
	MATRIX4 World;
};
