#ifndef __RENDER_DEF_H__
#define __RENDER_DEF_H__


/**
* @brief 렌더링 API 열거형
*/
enum class RENDER_API : UINT32
{
	UNKNOWN = 0, // 알 수 없음
	DIRECT3D11 = 1, // Direct3D 11
	DIRECT3D12 = 2, // Direct3D 12
	VULKAN = 3, // Vulkan
	OPENGL = 4, // OpenGL
	METAL = 5, // Metal
};

/**
* @brief 색상 포맷 열거형
*/
enum class COLOR_FORMAT : unsigned int
{
	UNKNOWN = 0,
	R32G32B32A32_TYPELESS,
	R32G32B32A32_FLOAT,
	R32G32B32A32_UINT,
	R32G32B32A32_SINT,
	R32G32B32_TYPELESS,
	R32G32B32_FLOAT,
	R32G32B32_UINT,
	R32G32B32_SINT,
	R16G16B16A16_TYPELESS,
	R16G16B16A16_FLOAT,
	R16G16B16A16_UNORM,
	R16G16B16A16_UINT,
	R16G16B16A16_SNORM,
	R16G16B16A16_SINT,
	R32G32_TYPELESS,
	R32G32_FLOAT,
	R32G32_UINT,
	R32G32_SINT,
	R32G8X24_TYPELESS,
	D32_FLOAT_S8X24_UINT,
	R32_FLOAT_X8X24_TYPELESS,
	X32_TYPELESS_G8X24_UINT,
	R10G10B10A2_TYPELESS,
	R10G10B10A2_UNORM,
	R10G10B10A2_UINT,
	R11G11B10_FLOAT,
	R8G8B8A8_TYPELESS,
	R8G8B8A8_UNORM,
	R8G8B8A8_UNORM_SRGB,
	R8G8B8A8_UINT,
	R8G8B8A8_SNORM,
	R8G8B8A8_SINT,
	R16G16_TYPELESS,
	R16G16_FLOAT,
	R16G16_UNORM,
	R16G16_UINT,
	R16G16_SNORM,
	R16G16_SINT,
	R32_TYPELESS,
	D32_FLOAT,
	R32_FLOAT,
	R32_UINT,
	R32_SINT,
	R24G8_TYPELESS,
	D24_UNORM_S8_UINT,
	R24_UNORM_X8_TYPELESS,
	X24_TYPELESS_G8_UINT,
	R8G8_TYPELESS,
	R8G8_UNORM,
	R8G8_UINT,
	R8G8_SNORM,
	R8G8_SINT,
	R16_TYPELESS,
	R16_FLOAT,
	D16_UNORM,
	R16_UNORM,
	R16_UINT,
	R16_SNORM,
	R16_SINT,
	R8_TYPELESS,
	R8_UNORM,
	R8_UINT,
	R8_SNORM,
	R8_SINT,
	A8_UNORM,
	R1_UNORM,
	R9G9B9E5_SHAREDEXP,
	R8G8_B8G8_UNORM,
	G8R8_G8B8_UNORM,
	BC1_TYPELESS,
	BC1_UNORM,
	BC1_UNORM_SRGB,
	BC2_TYPELESS,
	BC2_UNORM,
	BC2_UNORM_SRGB,
	BC3_TYPELESS,
	BC3_UNORM,
	BC3_UNORM_SRGB,
	BC4_TYPELESS,
	BC4_UNORM,
	BC4_SNORM,
	BC5_TYPELESS,
	BC5_UNORM,
	BC5_SNORM,
	B5G6R5_UNORM,
	B5G5R5A1_UNORM,
	B8G8R8A8_UNORM,
	B8G8R8X8_UNORM,
	R10G10B10_XR_BIAS_A2_UNORM,
	B8G8R8A8_TYPELESS,
	B8G8R8A8_UNORM_SRGB,
	B8G8R8X8_TYPELESS,
	B8G8R8X8_UNORM_SRGB,
	BC6H_TYPELESS,
	BC6H_UF16,
	BC6H_SF16,
	BC7_TYPELESS,
	BC7_UNORM,
	BC7_UNORM_SRGB,
	AYUV,
	Y410,
	Y416,
	NV12,
	P010,
	P016,
	OPAQUE_420,
	YUY2,
	Y210,
	Y216,
	NV11,
	AI44,
	IA44,
	P8,
	A8P8,
	B4G4R4A4_UNORM,

	P208,
	V208,
	V408,

	SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
	SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,

	EMAX,

	FORCE_UINT = 0xffffffff
};

enum class SHADER_TYPE : unsigned int
{
	VERTEX,
	PIXEL,
	COMPUTE,

	MAX
};

enum class BUFFER_USAGE : unsigned int
{
	DEFAULT = 0,
	DYNAMIC,

	EMAX
};

enum class VERTEX_FORMAT : unsigned int
{
	UNKNOWN = 0x00000000,
	POSITION = 0x00000001,		// FLOAT3
	NORMAL = 0x00000002,		// FLOAT3
	TEXCOORD = 0x00000004,		// FLOAT2
	COLOR = 0x00000008,			// FLOAT4

