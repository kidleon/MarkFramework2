#ifndef __RENDER_DEF_H__
#define __RENDER_DEF_H__

#include "mathlib.h"

/**
* @brief 렌더링 API 열거형
*/
enum class RENDER_API : UINT32
{
	UNKNOWN = 0, // 알 수 없음
	D3D11 = 1, // Direct3D 11
	D3D12 = 2, // Direct3D 12
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

	
	EMAX,

	FORCE_UINT = 0xffffffff
};

// 각 COLOR_FORMAT에 대한 바이트 크기 (픽셀당)
constexpr static size_t FORMAT_SIZE[] = {
	0,   // UNKNOWN
	16,  // R32G32B32A32_TYPELESS
	16,  // R32G32B32A32_FLOAT
	16,  // R32G32B32A32_UINT
	16,  // R32G32B32A32_SINT
	12,  // R32G32B32_TYPELESS
	12,  // R32G32B32_FLOAT
	12,  // R32G32B32_UINT
	12,  // R32G32B32_SINT
	8,   // R16G16B16A16_TYPELESS
	8,   // R16G16B16A16_FLOAT
	8,   // R16G16B16A16_UNORM
	8,   // R16G16B16A16_UINT
	8,   // R16G16B16A16_SNORM
	8,   // R16G16B16A16_SINT
	8,   // R32G32_TYPELESS
	8,   // R32G32_FLOAT
	8,   // R32G32_UINT
	8,   // R32G32_SINT
	8,   // R32G8X24_TYPELESS
	8,   // D32_FLOAT_S8X24_UINT
	8,   // R32_FLOAT_X8X24_TYPELESS
	8,   // X32_TYPELESS_G8X24_UINT
	4,   // R10G10B10A2_TYPELESS
	4,   // R10G10B10A2_UNORM
	4,   // R10G10B10A2_UINT
	4,   // R11G11B10_FLOAT
	4,   // R8G8B8A8_TYPELESS
	4,   // R8G8B8A8_UNORM
	4,   // R8G8B8A8_UNORM_SRGB
	4,   // R8G8B8A8_UINT
	4,   // R8G8B8A8_SNORM
	4,   // R8G8B8A8_SINT
	4,   // R16G16_TYPELESS
	4,   // R16G16_FLOAT
	4,   // R16G16_UNORM
	4,   // R16G16_UINT
	4,   // R16G16_SNORM
	4,   // R16G16_SINT
	4,   // R32_TYPELESS
	4,   // D32_FLOAT
	4,   // R32_FLOAT
	4,   // R32_UINT
	4,   // R32_SINT
	4,   // R24G8_TYPELESS
	4,   // D24_UNORM_S8_UINT
	4,   // R24_UNORM_X8_TYPELESS
	4,   // X24_TYPELESS_G8_UINT
	2,   // R8G8_TYPELESS
	2,   // R8G8_UNORM
	2,   // R8G8_UINT
	2,   // R8G8_SNORM
	2,   // R8G8_SINT
	2,   // R16_TYPELESS
	2,   // R16_FLOAT
	2,   // D16_UNORM
	2,   // R16_UNORM
	2,   // R16_UINT
	2,   // R16_SNORM
	2,   // R16_SINT
	1,   // R8_TYPELESS
	1,   // R8_UNORM
	1,   // R8_UINT
	1,   // R8_SNORM
	1,   // R8_SINT
	1,   // A8_UNORM
	0,   // R1_UNORM (비트 단위, 8픽셀당 1바이트)
};

inline size_t GetPixelFormatSize(COLOR_FORMAT format) 
{
	size_t index = static_cast<size_t>(format);
	if (index < sizeof(FORMAT_SIZE) / sizeof(FORMAT_SIZE[0])) {
		return FORMAT_SIZE[index];
	}
	return 0;
}

// 텍스처 전체 크기 계산
inline size_t CalculateTextureSize(COLOR_FORMAT format, size_t width, size_t height) 
{
	size_t pixelSize = GetPixelFormatSize(format);
	if (pixelSize == 0) return 0;  // UNKNOWN이나 특수 포맷
	return width * height * pixelSize;
}

enum class PRIMITIVE_TYPE : uint32
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
enum class SHADER_TYPE : uint32
{
	UNKNOWN = 0,
	VERTEX,
	PIXEL,
	GEOMETRY,
	COMPUTE,

	MAX
};

/**
* @brief 버퍼 사용 용도 열거형
*/
enum class BUFFER_USAGE : uint32
{
	DEFAULT = 0,
	IMMUTABLE,
	DYNAMIC, 
	STAGING,

	EMAX
};

