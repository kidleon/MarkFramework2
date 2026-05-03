#pragma once
#include <cstdint>
#include "unknown.h"


namespace mark
{
	enum class GraphicsAPI : uint8_t
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

		MAX
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

		return VERTEX_FORMAT_INDEX::MAX; // 유효하지 않은 포맷
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
	enum class VERTEX_FORMAT_STRIDE : unsigned int
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

	enum class INDEX_FORMAT : UINT32
	{
		UINT16 = 0,
		UINT32,
		MAX
	};

	struct RenderSystemCreateDesc
	{
		uint32_t ScreenWidth;
		uint32_t ScreenHeight;

#if defined(__TARGET_OS_WINDOWS)
		HWND WindowHandle;
#endif // #if defined(__TARGET_OS_WINDOWS)
	};

	struct GPUBufferCreateDesc
	{
		BUFFER_TYPE Type;
		BUFFER_USAGE Usage;
		BUFFER_ACCESS Access; // Staging 버퍼의 경우 CPU 접근 권한
		size_t BufferSize;
	};

	struct IGPUBuffer : public Unknown
	{
		[[nodiscard]] virtual BUFFER_TYPE GetBufferType() const = 0;
		[[nodiscard]] virtual size_t GetBufferSize() const = 0;
		[[nodiscard]] virtual void* GetNativePointer() const = 0;

		virtual bool UpdateBuffer(const void* pData, size_t DataSize) = 0;
		virtual void* Lock() = 0;
		virtual void Unlock() = 0;
	};

	struct IShaderProgram : public Unknown
	{
		// 셰이더 프로그램 관련 인터페이스 메서드 선언
	};


	struct IRenderSystem : public Unknown
	{
		virtual bool Initialize(const RenderSystemCreateDesc& desc) = 0;
		virtual void Shutdown() = 0;

		[[nodiscard]] virtual IGPUBuffer* CreateGPUBuffer(const GPUBufferCreateDesc& desc) = 0;

	};

}
