#ifndef __D3D11TEXTURELOADER_DDS_H__
#define __D3D11TEXTURELOADER_DDS_H__

#include <d3d11.h>


HRESULT CreateDDSTextureFromMemory(
	__in ID3D11Device* pDev,
	__in_z const char* szFileName,
	__in_z const void* pData,
	__in size_t DataLength,
	__out_opt ID3D11ShaderResourceView** ppSRV,
	bool sRGB = false
);



#endif // __D3D11TEXTURELOADER_DDS_H__