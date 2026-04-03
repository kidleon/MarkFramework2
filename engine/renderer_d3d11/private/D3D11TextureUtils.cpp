#include "pch.h"
#include "D3D11TextureUtils.h"
#include "temp_pool.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"


BOOL CreateTexture1DFromSTBI(
    HANDLE hTempHeap,
    ID3D11Device* pDevice,
    const TEXTURE1D_CREATE_DESC& Desc,
	TEXTURE1D_DESC* pOutDesc,
    ID3D11Texture1D** ppTex1D,
    ID3D11ShaderResourceView** ppSRV
)
{
    // 1. stb_image 디코딩
    // Texture1D는 높이가 1이어야 하므로 디코딩 후 검증
    int width = 0, height = 0, channels = 0;
    stbi_uc* pMip0 = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(Desc.pData),
        static_cast<int>(Desc.DataSize),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!pMip0)
    {
        SYS_LOG_E("CreateTexture1DFromSTBI: stbi_load_from_memory failed. reason = %s",
            stbi_failure_reason());
        return FALSE;
    }

    if (height != 1)
    {
        SYS_LOG_E("CreateTexture1DFromSTBI: Texture1D requires height == 1, but got height = %d", height);
        stbi_image_free(pMip0);
        return FALSE;
    }

    // 2. DXGI 포맷 결정
    const DXGI_FORMAT dxgiFormat = (TRUE == Desc.sRGB)
        ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
        : DXGI_FORMAT_R8G8B8A8_UNORM;

    // 3. 밉 레벨 수 결정
    // Texture1D는 Width만 기준으로 밉 체인 계산
    const UINT maxMips = T_MIN(
        static_cast<UINT>(std::floor(log2f(static_cast<float>(width))) + 1.0f),
        static_cast<UINT>(D3D11_REQ_MIP_LEVELS)
    );

    const UINT mipLevels = (Desc.MipLevels > 0)
        ? T_MIN(Desc.MipLevels, maxMips)
        : maxMips;

    // 4. 각 밉 레벨 픽셀 데이터를 CPU에서 생성
    uint8_t* pTempBuffer[MAX_MIPMAP_LEVELS] = { nullptr };
    D3D11_SUBRESOURCE_DATA SubResourceData[MAX_MIPMAP_LEVELS] = {};

    // Mip0: stb_image 원본 그대로 (width * 4 bytes, height = 1 고정)
    pTempBuffer[0] = (uint8_t*)temppool_alloc(hTempHeap, static_cast<size_t>(width * 4));
    memcpy(pTempBuffer[0], pMip0, static_cast<size_t>(width * 4));

    SubResourceData[0].pSysMem = pTempBuffer[0];
    SubResourceData[0].SysMemPitch = static_cast<UINT>(width * 4); // 1D는 사실상 의미 없지만 명시
    SubResourceData[0].SysMemSlicePitch = 0;

    stbi_image_free(pMip0);
    pMip0 = nullptr;

    // Mip1 ~ MipN
    // Texture1D는 너비만 절반으로 줄어듦, 높이는 항상 1
    int srcW = width;
    for (UINT i = 1; i < mipLevels; ++i)
    {
        const int dstW = T_MAX(1, srcW / 2);

        pTempBuffer[i] = (uint8_t*)temppool_alloc(hTempHeap, static_cast<size_t>(dstW * 4));

        if (TRUE == Desc.sRGB)
        {
            // sRGB: 감마 → 선형 변환 후 필터링 → 다시 sRGB
            stbir_resize_uint8_srgb(
                pTempBuffer[i - 1], srcW, 1, srcW * 4,  // height = 1 고정
                pTempBuffer[i], dstW, 1, dstW * 4,
                STBIR_RGBA
            );
        }
        else
        {
            // 선형: 감마 보정 없이 직접 필터링
            stbir_resize_uint8_linear(
                pTempBuffer[i - 1], srcW, 1, srcW * 4,
                pTempBuffer[i], dstW, 1, dstW * 4,
                STBIR_RGBA
            );
        }

        SubResourceData[i].pSysMem = pTempBuffer[i];
        SubResourceData[i].SysMemPitch = static_cast<UINT>(dstW * 4);
        SubResourceData[i].SysMemSlicePitch = 0;

        srcW = dstW;
    }

    // 5. Texture1D 생성
    D3D11_TEXTURE1D_DESC TexDesc = {};
    TexDesc.Width = static_cast<UINT>(width);
    TexDesc.MipLevels = mipLevels;
    TexDesc.ArraySize = 1;
    TexDesc.Format = dxgiFormat;
    TexDesc.Usage = D3D11_USAGE_DEFAULT;
    TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TexDesc.CPUAccessFlags = 0;
    TexDesc.MiscFlags = 0;

    HRESULT hr = pDevice->CreateTexture1D(&TexDesc, SubResourceData, ppTex1D);
    if (FAILED(hr))
    {
        SYS_LOG_E("CreateTexture1DFromSTBI: CreateTexture1D failed, HRESULT = 0x%X", hr);
        return FALSE;
    }

    // 6. SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = dxgiFormat;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    SRVDesc.Texture1D.MostDetailedMip = 0;
    SRVDesc.Texture1D.MipLevels = mipLevels;

    hr = pDevice->CreateShaderResourceView(*ppTex1D, &SRVDesc, ppSRV);
    if (FAILED(hr))
    {
        SYS_LOG_E("CreateTexture1DFromSTBI: CreateShaderResourceView failed, HRESULT = 0x%X", hr);
        (*ppTex1D)->Release();
        *ppTex1D = nullptr;
        return FALSE;
    }

    pOutDesc->Width = width;
    pOutDesc->MipLevels = mipLevels;
    pOutDesc->Format = static_cast<COLOR_FORMAT>(dxgiFormat);
	pOutDesc->sRGB = Desc.sRGB;

    return TRUE;
}

