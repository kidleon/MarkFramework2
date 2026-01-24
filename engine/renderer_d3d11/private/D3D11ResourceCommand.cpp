#include "pch.h"
#include "D3D11ResourceCommand.h"
#include "D3D11PrimitiveBuffer.h"


void D3D11_RESOURCE_COMMAND::Reset()
{
	CHECK_RELEASE(pPrimitiveBuffer);
}