/**
* @brief 버퍼 클리어 옵션 열거형
*/
enum class CLEAR_BUFFER : unsigned int
{
	COLOR = 0x00000001u,
	DEPTH = 0x00000002u,
	STENCIL = 0x00000004u,
	ALL = COLOR | DEPTH | STENCIL
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
static constexpr int32 MAX_BLEND_TARGET = 8; // 최대 블렌드 타겟 수 (MRT 지원)
static constexpr uint32 MAX_PRIMITIVE = 32; // 최대 프리미티브 수

//static constexpr int32 MAX_TEXTURE_SLOT = 16; // 최대 텍스처 슬롯 수
static constexpr int32 MAX_SAMPLER_SLOT = 16; // 최대 샘플러 슬롯 수
static constexpr int32 MAX_CONSTANT_SLOT = 14; // 최대 상수 버퍼 슬롯 수
static constexpr int32 MAX_ANISOTROPY_LEVEL = 16; // 최대 이방성 필터링 레벨

/**
* @brief 텍스쳐 타입
*/
enum class TEXTURE_TYPE : UINT32
{
	TEX_UNKNOWN = 0,
	TEX_1D, // 1D 텍스처
	TEX_2D, // 2D 텍스처
	TEX_3D, // 3D 텍스처
	TEX_CUBE, // 큐브 맵 텍스처
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
	ALL = RED | GREEN | BLUE | ALPHA,
};

constexpr uint8 MAX_COLOR_WRITE_MASK = 6;

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
enum class COMPARISON_FUNC : UINT8
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

typedef COMPARISON_FUNC STENCIL_FUNC;

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
* @brief 리소스 사용 용도 열거형
*/
enum class RESOURCE_USAGE : UINT32
{
	STATIC = 0,
	FIXED = 1,
	DYNAMIC = 2,
	RW_BUFFER = 3,
	EMAX
};

/**
* @brief 카메라 모드
*/
enum class CAMERA_MODE : unsigned int
{
	PERSPECTIVE = 0,
	ORTHO,
};

/**
* @brief 렌더 명령어 타입 열거형
*/
enum class COMMAND_TYPE : unsigned int
{
	NONE = 0,
	CLEAR_RENDER_TARGET = 0x00000001,
	SET_RENDER_TARGET = 0x00000002,
	DRAW_MESH = 0x00000004,
};

enum class RENDER_QUEUE_TYPE : unsigned int
{
	RQ_OPAQUE = 0,
	RQ_TRANSPARENT,
	EMAX
};

/**
* @brief 3D 엔진 생성 정보 구조체
* @remark CreateEngine 함수에 전달되는 구조체
* @note Width : 화면 너비
* @note Height : 화면 높이
* @note Fullscreen : 전체 화면 모드 여부
*/
struct MARKENGINE_API ENGINE_CREATE_DESC
{
#if defined(__TARGET_OS_WINDOWS)
	HWND hWnd; // 윈도우 핸들
#endif // __TARGET_OS_WINDOWS
	RENDER_API RenderAPI; // 렌더링 API
	UINT32 ScreenWidth; // 화면 너비
	UINT32 ScreenHeight; // 화면 높이

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


struct RENDERCAMERA_CREATE_DESC
{
	CAMERA_MODE CameraMode; // 카메라 모드
	FLOAT FOVY; // 시야각 (라디안 단위, 원근 카메라에만 해당)
	FLOAT AspectRatio; // 종횡비 (원근 카메라에만 해당)
	FLOAT NearZ; // 근평면 거리
	FLOAT FarZ; // 원평면 거리
	FLOAT OrthoWidth; // 직교 카메라의 너비 (직교 카메라에만 해당)
	FLOAT OrthoHeight; // 직교 카메라의 높이 (직교 카메라에만 해당)

	FLOAT4 ClearColor; // 클리어 색상
	UINT32 ClearFlags; // 클리어 플래그 (CLEAR_BUFFER 열거형의 비트 플래그 조합)
	FLOAT Depth; // 깊이 값 (렌더링 순서에 사용)
	UINT8 Stencil; // 스텐실 값
	UINT8 CameraOrder; // 카메라 렌더링 순서
	UINT8 PADDING[2];
};

struct PRIMITIVEBUFFER_CREATE_DESC
{
	size_t VertexBufferSize;
	size_t IndexBufferSize;
	BUFFER_USAGE Usage;
	BOOL IsInitialData;
	void* pInitialVertexData;
	void* pInitialIndexData;
	size_t InitialVertexDataSize;
	size_t InitialIndexDataSize;
};


constexpr size_t MAX_SHADER_DEFINE = 32;
constexpr size_t MAX_SHADER_DEFINE_LENGTH = 64;
struct SHADER_PROGRAM_CREATE_DESC
{
	char szDebugName[64];
	NameHash ShaderName;
	SHADER_TYPE ShaderType;
	char pEntryPoint[32];
	char pTargetProfile[32];
	char szShaderDefines[MAX_SHADER_DEFINE][MAX_SHADER_DEFINE_LENGTH];
	void* pShaderSource;
	size_t ShaderSourceSize;
};

struct RENDER_TRANSFORM
{
	FLOAT4 LocalPosition;
	QUATERNION LocalRotation;
	FLOAT4 LocalScale;

	MATRIX4 LocalTM;
	MATRIX4 WorldTM;
	MATRIX4 InvWorldTM;
};

struct MARKENGINE_API RENDER_SETTINGS
{
	BOOL VSyncEnabled; // 수직 동기화 활성화 여부

	RENDER_SETTINGS()
		: VSyncEnabled(FALSE)
	{
	}
};

/**
* @brief 상수 버퍼 인터페이스
*/
struct IConstantBuffer : public IAsset
{
	/**
	* @brief 데이터 업데이트.
	* @param pData 데이터 포인터
	* @param DataSize 데이터 크기 (바이트 단위)
	* @remark 내부 버퍼에 데이터를 복사합니다.
	* @note pData가 nullptr이거나 DataSize가 0이면 동작하지 않습니다.
	* @note DataSize가 내부 버퍼 크기보다 크면 동작하지 않습니다.
	* @return 없음
	*/
	virtual void UpdateData(void* pData, size_t DataSize) = 0;

};


/**
* @brief 셰이더 프로그램 인터페이스
*/
struct IShaderProgram : IAsset
{
public:
	virtual SHADER_TYPE GetShaderType() const noexcept = 0;

};

struct ITexture1D : public IAsset
{
	/**
	* @brief 텍스처 너비 반환
	* @return 텍스처 너비
	*/
	virtual uint32 GetWidth() const noexcept = 0;

	/**
	* @brief MIP 레벨 수 반환
	* @return MIP 레벨 수
	*/
	virtual uint32 GetMipLevels() const noexcept = 0;

