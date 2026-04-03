#pragma once


extern BOOL CreateTexture1DFromSTBI(
    HANDLE hTempHeap,
    ID3D11Device* pDevice,
    const TEXTURE1D_CREATE_DESC& Desc,
	TEXTURE1D_DESC* pOutDesc,
    ID3D11Texture1D** ppTex1D,
    ID3D11ShaderResourceView** ppSRV
);

extern BOOL CreateTexture2DFromSTBI(
    HANDLE hTempHeap,
    ID3D11Device* pDevice,
    const TEXTURE2D_CREATE_DESC& Desc,
	TEXTURE2D_DESC* pOutDesc,
    ID3D11Texture2D** ppTex2D,
    ID3D11ShaderResourceView** ppSRV
);
