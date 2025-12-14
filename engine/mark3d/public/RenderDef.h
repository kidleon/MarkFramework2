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

enum class PRIMITIVE_TYPE : unsigned int
{
	UNKNOWN = 0,
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	MAX
};

/**
* @brief 셰이더 타입 열거형
*/
enum class SHADER_TYPE : unsigned int
{
	UNKNOWN = 0,
	VERTEX,
	PIXEL,
	COMPUTE,

	MAX
};

/**
* @brief 버퍼 사용 용도 열거형
*/
enum class BUFFER_USAGE : unsigned int
{
	DEFAULT = 0, 
	DYNAMIC, 

	EMAX
};

/**
* @brief 버텍스 포맷 열거형
*/
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

/**
* @brief 버텍스 포맷 인덱스 열거형
*/
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

/**
* @brief 버텍스 포맷 스트라이드 열거형
*/
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

static constexpr UINT32 MAX_VERTEX_FORMAT = 16; // 최대 버텍스 포맷 수
static constexpr int32 MAX_RENDER_PASS = 4; // 서피스 메테리얼의 최대 렌더 패스 수 (추가 패스는 별도의 메테리얼로 구현)
static constexpr int32 MAX_TEXTURE_SLOT = 16; // 최대 텍스처 슬롯 수
static constexpr int32 MAX_SAMPLER_SLOT = 8; // 최대 샘플러 슬롯 수

/**
* @brief 텍스쳐 타입
*/
enum class TEXTURE_TYPE : UINT32
{
	TEX_1D = 0, // 1D 텍스처
	TEX_2D = 1, // 2D 텍스처
	TEX_3D = 2, // 3D 텍스처
	TEX_CUBE = 3, // 큐브 맵 텍스처
};

/**
* @brief 샘플러 필터 열거형
*/
enum class SAMPLER_FILTER : UINT8
{
	NONE = 0,
	NEAREST,
	LINEAR,

	EMAX
};


/**
* @brief 텍스처 주소 모드 열거형
*/
enum class TEXTURE_ADDRESS_MODE : UINT8
{
	WRAP = 0,
	MIRROR,
	CLAMP,
	BORDER,
	MIRROR_ONCE,

	EMAX
};

enum class BORDER_COLOR : UINT8
{
	TRANSPARENT_BLACK = 0,
	OPAQUE_BLACK,
	OPAQUE_WHITE,
	CUSTOM,

	EMAX
};

/**
* @brief 비교 함수 열거형
*/
enum class COMPARISON_FUNC : UINT32
{
	NEVER = 0,
	LESS,
	EQUAL,
	LESS_EQUAL,
	GREATER,
	NOT_EQUAL,
	GREATER_EQUAL,
	ALWAYS,

	EMAX
};

/**
* @brief 블렌드 팩터 열거형
*/
enum class BLEND_FACTOR : UINT8
{
	ZERO = 0,
	ONE,
	SRC_COLOR,
	INV_SRC_COLOR,
	DST_COLOR,
	INV_DST_COLOR,
	SRC_ALPHA,
	INV_SRC_ALPHA,
	DST_ALPHA,
	INV_DST_ALPHA,
	CONSTANT_COLOR,
	INV_CONSTANT_COLOR,
	CONSTANT_ALPHA,
	INV_CONSTANT_ALPHA,
	SRC_ALPHA_SATURATE,
	
	SRC1_COLOR, // Dual source blending (D3D11.1+, OpenGL 3.3+, Vulkan)
	INV_SRC1_COLOR,
	SRC1_ALPHA,
	INV_SRC1_ALPHA,

	EMAX
};

/**
* @brief 블렌드 연산자 열거형
*/
enum class BLEND_OP : UINT8
{
	ADD = 0,
	SUBTRACT,
	REV_SUBTRACT,
	MIN,
	MAX,

	EMAX
};

// 컬러 쓰기 마스크 (비트 플래그)
enum COLOR_WRITE_MASK : UINT8
{
	DISABLE = 0x00,
	RED = 0x01,
	GREEN = 0x02,
	BLUE = 0x04,
	ALPHA = 0x08,
	RGB = RED | GREEN | BLUE,
	ALL = RED | GREEN | BLUE | ALPHA,
};

/**
* @brief 채우기 모드 열거형
*/
enum class FILL_MODE : UINT8
{
	SOLID = 0,
	WIREFRAME,

	EMAX
};

/**
* @brief 컬링 모드 열거형
*/
enum class CULL_MODE : UINT8
{
	NONE = 0,
	CW,
	CCW,

	EMAX
};

// 보수적 래스터화 모드 (D3D12, Vulkan 확장)
enum class CONSERVATIVE_RASTER_MODE : UINT8
{
	OFF = 0,
	ON = 1,
};

/**
* @brief 깊이 쓰기 마스크 열거형
*/
enum class DEPTH_WRITE_MASK : UINT8
{
	ZERO = 0,
	ALL,

	EMAX
};

/**
* @brief 깊이 비교 함수 열거형
*/
enum class DEPTH_FUNC : UINT8
{
	NEVER = 0,
	LESS,
	EQUAL,
	LESS_EQUAL,
	GREATER,
	NOT_EQUAL,
	GREATER_EQUAL,
	ALWAYS,

	EMAX
};

typedef DEPTH_FUNC STENCIL_FUNC;

/**
* @brief 스텐실 연산 열거형
*/
enum class STENCIL_OP : UINT8
{
	KEEP = 0,
	ZERO,
	REPLACE,
	INCR_SAT,
	DECR_SAT,
	INVERT,
	INCR_WRAP,
	DECR_WRAP,

	EMAX
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


// 렌더 상태 구조체들



#endif // __RENDER_DEF_H__