	/**
	* @brief 텍스처 컬러 포맷 반환
	* @return 텍스처 컬러 포맷
	*/
	virtual COLOR_FORMAT GetFormat() const noexcept = 0;

};


struct ITexture2D : public IAsset
{
	/**
	* @brief 텍스처 너비 반환
	* @return 텍스처 너비
	*/
	virtual uint32 GetWidth() const noexcept = 0;

	/**
	* @brief 텍스처 높이 반환
	* @return 텍스처 높이
	*/
	virtual uint32 GetHeight() const noexcept = 0;

	/**
	* @brief MIP 레벨 수 반환
	* @return MIP 레벨 수
	*/
	virtual uint32 GetMipLevels() const noexcept = 0;

	/**
	* @brief 텍스처 컬러 포맷 반환
	* @return 텍스처 컬러 포맷
	*/
	virtual COLOR_FORMAT GetFormat() const noexcept = 0;

};

/**
* @brief 렌더 타겟 인터페이스
*/
struct IRenderTarget : public IAsset
{
	virtual UINT32 GetColorWidth() const noexcept = 0;
	virtual UINT32 GetColorHeight() const noexcept = 0;
	virtual COLOR_FORMAT GetColorFormat() const noexcept = 0;

	virtual BOOL HasDepthStencil() const noexcept = 0;
	virtual UINT32 GetDepthWidth() const noexcept = 0;
	virtual UINT32 GetDepthHeight() const noexcept = 0;
	virtual COLOR_FORMAT GetDepthFormat() const noexcept = 0;

};

/**
* @brief 샘플러 상태 구조체
*/
struct MARKENGINE_API RS_SAMPLER_STATE
{
	static RS_SAMPLER_STATE LINEAR_WRAP; // 기본 선형 필터링 (일반)
	static RS_SAMPLER_STATE POINT_WRAP; // 포인트 필터링 (픽셀 그래픽)
	static RS_SAMPLER_STATE ANISOTROPIC_WRAP; // 이방성 필터링 16레벨 (고품질)
	static RS_SAMPLER_STATE LINEAR_CLAMP; // 선형 필터링 + 클램프 (UI, 스카이박스)
	static RS_SAMPLER_STATE SHADOW_MAP; // 섀도우 맵 전용 샘플러

	SAMPLER_FILTER MinFilter; // Min 필터
	SAMPLER_FILTER MagFilter; // Mag 필터
	SAMPLER_FILTER MipFilter; // Mipmap 필터
	uint8_t MaxAnisotropy; // 1~16 (0 = 비활성화)

	TEXTURE_ADDRESS_MODE AddressU; // U 축 주소 모드
	TEXTURE_ADDRESS_MODE AddressV; // V 축 주소 모드
	TEXTURE_ADDRESS_MODE AddressW; // W 축 주소 모드
	BORDER_COLOR BorderColor; // Border 색상
	FLOAT4 CustomBorderColor; // 사용자 정의 Border 색상 (BorderColor가 CUSTOM일 때 사용)

	COMPARISON_FUNC ComparisonFunc; // NEVER = 비활성화
	UINT8 PADDINGs[3];

	float MipLODBias;	// 기본값: 0.0f
	float MinLOD;       // 기본값: 0.0f
	float MaxLOD;       // 기본값: FLT_MAX

	constexpr RS_SAMPLER_STATE()
		: MinFilter(SAMPLER_FILTER::LINEAR)
		, MagFilter(SAMPLER_FILTER::LINEAR)
		, MipFilter(SAMPLER_FILTER::LINEAR)
		, MaxAnisotropy(0)
		, AddressU(TEXTURE_ADDRESS_MODE::WRAP)
		, AddressV(TEXTURE_ADDRESS_MODE::WRAP)
		, AddressW(TEXTURE_ADDRESS_MODE::WRAP)
		, BorderColor(BORDER_COLOR::OPAQUE_BLACK)
		, ComparisonFunc(COMPARISON_FUNC::NEVER)
		, CustomBorderColor({ 0.0f, 0.0f, 0.0f, 0.0f })
		, PADDINGs{ 0, 0, 0 }
		, MipLODBias(0.0f)
		, MinLOD(0.0f)
		, MaxLOD(3.402823466e+38F) // FLT_MAX
	{
	}

	// 비교 연산자
	__FORCEINLINE bool operator==(const RS_SAMPLER_STATE& other) const
	{
		return MinFilter == other.MinFilter &&
			MagFilter == other.MagFilter &&
			MipFilter == other.MipFilter &&
			MaxAnisotropy == other.MaxAnisotropy &&
			AddressU == other.AddressU &&
			AddressV == other.AddressV &&
			AddressW == other.AddressW &&
			BorderColor == other.BorderColor &&
			ComparisonFunc == other.ComparisonFunc &&
			MipLODBias == other.MipLODBias &&
			MinLOD == other.MinLOD &&
			MaxLOD == other.MaxLOD;
	}

	__FORCEINLINE bool operator!=(const RS_SAMPLER_STATE& other) const
	{
		return !(*this == other);
	}


	// 이방성 필터링 활성화 여부
	__FORCEINLINE BOOL8 EnabledAnisotropic() const
	{
		return (MaxAnisotropy >= 2);
	}

	// 비교 함수 활성화 여부
	__FORCEINLINE BOOL8 EnabledComparison() const
	{
		return ComparisonFunc != COMPARISON_FUNC::NEVER;
	}

