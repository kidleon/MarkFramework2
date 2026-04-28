#pragma once
#include <d3d11.h>


namespace mark
{
	struct D3D11Buffer
	{
		ID3D11Buffer* pBuffer = nullptr;
		UINT32 BufferSize = 0;
		BUFFER_USAGE Usage = BUFFER_USAGE::DEFAULT;
	};
}
