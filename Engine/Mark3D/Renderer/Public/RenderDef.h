#pragma once


namespace mark
{
	/**
	* @brief 렌더링 API 열거형
	*/
	enum class RENDER_API : uint32_t
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
	enum class COLOR_FORMAT : uint32_t
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

	// 인덱스 버퍼에서 사용되는 인덱스 데이터의 형식을 나타내는 열거형
	enum class INDEX_FORMAT : uint32_t
	{
		UINT16 = 0,
		UINT32,
		MAX
	};

	// 렌더링에서 사용되는 기본 도형 유형을 나타내는 열거형
	enum class PRIMITIVE_TYPE : uint32_t
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

	// 셰이더 타입 열거형
	enum class SHADER_TYPE : uint32_t
	{
		UNKNOWN = 0,
		VERTEX,
		PIXEL,
		GEOMETRY,
		COMPUTE,

		MAX
	};

	// 버퍼 사용 용도를 나타내는 열거형
	enum class BUFFER_USAGE : uint32_t
	{
		DEFAULT = 0,
		IMMUTABLE,
		DYNAMIC,
		STAGING,

		EMAX
	};

	// 렌더 타겟과 깊이-스텐실 버퍼를 지울 때 어떤 버퍼를 지울지 지정하는 플래그 열거형
	enum class CLEAR_BUFFER : uint32_t
	{
		COLOR = 0x00000001u,
		DEPTH = 0x00000002u,
		STENCIL = 0x00000004u,
		ALL = COLOR | DEPTH | STENCIL
	};

	// 버텍스 포맷 열거형
	enum class VERTEX_FORMAT : uint32_t
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
	enum class VERTEX_FORMAT_INDEX : uint32_t
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

	static constexpr uint32_t MAX_VERTEX_FORMAT = 16; // 최대 버텍스 포맷 수
	static constexpr int32_t MAX_RENDER_PASS = 4; // 서피스 메테리얼의 최대 렌더 패스 수 (추가 패스는 별도의 메테리얼로 구현)
	static constexpr int32_t MAX_BLEND_TARGET = 8; // 최대 블렌드 타겟 수 (MRT 지원)
	static constexpr uint32_t MAX_PRIMITIVE = 32; // 최대 프리미티브 수

	static constexpr int32_t MAX_SAMPLER_SLOT = 16; // 최대 샘플러 슬롯 수
	static constexpr int32_t MAX_CONSTANT_SLOT = 14; // 최대 상수 버퍼 슬롯 수
	static constexpr int32_t MAX_ANISOTROPY_LEVEL = 16; // 최대 이방성 필터링 레벨

	/**
	* @brief 텍스쳐 타입
	*/
	enum class TEXTURE_TYPE : uint32_t
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
	enum class SAMPLER_FILTER : uint8_t
	{
		NONE = 0,
		NEAREST,
		LINEAR,

		EMAX
	};

	/**
	* @brief 텍스처 주소 모드 열거형
	*/
	enum class TEXTURE_ADDRESS_MODE : uint8_t
	{
		WRAP = 0,
		MIRROR,
		CLAMP,
		BORDER,
		MIRROR_ONCE,

		EMAX
	};

	enum class BORDER_COLOR : uint8_t
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
	enum class BLEND_FACTOR : uint8_t
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
	enum class BLEND_OP : uint8_t
	{
		ADD = 0,
		SUBTRACT,
		REV_SUBTRACT,
		MIN,
		MAX,

		EMAX
	};

	// 컬러 쓰기 마스크 (비트 플래그)
	enum COLOR_WRITE_MASK : uint8_t
	{
		DISABLE = 0x00,
		RED = 0x01,
		GREEN = 0x02,
		BLUE = 0x04,
		ALPHA = 0x08,
		ALL = RED | GREEN | BLUE | ALPHA,
	};

	constexpr uint8_t MAX_COLOR_WRITE_MASK = 6;

	/**
* @brief 채우기 모드 열거형
*/
	enum class FILL_MODE : uint8_t
	{
		SOLID = 0,
		WIREFRAME,

		EMAX
	};

	/**
	* @brief 컬링 모드 열거형
	*/
	enum class CULL_MODE : uint8_t
	{
		NONE = 0,
		CW,
		CCW,

		EMAX
	};

	// 보수적 래스터화 모드 (D3D12, Vulkan 확장)
	enum class CONSERVATIVE_RASTER_MODE : uint8_t
	{
		OFF = 0,
		ON = 1,
	};

	/**
	* @brief 깊이 쓰기 마스크 열거형
	*/
	enum class DEPTH_WRITE_MASK : uint8_t
	{
		ZERO = 0,
		ALL,

		EMAX
	};

	/**
	* @brief 깊이 비교 함수 열거형
	*/
	enum class COMPARISON_FUNC : uint8_t
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
	enum class STENCIL_OP : uint8_t
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
	enum class RESOURCE_USAGE : uint32_t
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
	enum class CAMERA_MODE : uint32_t
	{
		PERSPECTIVE = 0,
		ORTHO,
	};

	/**
	* @brief 렌더 명령어 타입 열거형
	*/
	enum class COMMAND_TYPE : uint32_t
	{
		NONE = 0,
		CLEAR_RENDER_TARGET = 0x00000001,
		SET_RENDER_TARGET = 0x00000002,
		DRAW_MESH = 0x00000004,
	};

	enum class RENDER_QUEUE_TYPE : uint32_t
	{
		RQ_OPAQUE = 0,
		RQ_TRANSPARENT,
		EMAX
	};

	enum class TEXTURE_FILE_FORMAT : uint32_t
	{
		UNKNOWN = 0,
		DDS,
		PNG,
		JPEG,
		BMP,
		TGA,
	};

	enum class RESOURCE_TYPE_INDEX : uint16_t
	{
		PRIMITIVE_BUFFER = 0,
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_CUBE,
	};

	struct D3D11_RENDER_HANDLE
	{
		uint64_t TypeIndex : 8;
		uint64_t PageIndex : 8;
		uint64_t Index : 22;
		uint64_t Version : 8;
		uint64_t RefCnt : 18; // 참조 카운트 (최대 262143)
	};

	/**
	* @brief 3D 엔진 생성 정보 구조체
	*/
	struct RENDERER_CREATE_DESC
	{
#if defined(__TARGET_OS_WINDOWS)
		HWND hWnd; // 윈도우 핸들
#endif // __TARGET_OS_WINDOWS

		RENDER_API RenderAPI; // 렌더링 API
		uint32_t ScreenWidth; // 화면 너비
		uint32_t ScreenHeight; // 화면 높이

		bool Fullscreen; // 전체 화면 모드 여부

	};

	struct PRIMITIVEBUFFER_CREATE_DESC
	{
		uint32_t VertexFormats;
		uint32_t VertexCount; // 버텍스 수
		uint32_t IndexCount; // 인덱스 수 (0이면 인덱스 버퍼 미사용)
		BUFFER_USAGE VBUsage; // 버퍼 사용 용도
		BUFFER_USAGE IBUsage; // 버퍼 사용 용도 (인덱스 버퍼, IndexCount > 0인 경우에만 사용)
	};

	using RENDER_HANDLE = uint64_t;
}