	// Mipmap 필터링 활성화 여부
	__FORCEINLINE BOOL EnabledMipmap() const
	{
		return MipFilter != SAMPLER_FILTER::NONE;
	}

};

/**
* @brief 블렌드 타겟 구조체
*/
struct MARKENGINE_API RS_BLEND_TARGET
{
	static RS_BLEND_TARGET NO_BLEND; // 블렌드 비활성화
	static RS_BLEND_TARGET ALPHA_BLEND; // 알파 블렌드 (전통적인 알파효과, Result = SrcAlpha * Src + (1 - SrcAlpha) * Dst)
	static RS_BLEND_TARGET PREMUL_ALPHA_BLEND; // PREMULTIPLIED 알파 블렌드 (Result = Src + (1 - SrcAlpha) * Dst)
	static RS_BLEND_TARGET ADDITIVE_BLEND; // Additive 블렌드 (파티클, 라이팅, Result = Src + Dst)
	static RS_BLEND_TARGET MULTIPLY_BLEND; // Multiply 블렌드 (어두운 효과, Result = Src * Dst)
	static RS_BLEND_TARGET MULTIPLY2X_BLEND; // Multiply 2X 블렌드 (밝은 효과, Result = 2 * Src * Dst)
	static RS_BLEND_TARGET SCREEN_BLEND; // Screen 블렌드 (밝은 효과, Result = 1 - (1 - Src) * (1 - Dst))
	static RS_BLEND_TARGET MIN_BLEND; // 최소값 블렌드 (특수효과, Result = min(Src, Dst))
	static RS_BLEND_TARGET MAX_BLEND; // 최대값 블렌드 (특수효과, Result = max(Src, Dst))
	static RS_BLEND_TARGET ALPHA_ONLY; // 알파 채널만 쓰기
	static RS_BLEND_TARGET COLOR_ONLY; // 컬러 채널만 쓰기
	static RS_BLEND_TARGET DEPTH_ONLY; // 깊이 채널만 쓰기
	static RS_BLEND_TARGET ALPHA_BLEND_ADD; // RGB: 알파 블렌드, Alpha: Additive 블렌드

	union
	{
		struct
		{
			BOOL8 BlendEnable; // 블렌드 활성화

			// RGB 블렌딩
			BLEND_FACTOR SrcBlend;
			BLEND_FACTOR DestBlend;
			BLEND_OP BlendOp;

			// Alpha 블렌딩
			BLEND_FACTOR SrcBlendAlpha;
			BLEND_FACTOR DestBlendAlpha;
			BLEND_OP BlendOpAlpha;
			UINT8 RenderTargetWriteMask; // 쓰기 마스크
		};

		uint64 data; // 전체 데이터를 하나의 64비트 정수로 접근
	};

	constexpr RS_BLEND_TARGET()
		: BlendEnable(FALSE)
		, SrcBlend(BLEND_FACTOR::ONE)
		, DestBlend(BLEND_FACTOR::ZERO)
		, BlendOp(BLEND_OP::ADD)
		, SrcBlendAlpha(BLEND_FACTOR::ONE)
		, DestBlendAlpha(BLEND_FACTOR::ZERO)
		, BlendOpAlpha(BLEND_OP::ADD)
		, RenderTargetWriteMask(COLOR_WRITE_MASK::ALL)
	{
	}

	__FORCEINLINE bool operator==(const RS_BLEND_TARGET& other) const
	{
		/*
		return BlendEnable == other.BlendEnable &&
			SrcBlend == other.SrcBlend &&
			DestBlend == other.DestBlend &&
			BlendOp == other.BlendOp &&
			SrcBlendAlpha == other.SrcBlendAlpha &&
			DestBlendAlpha == other.DestBlendAlpha &&
			BlendOpAlpha == other.BlendOpAlpha &&
			RenderTargetWriteMask == other.RenderTargetWriteMask;
			*/

		return data == other.data;
	}

	__FORCEINLINE bool operator!=(const RS_BLEND_TARGET& other) const
	{
		return !(*this == other);
	}

	// 블렌드 비활성화 여부 확인
	__FORCEINLINE BOOL8 IsDisabledBlend() const
	{
		return !BlendEnable ||
			(SrcBlend == BLEND_FACTOR::ONE &&
				DestBlend == BLEND_FACTOR::ZERO &&
				SrcBlendAlpha == BLEND_FACTOR::ONE &&
				DestBlendAlpha == BLEND_FACTOR::ZERO);
	}

	// RGB와 Alpha가 같은 블렌드 설정인지 확인
	__FORCEINLINE  BOOL8 IsSeparateAlphaBlend() const
	{
		return (SrcBlend != SrcBlendAlpha) ||
			(DestBlend != DestBlendAlpha) ||
			(BlendOp != BlendOpAlpha);
	}

	// 특정 채널 쓰기 가능 여부
	__FORCEINLINE  BOOL IsColorWriteEnabled(COLOR_WRITE_MASK channel) const
	{
		return (RenderTargetWriteMask & channel) != 0;
	}

	// Premultiplied alpha 사용 여부 체크
	__FORCEINLINE  BOOL IsPremultipliedAlpha() const
	{
		return BlendEnable &&
			SrcBlend == BLEND_FACTOR::ONE &&
			DestBlend == BLEND_FACTOR::INV_SRC_ALPHA;
	}

	// Additive blending 여부
	__FORCEINLINE  BOOL IsAdditiveBlending() const
	{
		return BlendEnable &&
			SrcBlend == BLEND_FACTOR::ONE &&
			DestBlend == BLEND_FACTOR::ONE &&
			BlendOp == BLEND_OP::ADD;
	}
};

/**
* @brief 블렌드 상태 구조체
*/
struct MARKENGINE_API RS_BLEND_STATE
{
	static RS_BLEND_STATE DEFAULT; // 기본 블렌드 상태

