#pragma once
#include <cstdint>
#include "unknown.h"


namespace mark
{
	enum class GRAPHICS_API : uint8_t
	{
		D3D11,
		D3D12,
		Vulkan,
		Metal,
		GLES3
	};

	enum class BUFFER_USAGE : uint8_t
	{
		DEFAULT,
		DYNAMIC,

		EMAX
	};

	enum class BUFFER_TYPE : uint8_t
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER,
		
		EMAX
	};

	enum class BUFFER_ACCESS : uint8_t
	{
		NONE = 0x0,
		READ = 0x01,
		WRITE = 0x02,
		READ_WRITE = READ | WRITE
	};

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

	/**
	* @brief 버텍스 포맷 열거형
	*/
	enum class VERTEX_FORMAT : unsigned int
	{
		NONE = 0x00000000,
		POSITION = 0x00000001,		// FLOAT3
		NORMAL = 0x00000002,		// FLOAT3
		COLOR = 0x00000004,			// FLOAT4

		TANGENT = 0x00000008,		// FLOAT3

		BONE = 0x00000010,			// UINT4
		WEIGHT = 0x00000020,		// FLOAT4

		TEXCOORD0 = 0x00000100,		// FLOAT2
		TEXCOORD1 = 0x00000200,		// FLOAT2
		CUSTOM0 = 0x00000400,		// FLOAT4
		CUSTOM1 = 0x00000800,		// FLOAT4
	};

	/**
	* @brief 버텍스 포맷 인덱스 열거형
	*/
	enum class VERTEX_FORMAT_INDEX : unsigned int
	{
		POSITION = 0,
		NORMAL,
		COLOR,

		TANGENT,

		BONE,
		WEIGHT,

		TEXCOORD0,
		TEXCOORD1,
		CUSTOM0,
		CUSTOM1,

		EMAX
	};

	inline VERTEX_FORMAT_INDEX ToVertexFormatIndex(VERTEX_FORMAT Format)
	{
		switch (Format)
		{
			case VERTEX_FORMAT::POSITION:
				return VERTEX_FORMAT_INDEX::POSITION;
			case VERTEX_FORMAT::NORMAL:
				return VERTEX_FORMAT_INDEX::NORMAL;
			case VERTEX_FORMAT::COLOR:
				return VERTEX_FORMAT_INDEX::COLOR;
			case VERTEX_FORMAT::TANGENT:
				return VERTEX_FORMAT_INDEX::TANGENT;
			case VERTEX_FORMAT::BONE:
				return VERTEX_FORMAT_INDEX::BONE;
			case VERTEX_FORMAT::WEIGHT:
				return VERTEX_FORMAT_INDEX::WEIGHT;
			case VERTEX_FORMAT::TEXCOORD0:
				return VERTEX_FORMAT_INDEX::TEXCOORD0;
			case VERTEX_FORMAT::TEXCOORD1:
				return VERTEX_FORMAT_INDEX::TEXCOORD1;
			case VERTEX_FORMAT::CUSTOM0:
				return VERTEX_FORMAT_INDEX::CUSTOM0;
			case VERTEX_FORMAT::CUSTOM1:
				return VERTEX_FORMAT_INDEX::CUSTOM1;
		}

		return VERTEX_FORMAT_INDEX::EMAX; // 유효하지 않은 포맷
	}

	inline VERTEX_FORMAT ToVertexFormat(VERTEX_FORMAT_INDEX Index)
	{
		switch (Index)
		{
			case  VERTEX_FORMAT_INDEX::POSITION:
				return VERTEX_FORMAT::POSITION;
			case VERTEX_FORMAT_INDEX::NORMAL:
				return VERTEX_FORMAT::NORMAL;
			case VERTEX_FORMAT_INDEX::COLOR:
				return VERTEX_FORMAT::COLOR;
			case VERTEX_FORMAT_INDEX::TANGENT:
				return VERTEX_FORMAT::TANGENT;
			case VERTEX_FORMAT_INDEX::BONE:
				return VERTEX_FORMAT::BONE;
			case VERTEX_FORMAT_INDEX::WEIGHT:
				return VERTEX_FORMAT::WEIGHT;
			case VERTEX_FORMAT_INDEX::TEXCOORD0:
				return VERTEX_FORMAT::TEXCOORD0;
			case VERTEX_FORMAT_INDEX::TEXCOORD1:
				return VERTEX_FORMAT::TEXCOORD1;
			case VERTEX_FORMAT_INDEX::CUSTOM0:
				return VERTEX_FORMAT::CUSTOM0;
			case VERTEX_FORMAT_INDEX::CUSTOM1:
				return VERTEX_FORMAT::CUSTOM1;
		}

		return VERTEX_FORMAT::NONE; // 유효하지 않은 인덱스
	}

	/**
	* @brief 버텍스 포맷 스트라이드 열거형
	*/
	enum class VERTEX_FORMAT_STRIDE : uint32_t
	{
		POSITION = sizeof(FLOAT) * 3,
		NORMAL = sizeof(FLOAT) * 3,
		
		COLOR = sizeof(FLOAT) * 4,

		TANGENT = sizeof(FLOAT) * 3,

		BONE = sizeof(uint32_t) * 4,
		WEIGHT = sizeof(FLOAT) * 4,

		TEXCOORD0 = sizeof(FLOAT) * 2,
		TEXCOORD1 = sizeof(FLOAT) * 2,
		CUSTOM0 = sizeof(FLOAT) * 4,
		CUSTOM1 = sizeof(FLOAT) * 4,
	};

	enum class INDEX_FORMAT : uint32_t
	{
		UINT16 = 0,
		UINT32,
		AUTO,
		EMAX
	};

	enum class SHADER_TYPE : uint32_t
	{
		VERTEX_SHADER,
		PIXEL_SHADER,
		COMPUTE_SHADER,
		EMAX
	};

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


	//---------------------------------------------------------
	// GPUBUffers
	struct GPUBufferCreateDesc
	{
		BUFFER_TYPE Type;
		BUFFER_USAGE Usage;
		BUFFER_ACCESS Access; // Staging 버퍼의 경우 CPU 접근 권한
		size_t BufferSize;
	};

	struct IGPUBuffer : public Unknown
	{
		[[nodiscard]] virtual BUFFER_TYPE GetBufferType() const noexcept = 0;
		[[nodiscard]] virtual BUFFER_USAGE GetBufferUsage() const noexcept = 0;
		[[nodiscard]] virtual size_t GetBufferSize() const noexcept = 0;
		[[nodiscard]] virtual void* GetNativePointer() const noexcept = 0;

		virtual bool UpdateBuffer(const void* pData, size_t DataSize, size_t* pWrittenOffset = nullptr) = 0;
		
	};

	//---------------------------------------------------------
	// Shader Programs
	constexpr uint32_t MAX_SHADER_DEFINE = 16;
	constexpr uint32_t MAX_SHADER_DEFINE_LENGTH = 64;

	struct ShaderProgramCreateDesc
	{
		char szShaderName[64];
		SHADER_TYPE ShaderType;
		uint32_t BytecodeSize;
		const void* pShaderBytecode;

		char szEntryPoint[64];
		char szShaderModel[32];
		char szShaderDefines[MAX_SHADER_DEFINE][MAX_SHADER_DEFINE_LENGTH];
		uint32_t NumDefines;
		BOOL DebugMode;
	};

	struct IShaderProgram : public Unknown
	{
		// 셰이더 프로그램 관련 인터페이스 메서드 선언
		virtual SHADER_TYPE GetShaderType() const = 0;
		virtual NameHash GetShaderNameHash() const = 0;

	};


	//---------------------------------------------------------
	// Render State
	struct RS_RASTERIZER_STATE
	{
		FILL_MODE FillMode;
		CULL_MODE CullMode;
		CONSERVATIVE_RASTER_MODE ConservativeRaster;
		uint8_t Flags;

		// Depth Bias 설정
		int32_t DepthBias;
		float DepthBiasClamp;
		float SlopeScaledDepthBias;
	};

	struct RS_SAMPLER_STATE
	{
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
		uint8_t PADDINGs[3];

		float MipLODBias;	// 기본값: 0.0f
		float MinLOD;       // 기본값: 0.0f
		float MaxLOD;       // 기본값: FLT_MAX
	};

	struct RS_BLEND_TARGET
	{
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
				uint8_t RenderTargetWriteMask; // 쓰기 마스크
			};

			uint64_t data; // 전체 데이터를 하나의 64비트 정수로 접근
		};
	};

	/**
	* @brief 블렌드 상태 구조체
	*/
	struct RS_BLEND_STATE
	{
		static constexpr int32_t MAX_BLEND_TARGET = 8; // 최대 블렌드 타겟 수 (MRT 지원)
		
		bool AlphaToCoverageEnable; // 알파 투 커버리지 활성화 여부 
		bool IndependentBlendEnable; // 독립적 블렌드 활성화 여부 (MRT용..)
		int8_t NumBlendTargets; // 블렌드 타겟 수
		RS_BLEND_TARGET BlendTarget[MAX_BLEND_TARGET]; // 블렌드 타겟 배열 (최대 8개:D3D11 기준)
	};

	struct RS_STENCIL_OP
	{
		STENCIL_OP StencilFailOp; // 스텐실 테스트 실패 시
		STENCIL_OP StencilDepthFailOp; // 스텐실 통과, 깊이 실패 시
		STENCIL_OP StencilPassOp; // 둘 다 통과 시
		STENCIL_FUNC StencilFunc; // 스텐실 비교 함수
	};

	struct RS_DEPTH_STENCIL_STATE
	{
		BOOL8 DepthEnable;
		BOOL8 DepthWriteEnable;
		COMPARISON_FUNC DepthFunc;
		DEPTH_WRITE_MASK DepthWriteMask;

		// 스텐실 설정
		BOOL8 StencilEnable;
		uint8_t StencilReadMask;
		uint8_t StencilWriteMask;
		uint8_t PADDING; // 패딩 (정렬을 위해)

		// 앞면/뒷면 스텐실 연산
		RS_STENCIL_OP FrontFace;
		RS_STENCIL_OP BackFace;
	};

	struct RENDER_STATE
	{
		RS_RASTERIZER_STATE RasterizerState;
		RS_BLEND_STATE BlendState;
		RS_DEPTH_STENCIL_STATE DepthStencilState;
		FLOAT4 BlendFactor; // 블렌드 팩터 (상수 블렌드에 사용)
		UINT32 SampleMask; // 샘플 마스크 (멀티샘플링에 사용)
		UINT32 StencilRef; // 스텐실 참조값
	};

	struct IHardwareGraphicsLayer : public Unknown
	{
		// 하드웨어 그래픽스 레이어 관련 인터페이스 메서드 선언
		[[nodiscard]] virtual IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& desc) = 0;
		[[nodiscard]] virtual IShaderProgram* CreateShaderProgram(const ShaderProgramCreateDesc& desc) = 0;
	};

	//---------------------------------------------------------
	// Render System
	struct RenderSystemCreateDesc
	{
		GRAPHICS_API PreferredGraphicsAPI;
		uint32_t ScreenWidth;
		uint32_t ScreenHeight;
#if defined(__TARGET_OS_WINDOWS)
		HWND WindowHandle;
#endif // #if defined(__TARGET_OS_WINDOWS)

		BOOL DebugMode;
	};
	
	struct IRenderSystem : public Unknown
	{
		[[nodiscard]] virtual IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& CreateDesc) = 0;
		[[nodiscard]] virtual IShaderProgram* CreateShaderProgram(const ShaderProgramCreateDesc& CreateDesc) = 0;
		[[nodiscard]] virtual IShaderProgram* GetShaderProgram(SHADER_TYPE ShaderType, const char* szShaderName) = 0;
	};
}