BOOL CreateTexture2DFromSTBI(
    HANDLE hTempHeap,
    ID3D11Device* pDevice,
    const TEXTURE2D_CREATE_DESC& Desc,
	TEXTURE2D_DESC* pOutDesc,
    ID3D11Texture2D** ppTex2D,
    ID3D11ShaderResourceView** ppSRV
)
{
    // 1. stb_image 디코딩
    int width = 0, height = 0, channels = 0;
    stbi_uc* pMip0 = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(Desc.pData),
        static_cast<int>(Desc.DataSize),
        &width, 
        &height, 
        &channels,
        STBI_rgb_alpha
    );

    if (!pMip0)
    {
        SYS_LOG_E("CreateTexture2DFromSTBI: stbi_load_from_memory failed. reason = %s",
            stbi_failure_reason());
        return FALSE;
    }

    // 2. DXGI 포맷 결정
    // sRGB == TRUE  → DXGI_FORMAT_R8G8B8A8_UNORM_SRGB (하드웨어 감마 보정)
    // sRGB == FALSE → DXGI_FORMAT_R8G8B8A8_UNORM       (선형 공간, 노멀맵 등)
    const DXGI_FORMAT dxgiFormat = (TRUE == Desc.sRGB)
        ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
        : DXGI_FORMAT_R8G8B8A8_UNORM;

    // 3. 밉 레벨 수 결정
    // Desc.MipLevels == 0 이면 전체 체인 자동 계산
    const UINT maxMips = static_cast<UINT>(
        std::floor(log2f(static_cast<float>(T_MAX(width, height)))) + 1.0f);

    const UINT mipLevels = (Desc.MipLevels > 0)
        ? T_MIN(Desc.MipLevels, maxMips)
        : maxMips;

    // 4. 각 밉 레벨 픽셀 데이터를 CPU에서 생성
	uint8_t* pTempBuffer[MAX_MIPMAP_LEVELS] = { nullptr }; // 각 밉 레벨의 픽셀 데이터를 가리키는 포인터 배열
	D3D11_SUBRESOURCE_DATA SubResourceData[MAX_MIPMAP_LEVELS] = {}; // 각 밉 레벨의 서브리소스 데이터 배열

    // Mip0: stb_image 원본 그대로
	pTempBuffer[0] = (uint8_t*)temppool_alloc(hTempHeap, static_cast<size_t>(width * height * 4));
	memcpy(pTempBuffer[0], pMip0, static_cast<size_t>(width * height * 4));

	SubResourceData[0].pSysMem = pTempBuffer[0];
	SubResourceData[0].SysMemPitch = static_cast<UINT>(width * 4);
	SubResourceData[0].SysMemSlicePitch = 0;

    stbi_image_free(pMip0);
    pMip0 = nullptr;

    // Mip1 ~ MipN
    int srcW = width, srcH = height;
    for (UINT i = 1; i < mipLevels; ++i)
    {
        const int dstW = T_MAX(1, srcW / 2);
        const int dstH = T_MAX(1, srcH / 2);

		pTempBuffer[i] = (uint8_t*)temppool_alloc(hTempHeap, static_cast<size_t>(dstW * dstH * 4));

        if (TRUE == Desc.sRGB)
        {
            // sRGB 텍스처 (디퓨즈, 알베도 등)
            // → 감마 공간을 선형으로 변환 후 필터링, 다시 sRGB로 변환
            // → 밝기가 올바르게 유지됨
            stbir_resize_uint8_srgb(
                pTempBuffer[i - 1], srcW, srcH, srcW * 4,
                pTempBuffer[i], dstW, dstH, dstW * 4,
                STBIR_RGBA
            );
        }
        else
        {
            // 선형 텍스처 (노멀맵, 러프니스, 메탈릭, 마스크 등)
            // → 감마 보정 없이 선형 공간에서 직접 필터링
            stbir_resize_uint8_linear(
                pTempBuffer[i - 1], srcW, srcH, srcW * 4,
                pTempBuffer[i], dstW, dstH, dstW * 4,
                STBIR_RGBA
            );
        }

        SubResourceData[i].pSysMem = pTempBuffer[i];
        SubResourceData[i].SysMemPitch = static_cast<UINT>(dstW * 4);
        SubResourceData[i].SysMemSlicePitch = 0;

        srcW = dstW;
        srcH = dstH;
    }

    // 5. Texture2D 생성
    D3D11_TEXTURE2D_DESC TexDesc = {};
    TexDesc.Width = static_cast<UINT>(width);
    TexDesc.Height = static_cast<UINT>(height);
    TexDesc.MipLevels = mipLevels;
    TexDesc.ArraySize = 1;
    TexDesc.Format = dxgiFormat;
    TexDesc.SampleDesc.Count = 1;
    TexDesc.SampleDesc.Quality = 0;
    TexDesc.Usage = D3D11_USAGE_DEFAULT;
    TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TexDesc.CPUAccessFlags = 0;
    TexDesc.MiscFlags = 0;

    HRESULT hr = pDevice->CreateTexture2D(&TexDesc, SubResourceData, ppTex2D);
    if (FAILED(hr))
    {
        SYS_LOG_E("CreateTexture2DFromSTBI: CreateTexture2D failed, HRESULT = 0x%X", hr);
        return FALSE;
    }

    // 6. SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = dxgiFormat;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = mipLevels;

    hr = pDevice->CreateShaderResourceView(*ppTex2D, &SRVDesc, ppSRV);
    if (FAILED(hr))
    {
        SYS_LOG_E("CreateTexture2DFromSTBI: CreateShaderResourceView failed, HRESULT = 0x%X", hr);
        (*ppTex2D)->Release();
        *ppTex2D = nullptr;
        return FALSE;
    }

    pOutDesc->Width = width;
    pOutDesc->Height = height;
    pOutDesc->MipLevels = mipLevels;
	pOutDesc->Format = static_cast<COLOR_FORMAT>(dxgiFormat);

    return TRUE;
}