	INT32 NumBlendTargets; // 블렌드 타겟 수
	BOOL AlphaToCoverageEnable; // 알파 투 커버리지 활성화 여부 
	BOOL IndependentBlendEnable; // 독립적 블렌드 활성화 여부 (MRT용..)
	UINT32 PADDING;
	RS_BLEND_TARGET BlendTarget[MAX_BLEND_TARGET]; // 블렌드 타겟 배열 (최대 8개:D3D11 기준)
	
	constexpr RS_BLEND_STATE()
		: NumBlendTargets(1)
		, AlphaToCoverageEnable(FALSE)
		, IndependentBlendEnable(FALSE)
		, PADDING(0)
		, BlendTarget{}
	{
	}
};

inline RS_BLEND_STATE GetBS_Default()
{
	RS_BLEND_STATE bs;
	bs.NumBlendTargets = 1;
	bs.AlphaToCoverageEnable = FALSE;
	bs.IndependentBlendEnable = FALSE;
	bs.BlendTarget[0] = RS_BLEND_TARGET::NO_BLEND;
	return bs;
}

/**
* @brief 스텐실 연산 설명 구조체
*/
struct MARKENGINE_API RS_STENCIL_OP
{
	STENCIL_OP StencilFailOp; // 스텐실 테스트 실패 시
	STENCIL_OP StencilDepthFailOp; // 스텐실 통과, 깊이 실패 시
	STENCIL_OP StencilPassOp; // 둘 다 통과 시
	STENCIL_FUNC StencilFunc; // 스텐실 비교 함수

	constexpr RS_STENCIL_OP()
		: StencilFailOp(STENCIL_OP::KEEP)
		, StencilDepthFailOp(STENCIL_OP::KEEP)
		, StencilPassOp(STENCIL_OP::KEEP)
		, StencilFunc(STENCIL_FUNC::ALWAYS)
	{
	}

	constexpr RS_STENCIL_OP(
		STENCIL_OP failOp,
		STENCIL_OP depthFailOp,
		STENCIL_OP passOp,
		STENCIL_FUNC func
	)
		: StencilFailOp(failOp)
		, StencilDepthFailOp(depthFailOp)
		, StencilPassOp(passOp)
		, StencilFunc(func)
	{
	}

	// 비교 연산자
	__FORCEINLINE bool operator==(const RS_STENCIL_OP& other) const
	{
		return StencilFailOp == other.StencilFailOp &&
			StencilDepthFailOp == other.StencilDepthFailOp &&
			StencilPassOp == other.StencilPassOp &&
			StencilFunc == other.StencilFunc;
	}

	__FORCEINLINE bool operator!=(const RS_STENCIL_OP& other) const
	{
		return !(*this == other);
	}

};

/**
* @brief 깊이-스텐실 상태 구조체
*/
struct MARKENGINE_API RS_DEPTH_STENCIL_STATE
{
	static RS_DEPTH_STENCIL_STATE DEFAULT; // 기본 깊이-스텐실 상태
	static RS_DEPTH_STENCIL_STATE DEPTH_READ_ONLY; // 깊이 읽기 전용 (투명 오브젝트용)
	static RS_DEPTH_STENCIL_STATE DEPTH_WRITE_ONLY; // 깊이 쓰기 전용 (깊이 프리패스용, 배경 오브젝트용)
	static RS_DEPTH_STENCIL_STATE DEPTH_DISABLED; // 깊이 테스트 비활성화 (UI, 파티클용, 스카이 박스)
	static RS_DEPTH_STENCIL_STATE DEPTH_EQUAL; // 깊이 비교 함수 EQUAL (라이팅 패스, 포스트 이펙트용)
	static RS_DEPTH_STENCIL_STATE DEPTH_REVERSE_Z; // 역 Z (Reverse-Z) 깊이 버퍼용

	static RS_DEPTH_STENCIL_STATE STENCIL_WRITE; // 스텐실 쓰기 전용 
	static RS_DEPTH_STENCIL_STATE STENCIL_TEST; // 스텐실 테스트 전용
	static RS_DEPTH_STENCIL_STATE STENCIL_INCR; // 스텐실 값 증가
	static RS_DEPTH_STENCIL_STATE STENCIL_DECR; // 스텐실 값 감소
	static RS_DEPTH_STENCIL_STATE STENCIL_TWO_SIDED; // 양면 스텐실 테스트
	static RS_DEPTH_STENCIL_STATE STENCIL_MASK; // 스텐실 마스크 테스트 (모든 픽셀에 스텐실 값을 씀)

	// 깊이 테스트 설정
	BOOL8 DepthEnable;
	BOOL8 DepthWriteEnable;
	COMPARISON_FUNC DepthFunc;
	DEPTH_WRITE_MASK DepthWriteMask;
	UINT8 PADDING_DEPTH;

	// 스텐실 설정
	BOOL8 StencilEnable;
	uint8_t StencilReadMask;
	uint8_t StencilWriteMask;

	uint16_t PADDING;

	// 앞면/뒷면 스텐실 연산
	RS_STENCIL_OP FrontFace;
	RS_STENCIL_OP BackFace;

	constexpr RS_DEPTH_STENCIL_STATE()
		: DepthEnable(FALSE)
		, DepthWriteEnable(FALSE)
		, DepthFunc(COMPARISON_FUNC::ALWAYS)
		, DepthWriteMask(DEPTH_WRITE_MASK::ALL)
		, PADDING_DEPTH(0)
		, StencilEnable(FALSE)
		, StencilReadMask(0xFF)
		, StencilWriteMask(0xFF)
		, PADDING(0)
		, FrontFace()
		, BackFace()
	{
	}