	TANGENT = 0x00000010,		// FLOAT3
	BINORMAL = 0x00000020,		// FLOAT3

	BONE = 0x00000040,			// UINT4
	WEIGHT = 0x00000080,		// FLOAT4

	TEXCOORD1 = 0x00000100,		// FLOAT4;
	TEXCOORD2 = 0x00000200,		// FLOAT4;
	TEXCOORD3 = 0x00000400,		// FLOAT4;
	TEXCOORD4 = 0x00000800,		// FLOAT4;
	TEXCOORD5 = 0x00001000,		// FLOAT4;
	TEXCOORD6 = 0x00002000,		// FLOAT4;
	TEXCOORD7 = 0x00004000,		// FLOAT4;
	TEXCOORD8 = 0x00008000,		// FLOAT4;
};

enum class VERTEX_FORMAT_INDEX : unsigned int
{
	POSITION = 0,
	NORMAL,
	TEXCOORD,
	COLOR,

	TANGENT,
	BINORMAL,

	BONE,
	WEIGHT,

	TEXCOORD1,
	TEXCOORD2,
	TEXCOORD3,
	TEXCOORD4,
	TEXCOORD5,
	TEXCOORD6,
	TEXCOORD7,
	TEXCOORD8,

	MAX
};

enum class VERTEX_FORMAT_STRIDE : unsigned int
{
	POSITION = sizeof(FLOAT) * 3,
	NORMAL = sizeof(FLOAT) * 3,
	TEXCOORD = sizeof(FLOAT) * 2,
	COLOR = sizeof(FLOAT) * 4,

	TANGENT = sizeof(FLOAT) * 3,
	BINORMAL = sizeof(FLOAT) * 3,

	BONE = sizeof(UINT32) * 4,
	WEIGHT = sizeof(FLOAT) * 4,

	TEXCOORD1 = sizeof(FLOAT) * 4,
	TEXCOORD2 = sizeof(FLOAT) * 4,
	TEXCOORD3 = sizeof(FLOAT) * 4,
	TEXCOORD4 = sizeof(FLOAT) * 4,
	TEXCOORD5 = sizeof(FLOAT) * 4,
	TEXCOORD6 = sizeof(FLOAT) * 4,
	TEXCOORD7 = sizeof(FLOAT) * 4,
	TEXCOORD8 = sizeof(FLOAT) * 4,
};

static constexpr UINT32 MAX_VERTEX_FORMAT = 16;

enum class TEXTURE_TYPE : UINT32
{
	TEX_1D = 0, // 1D 텍스처
	TEX_2D = 1, // 2D 텍스처
	TEX_3D = 2, // 3D 텍스처
	TEX_CUBE = 3, // 큐브 맵 텍스처
};

/**
* @brief 3D 엔진 생성 정보 구조체
* @remark CreateEngine 함수에 전달되는 구조체
* @note Width : 화면 너비
* @note Height : 화면 높이
* @note Fullscreen : 전체 화면 모드 여부
*/
struct ENGINE_CREATE_DESC
{
#if defined(__TARGET_OS_WINDOWS)
	HWND hWnd; // 윈도우 핸들
#endif // __TARGET_OS_WINDOWS

	UINT32 ScreenWidth; // 화면 너비
	UINT32 ScreenHeight; // 화면 높이
	LOG_LEVEL MinLogLevel; // 최소 로그 레벨
	BOOL Fullscreen; // 전체 화면 모드 여부
};

/**
* @brief 1D 텍스처 생성 정보 구조체
*/
struct TEXTURE1D_CREATE_DESC
{
	TEXTURE_TYPE Type; // 텍스처 타입
	UINT32 Width; // 텍스처 너비
	UINT32 MipLevels; // 밉맵 레벨 수
	COLOR_FORMAT Format; // 색상 포맷
	const char* pData; // 초기 데이터 포인터
	size_t DataSize; // 초기 데이터 크기 (바이트 단위)
};

/**
* @brief 2D 텍스처 생성 정보 구조체
*/
struct TEXTURE2D_CREATE_DESC
{
	TEXTURE_TYPE Type; // 텍스처 타입
	UINT32 Width; // 텍스처 너비
	UINT32 Height; // 텍스처 높이
	UINT32 MipLevels; // 밉맵 레벨 수
	COLOR_FORMAT Format; // 색상 포맷
	const char* pData; // 초기 데이터 포인터
	size_t DataSize; // 초기 데이터 크기 (바이트 단위)
};