	// 깊이 테스트가 실질적으로 비활성화 상태인지
	__FORCEINLINE BOOL IsDisabledDepthTest() const
	{
		return !DepthEnable || DepthFunc == COMPARISON_FUNC::ALWAYS;
	}

	// 깊이 쓰기 전용 모드
	__FORCEINLINE BOOL IsDepthWriteOnly() const
	{
		return DepthWriteEnable && (!DepthEnable || DepthFunc == COMPARISON_FUNC::ALWAYS);
	}

	// Read-only 깊이 버퍼
	__FORCEINLINE BOOL IsDepthReadOnly() const
	{
		return DepthEnable && !DepthWriteEnable;
	}

	// 스텐실이 실질적으로 비활성화 상태인지
	__FORCEINLINE BOOL IsDisabledStencil() const
	{
		return !StencilEnable ||
			(FrontFace.StencilFunc == STENCIL_FUNC::ALWAYS &&
				BackFace.StencilFunc == STENCIL_FUNC::ALWAYS);
	}

	// 앞면과 뒷면이 다른 스텐실 설정인지
	__FORCEINLINE BOOL IsTwoSidedStencil() const
	{
		return FrontFace != BackFace;
	}

	// 비교 연산자
	__FORCEINLINE bool operator==(const RS_DEPTH_STENCIL_STATE& other) const
	{
		return DepthEnable == other.DepthEnable &&
			DepthWriteEnable == other.DepthWriteEnable &&
			DepthFunc == other.DepthFunc &&
			StencilEnable == other.StencilEnable &&
			StencilReadMask == other.StencilReadMask &&
			StencilWriteMask == other.StencilWriteMask &&
			FrontFace == other.FrontFace &&
			BackFace == other.BackFace;
	}

	__FORCEINLINE bool operator!=(const RS_DEPTH_STENCIL_STATE& other) const
	{
		return !(*this == other);
	}
};

inline RS_DEPTH_STENCIL_STATE GetDSS_Default()
{
	return RS_DEPTH_STENCIL_STATE::DEFAULT;
}

inline RS_DEPTH_STENCIL_STATE GetDSS_DepthReadOnly()
{
	return RS_DEPTH_STENCIL_STATE::DEPTH_READ_ONLY;
}

/**
* @brief 래스터라이저 상태 구조체
*/
struct MARKENGINE_API RS_RASTERIZER_STATE
{
	static RS_RASTERIZER_STATE DEFAULT; // 기본 래스터라이저 상태
	static RS_RASTERIZER_STATE TWO_SIDE; // 양면 렌더링 모드
	static RS_RASTERIZER_STATE WIREFRAME; // 와이어프레임 모드
	static RS_RASTERIZER_STATE TWO_SIDE_WIREFRAME; // 양면 와이어프레임 모드

	// 기본 래스터화 설정
	FILL_MODE FillMode;
	CULL_MODE CullMode;
	CONSERVATIVE_RASTER_MODE ConservativeRaster;
	uint8_t Flags;

	// Depth Bias 설정
	int32_t DepthBias;
	float DepthBiasClamp;
	float SlopeScaledDepthBias;

	// 플래그 비트 정의
	enum FlagBits : uint8_t
	{
		FLAG_FRONT_CCW = 1 << 0, // 앞면이 반시계방향
		FLAG_DEPTH_CLIP_ENABLE = 1 << 1, // 깊이 클리핑 (기본 켜짐)
		FLAG_SCISSOR_ENABLE = 1 << 2, // 가위 영역
		FLAG_MULTISAMPLE_ENABLE = 1 << 3, // MSAA
		FLAG_ANTIALIASED_LINE = 1 << 4, // 라인 AA
		FLAG_DEPTH_BIAS_ENABLE = 1 << 5, // Depth Bias 활성화

		// 기본값
		FLAG_DEFAULT = FLAG_DEPTH_CLIP_ENABLE,
	};

	constexpr RS_RASTERIZER_STATE()
		: FillMode(FILL_MODE::SOLID)
		, CullMode(CULL_MODE::NONE)
		, ConservativeRaster(CONSERVATIVE_RASTER_MODE::OFF)
		, Flags(FLAG_DEFAULT)
		, DepthBias(0)
		, DepthBiasClamp(0.0f)
		, SlopeScaledDepthBias(0.0f)
	{
	}

	constexpr RS_RASTERIZER_STATE(
		FILL_MODE fillMode, 
		CULL_MODE cullMode, 
		CONSERVATIVE_RASTER_MODE 
		conservativeRaster, 
		uint8_t flags, 
		int32_t depthBias,
		float depthBiasClamp, 
		float slopeScaledDepthBias
	)
		: FillMode(fillMode)
		, CullMode(cullMode)
		, ConservativeRaster(conservativeRaster)
		, Flags(flags)
		, DepthBias(depthBias)
		, DepthBiasClamp(depthBiasClamp)
		, SlopeScaledDepthBias(slopeScaledDepthBias)
	{
	}

	__FORCEINLINE BOOL IsFrontCounterClockwise() const
	{
		return (Flags & FLAG_FRONT_CCW) != 0;
	}

	__FORCEINLINE void SetFrontCounterClockwise(bool enable)
	{
		if (enable)
			Flags |= FLAG_FRONT_CCW;
		else
			Flags &= ~FLAG_FRONT_CCW;
	}

	__FORCEINLINE bool IsDepthClipEnabled() const
	{
		return (Flags & FLAG_DEPTH_CLIP_ENABLE) != 0;
	}

	__FORCEINLINE void SetDepthClipEnable(bool enable)
	{
		if (enable)
			Flags |= FLAG_DEPTH_CLIP_ENABLE;
		else
			Flags &= ~FLAG_DEPTH_CLIP_ENABLE;
	}

	__FORCEINLINE bool IsScissorEnabled() const
	{
		return (Flags & FLAG_SCISSOR_ENABLE) != 0;
	}

	__FORCEINLINE void SetScissorEnable(bool enable)
	{
		if (enable)
			Flags |= FLAG_SCISSOR_ENABLE;
		else
			Flags &= ~FLAG_SCISSOR_ENABLE;
	}

	__FORCEINLINE bool IsMultisampleEnabled() const
	{
		return (Flags & FLAG_MULTISAMPLE_ENABLE) != 0;
	}

	__FORCEINLINE void SetMultisampleEnable(bool enable)
	{
		if (enable)
			Flags |= FLAG_MULTISAMPLE_ENABLE;
		else
			Flags &= ~FLAG_MULTISAMPLE_ENABLE;
	}

	__FORCEINLINE bool IsAntialiasedLineEnabled() const
	{
		return (Flags & FLAG_ANTIALIASED_LINE) != 0;
	}

	// Antialiased Line 활성화 설정
	__FORCEINLINE void SetAntialiasedLineEnable(bool enable)
	{
		if (enable)
			Flags |= FLAG_ANTIALIASED_LINE;
		else
			Flags &= ~FLAG_ANTIALIASED_LINE;
	}

	// Depth Bias 활성화 여부
	__FORCEINLINE bool IsDepthBiasEnabled() const
	{
		return (Flags & FLAG_DEPTH_BIAS_ENABLE) != 0;
	}

	// Depth Bias 활성화 설정
	__FORCEINLINE void SetDepthBiasEnable(bool enable)
	{
		if (enable)
			Flags |= FLAG_DEPTH_BIAS_ENABLE;
		else
			Flags &= ~FLAG_DEPTH_BIAS_ENABLE;
	}

	// Depth Bias가 실질적으로 활성화되어 있는지
	__FORCEINLINE bool IsDepthBiasEffectivelyEnabled() const
	{
		return IsDepthBiasEnabled() &&
			(DepthBias != 0 || SlopeScaledDepthBias != 0.0f);
	}

	// Depth Bias 설정 헬퍼
	__FORCEINLINE void SetDepthBias(int32_t bias, float clamp, float slopeScaled)
	{
		DepthBias = bias;
		DepthBiasClamp = clamp;
		SlopeScaledDepthBias = slopeScaled;
		SetDepthBiasEnable(bias != 0 || slopeScaled != 0.0f);
	}

	// Depth Bias 비활성화
	__FORCEINLINE void DisableDepthBias()
	{
		DepthBias = 0;
		DepthBiasClamp = 0.0f;
		SlopeScaledDepthBias = 0.0f;
		SetDepthBiasEnable(false);
	}

	// 와이어프레임 모드인지
	__FORCEINLINE bool IsWireframe() const
	{
		return FillMode == FILL_MODE::WIREFRAME;
	}

	// 양면 렌더링 확인
	__FORCEINLINE bool IsTwoSided() const
	{
		return CullMode == CULL_MODE::NONE;
	}

	// 보수적 래스터화 사용 여부
	__FORCEINLINE bool IsConservativeRasterEnabled() const
	{
		return ConservativeRaster == CONSERVATIVE_RASTER_MODE::ON;
	}

	// 비교 연산자
	__FORCEINLINE bool operator==(const RS_RASTERIZER_STATE& other) const
	{
		return FillMode == other.FillMode &&
			CullMode == other.CullMode &&
			ConservativeRaster == other.ConservativeRaster &&
			Flags == other.Flags &&
			DepthBias == other.DepthBias &&
			DepthBiasClamp == other.DepthBiasClamp &&
			SlopeScaledDepthBias == other.SlopeScaledDepthBias;
	}

	__FORCEINLINE bool operator!=(const RS_RASTERIZER_STATE& other) const
	{
		return !(*this == other);
	}
};

inline RS_RASTERIZER_STATE GetRS_Default()
{
	return RS_RASTERIZER_STATE();
}

inline RS_RASTERIZER_STATE GetRS_TwoSide()
{
	RS_RASTERIZER_STATE state = GetRS_Default();
	state.CullMode = CULL_MODE::NONE;
	return state;
}

inline RS_RASTERIZER_STATE GetRS_Wireframe()
{
	RS_RASTERIZER_STATE state = GetRS_Default();
	state.FillMode = FILL_MODE::WIREFRAME;
	return state;
}

inline RS_RASTERIZER_STATE GetRS_WireframeTwoSide()
{
	RS_RASTERIZER_STATE state = GetRS_Default();
	state.FillMode = FILL_MODE::WIREFRAME;
	state.CullMode = CULL_MODE::NONE;
	return state;
}

/**
* @brief 서피스 메테리얼 인터페이스
*/
struct ISurfaceMaterial : public IAsset
{
public:
	virtual int32 AddPass(const char* szPassName) noexcept = 0;
	virtual int32 GetNumPass() const noexcept = 0;

	virtual void SetVertexShader(int32 Pass, IShaderProgram* pVS) = 0;
	virtual void SetVertexShader(IShaderProgram* pVS) = 0;

	virtual void SetPixelShader(int32 Pass, IShaderProgram* pPS) = 0;
	virtual void SetPixelShader(IShaderProgram* pPS) = 0;

	virtual IShaderProgram* GetVertexShader(int32 Pass) noexcept = 0;
	virtual IShaderProgram* GetVertexShader() noexcept = 0;

	virtual IShaderProgram* GetPixelShader(int32 Pass) noexcept = 0;
	virtual IShaderProgram* GetPixelShader() noexcept = 0;