enum class FILTER_TYPE : UINT32
{
	MIN_MAG_MIP_POINT = 0,
	MIN_MAG_POINT_MIP_LINEAR = 0x1,
	MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
	MIN_POINT_MAG_MIP_LINEAR = 0x5,
	MIN_LINEAR_MAG_MIP_POINT = 0x10,
	MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
	MIN_MAG_LINEAR_MIP_POINT = 0x14,
	MIN_MAG_MIP_LINEAR = 0x15,
	ANISOTROPIC = 0x55,
	COMPARISON_MIN_MAG_MIP_POINT = 0x80,
	COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
	COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
	COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
	COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
	COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
	COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
	COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
	COMPARISON_ANISOTROPIC = 0xd5,
};

enum class TEXTURE_ADDRESS_MODE : UINT32
{
	WRAP = 1,
	MIRROR = 2,
	CLAMP = 3,
	BORDER = 4,
	MIRROR_ONCE = 5,
};

enum class COMPARISON_FUNC : UINT32
{
	NEVER = 1,
	LESS = 2,
	EQUAL = 3,
	LESS_EQUAL = 4,
	GREATER = 5,
	NOT_EQUAL = 6,
	GREATER_EQUAL = 7,
	ALWAYS = 8,
};

// 렌더 상태 구조체들

/**
* @brief 샘플러 상태 구조체
*/
struct RS_SAMPLER_STATE
{
	FILTER_TYPE Filter; // 필터 타입
	TEXTURE_ADDRESS_MODE AddressU; // U 좌표 주소 모드
	TEXTURE_ADDRESS_MODE AddressV; // V 좌표 주소 모드
	TEXTURE_ADDRESS_MODE AddressW; // W 좌표 주소 모드
	UINT32 MipLODBias; // 밉 레벨 LOD 바이어스
	UINT32 MaxAnisotropy; // 최대 이방성 필터링 값
	COMPARISON_FUNC ComparisonFunc; // 비교 함수
	FLOAT BorderColor[4]; // 경계 색상
	FLOAT MinLOD; // 최소 LOD
	FLOAT MaxLOD; // 최대 LOD
};

/**
* @brief 블렌드 상태 구조체
*/
struct RS_BLEND_STATE
{
	BOOL BlendEnable; // 블렌딩 활성화 여부
	UINT32 SrcBlend; // 소스 블end 팩터
	UINT32 DestBlend; // 대상 블렌드 팩터
	UINT32 BlendOp; // 블렌드 연산자
	UINT32 SrcBlendAlpha; // 소스 알파 블렌드 팩터
	UINT32 DestBlendAlpha; // 대상 알파 블렌드 팩터
	UINT32 BlendOpAlpha; // 알파 블렌드 연산자
	UINT8 RenderTargetWriteMask; // 렌더 타겟 쓰기 마스크
};

/**
* @brief 깊이-스텐실 상태 구조체
*/
struct RS_DEPTH_STENCIL_STATE
{
	BOOL DepthEnable; // 깊이 테스트 활성화 여부
	UINT32 DepthWriteMask; // 깊이 쓰기 마스크
	UINT32 DepthFunc; // 깊이 비교 함수
	BOOL StencilEnable; // 스텐실 테스트 활성화 여부
	UINT8 StencilReadMask; // 스텐실 읽기 마스크
	UINT8 StencilWriteMask; // 스텐실 쓰기 마스크

	// 앞면 스텐실 연산
	UINT32 FrontFaceStencilFailOp; // 앞면 스텐실 실패 시 연산
	UINT32 FrontFaceStencilDepthFailOp; // 앞면 깊이 실패 시 연산
	UINT32 FrontFaceStencilPassOp; // 앞면 모두 통과 시 연산
	UINT32 FrontFaceStencilFunc; // 앞면 스텐실 비교 함수

	// 뒷면 스텐실 연산
	UINT32 BackFaceStencilFailOp; // 뒷면 스텐실 실패 시 연산
	UINT32 BackFaceStencilDepthFailOp; // 뒷면 깊이 실패 시 연산
	UINT32 BackFaceStencilPassOp; // 뒷면 모두 통과 시 연산
	UINT32 BackFaceStencilFunc; // 뒷면 스텐실 비교 함수
};

/**
* @brief 래스터라이저 상태 구조체
*/
struct RS_RASTERIZER_STATE
{
	UINT32 FillMode; // 채우기 모드
	UINT32 CullMode; // 컬링 모드
	BOOL FrontCounterClockwise; // 앞면이 반시계 방향인지 여부
	INT32 DepthBias; // 깊이 바이어스
	FLOAT DepthBiasClamp; // 깊이 바이어스 클램프
	FLOAT SlopeScaledDepthBias; // 기울기 스케일 깊이 바이어스
	BOOL DepthClipEnable; // 깊이 클리핑 활성화 여부
	BOOL ScissorEnable; // 가위 영역 활성화 여부
	BOOL MultisampleEnable; // 멀티샘플링 활성화 여부
	BOOL AntialiasedLineEnable; // 앤티앨리어싱 라인 활성화 여부
	UINT32 ForcedSampleCount; // 강제 샘플 수
	UINT32 ConservativeRasterizationMode; // 보수적 래스터화 모드
};


#endif // __RENDER_DEF_H__