	virtual void SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState) = 0;
	virtual void SetRasterizerState(const RS_RASTERIZER_STATE& RasterizerState) = 0;

	virtual void SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState) = 0;
	virtual void SetBlendState(const RS_BLEND_STATE& BlendState) = 0;

	virtual void SetBlendFactor(int32 Pass, const FLOAT4& BlendFactor) = 0;
	virtual void SetBlendFactor(const FLOAT4& BlendFactor) = 0;

	virtual void SetSampleMask(int32 Pass, UINT32 SampleMask) = 0;
	virtual void SetSampleMask(UINT32 SampleMask) = 0;

	virtual void SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;
	virtual void SetDepthStencilState(const RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;

	virtual void SetStencilRef(int32 Pass, UINT8 StencilRef) = 0;
	virtual void SetStencilRef(UINT8 StencilRef) = 0;

	virtual void SetColor(int32 Pass, const FLOAT4& Color) = 0;
	virtual void SetColor(const FLOAT4& Color) = 0;

	virtual const FLOAT4& GetColor(int32 Pass) const noexcept = 0;
	virtual const FLOAT4& GetColor() const noexcept = 0;

	/*
	virtual void SetConstantBuffer_VS(int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) = 0;
	virtual void SetConstantBuffer_PS(int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) = 0;
	virtual void SetConstantBuffer_VS(int32 Pass, int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) = 0;
	virtual void SetConstantBuffer_PS(int32 Pass, int32 SlotIndex, const NameHash& Name, IConstantBuffer* pCBuffer) = 0;

	virtual void SetSamplerState(int32 Pass, int32 SamplerIndex, const RS_SAMPLER_STATE& SamplerState) = 0;
	virtual void SetBlendState(int32 Pass, const RS_BLEND_STATE& BlendState) = 0;
	virtual void SetBlendTarget(int32 Pass, int32 BlendTargetIndex, const RS_BLEND_TARGET& BlendTarget) = 0;
	virtual void SetBlendTargetFactorValue(int32 Pass, int32 BlendTargetIndex, const FLOAT4& Factor) = 0;
	virtual void SetDepthStencilState(int32 Pass, const RS_DEPTH_STENCIL_STATE& DepthStencilState) = 0;
	virtual void SetRasterizerState(int32 Pass, const RS_RASTERIZER_STATE& RasterizerState) = 0;

	virtual void SetTexture1D(int32 Pass, int32 SamplerIndex, const NameHash& Name, ITexture1D* pTexture) = 0;
	virtual void SetTexture2D(int32 Pass, int32 SamplerIndex, const NameHash& Name, ITexture2D* pTexture) = 0;
	*/
};


/**
* @brief 프리미티브 버퍼 인터페이스
*/
struct IPrimitiveBuffer : public IAsset
{
	virtual BUFFER_USAGE GetUsage() const noexcept = 0;

	virtual void ResetPrimitive() noexcept = 0;

	virtual INT32 AddPrimitive(
		PRIMITIVE_TYPE PrimitiveType,
		uint32 VertexCount,
		uint32 VertexStride,
		uint32 IndexCount,
		uint32 IndexStride
	) noexcept = 0;

	virtual size_t GetNumPrimitives() const noexcept = 0;

	virtual BOOL UpdateVertex(
		int32 PrimitiveIndex,
		const void* pVertexData,
		size_t VertexSize
	) = 0;

	virtual BOOL UpdateIndex(
		int32 PrimitiveIndex,
		const void* pIndexData,
		size_t IndexSize
	) = 0;

	virtual size_t GetTotalVertexBufferSize() const noexcept = 0;

	virtual size_t GetTotalIndexBufferSize() const noexcept = 0;
};

/**
* @brief 렌더 카메라 인터페이스
*/
struct IRenderCamera : public IAsset
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

	virtual void SetCameraOrder(INT8 Order) noexcept = 0;
};


/**
* @brief 렌더 컨텍스트 인터페이스
*/
struct IRenderContext : public IUNKNOWN
{
	virtual void BeginFrame() noexcept = 0;
	virtual void EndFrame() noexcept = 0;

	virtual void BeginRenderCamera(IRenderCamera* pRenderCamera) noexcept = 0;
	virtual void EndRenderCamera() noexcept = 0;
	
	virtual void SetSurfaceMaterial(ISurfaceMaterial* pSurfaceMaterial) = 0;
	virtual void SetPrimitiveBuffer(IPrimitiveBuffer* pPrimitiveBuffer) = 0;
	
	virtual void DrawPrimitive(const LOCAL_TRANSFORM& Transform, int32 PrimitiveIndex) = 0;

};

/**
* @brief 렌더링 시스템 인터페이스
*/
struct IRenderSystem : public IUNKNOWN
{
public:
	virtual const RENDER_SETTINGS& GetRenderSettings() const noexcept = 0;
	virtual void SetRenderSettings(const RENDER_SETTINGS& Settings) noexcept = 0;

	virtual BOOL CreatePrimitiveBuffer(const PRIMITIVEBUFFER_CREATE_DESC& Desc, IPrimitiveBuffer** ppOut) = 0;
	virtual BOOL CreateRenderCamera(const RENDERCAMERA_CREATE_DESC& Desc, IRenderCamera** ppOut) = 0;
	virtual BOOL CreateSurfaceMaterial(ISurfaceMaterial** ppOut) = 0;
	virtual BOOL GetOrCreateShaderProgram(const SHADER_PROGRAM_CREATE_DESC& Desc, IShaderProgram** ppOut) = 0;

	virtual BOOL GetOrCreateRenderContext(IRenderContext** ppContext) = 0;

	virtual void Update() = 0;

};

#endif // __RENDER_DEF_H__