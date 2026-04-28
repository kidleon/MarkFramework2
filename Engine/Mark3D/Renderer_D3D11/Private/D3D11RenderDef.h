#pragma once


namespace mark
{
	// TEXTURE_ADDRESS_MODE와 D3D11_TEXTURE_ADDRESS_MODE 매핑 테이블
	static constexpr D3D11_TEXTURE_ADDRESS_MODE D3D11_IMPL_TEXTURE_ADDRESS_MODE[(int)TEXTURE_ADDRESS_MODE::EMAX] =
	{
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
	};

	// DEPTH_FUNC와 D3D11_COMPARISON_FUNC 매핑 테이블
	static constexpr D3D11_COMPARISON_FUNC D3D11_IMPL_COMPARISON_FUNC[(int)COMPARISON_FUNC::EMAX] =
	{
		D3D11_COMPARISON_NEVER,
		D3D11_COMPARISON_LESS,
		D3D11_COMPARISON_EQUAL,
		D3D11_COMPARISON_LESS_EQUAL,
		D3D11_COMPARISON_GREATER,
		D3D11_COMPARISON_NOT_EQUAL,
		D3D11_COMPARISON_GREATER_EQUAL,
		D3D11_COMPARISON_ALWAYS
	};

	// BLEND_FACTOR와 D3D11_BLEND 매핑 테이블
	static constexpr D3D11_BLEND D3D11_IMPL_BLEND_FACTOR[(int)BLEND_FACTOR::EMAX] =
	{
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,
		D3D11_BLEND_SRC_COLOR,
		D3D11_BLEND_INV_SRC_COLOR,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_DEST_ALPHA,
		D3D11_BLEND_INV_DEST_ALPHA,
		D3D11_BLEND_DEST_COLOR,
		D3D11_BLEND_INV_DEST_COLOR,
		D3D11_BLEND_SRC_ALPHA_SAT,
		D3D11_BLEND_BLEND_FACTOR,
		D3D11_BLEND_INV_BLEND_FACTOR,
		D3D11_BLEND_SRC1_COLOR,
		D3D11_BLEND_INV_SRC1_COLOR,
		D3D11_BLEND_SRC1_ALPHA,
		D3D11_BLEND_INV_SRC1_ALPHA
	};

	// BLEND_OP와 D3D11_BLEND_OP 매핑 테이블
	static constexpr D3D11_BLEND_OP D3D11_IMPL_BLEND_OP[(int)BLEND_OP::EMAX] =
	{
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_SUBTRACT,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_MIN,
		D3D11_BLEND_OP_MAX
	};

	// COLOR_WRITE_MASK와 D3D11_COLOR_WRITE_ENABLE 매핑 테이블
	static constexpr uint8_t D3D11_IMPL_COLOR_WRITE_MASK[MAX_COLOR_WRITE_MASK] =
	{
		D3D11_COLOR_WRITE_ENABLE_RED,
		D3D11_COLOR_WRITE_ENABLE_GREEN,
		D3D11_COLOR_WRITE_ENABLE_BLUE,
		D3D11_COLOR_WRITE_ENABLE_ALPHA,
		D3D11_COLOR_WRITE_ENABLE_ALL
	};

	// FILL_MODE와 D3D11_FILL_MODE 매핑 테이블
	static constexpr D3D11_FILL_MODE D3D11_IMPL_FILL_MODE[(int)FILL_MODE::EMAX] =
	{
		D3D11_FILL_SOLID,
		D3D11_FILL_WIREFRAME
	};

	// CULL_MODE와 D3D11_CULL_MODE 매핑 테이블
	static constexpr D3D11_CULL_MODE D3D11_IMPL_CULL_MODE[(int)CULL_MODE::EMAX] =
	{
		D3D11_CULL_NONE,
		D3D11_CULL_FRONT,
		D3D11_CULL_BACK
	};

	static constexpr D3D11_DEPTH_WRITE_MASK D3D11_IMPL_DEPTH_WRITE_MASK[(int)DEPTH_WRITE_MASK::EMAX] =
	{
		D3D11_DEPTH_WRITE_MASK_ZERO,
		D3D11_DEPTH_WRITE_MASK_ALL
	};

	// STENCIL_OP와 D3D11_STENCIL_OP 매핑 테이블
	static constexpr D3D11_STENCIL_OP D3D11_IMPL_STENCIL_OP[(int)STENCIL_OP::EMAX] =
	{
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_ZERO,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_INCR_SAT,
		D3D11_STENCIL_OP_DECR_SAT,
		D3D11_STENCIL_OP_INVERT,
		D3D11_STENCIL_OP_INCR,
		D3D11_STENCIL_OP_DECR
	};

	static constexpr D3D11_USAGE D3D11_IMPL_BUFFER_USAGE[(int)BUFFER_USAGE::EMAX] =
	{
		D3D11_USAGE_DEFAULT,
		D3D11_USAGE_IMMUTABLE,
		D3D11_USAGE_DYNAMIC,
		D3D11_USAGE_STAGING
	};

	static constexpr D3D11_PRIMITIVE_TOPOLOGY D3D11_IMPL_PRIMITIVE_TOPOLOGY[(int)PRIMITIVE_TYPE::MAX] =
	{
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED // TRIANGLE_FAN는 D3D11에서 지원하지 않음
	};

	enum class D3D11_BUFFER_TYPE : uint8_t
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER
	};

	enum class D3D11_BUFFER_STRATEGY : uint8_t
	{
		// 독립, 버퍼풀, 트랜지언트 버퍼
		INDEPENDENT,
		POOL,
		TRANSIENT
	};

#define MAKE_BUFFER_POOL_ID(Format, Index) (((uint32_t)(Format) << 24) | ((uint32_t)(Index) & 0x00FFFFFF))
#define GET_BUFFER_POOL_FORMAT(PoolID) ((D3D11_BUFFER_TYPE)(((PoolID) >> 24) & 0xFF))
#define GET_BUFFER_POOL_INDEX(PoolID) ((uint32_t)((PoolID) & 0x00FFFFFF))

	inline D3D11_FILTER __D3D11ConvSamplerFilter(
		SAMPLER_FILTER MinFilter,
		SAMPLER_FILTER MagFilter,
		SAMPLER_FILTER MipFilter,
		uint8_t MaxAnisotropy
	)
	{
		// Anisotropic 필터링이 활성화된 경우
		if (MaxAnisotropy > 1)
		{
			return D3D11_FILTER_ANISOTROPIC;
		}

		// NONE 처리 - 기본값을 LINEAR로 설정
		if (MinFilter == SAMPLER_FILTER::NONE)
			MinFilter = SAMPLER_FILTER::LINEAR;

		if (MagFilter == SAMPLER_FILTER::NONE)
			MagFilter = SAMPLER_FILTER::LINEAR;

		if (MipFilter == SAMPLER_FILTER::NONE)
			MipFilter = SAMPLER_FILTER::LINEAR;

		// 모든 조합을 명시적으로 처리
		// NEAREST = POINT, LINEAR = LINEAR

		// Min=NEAREST, Mag=NEAREST, Mip=NEAREST
		if (MinFilter == SAMPLER_FILTER::NEAREST &&
			MagFilter == SAMPLER_FILTER::NEAREST &&
			MipFilter == SAMPLER_FILTER::NEAREST)
		{
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}

		// Min=NEAREST, Mag=NEAREST, Mip=LINEAR
		if (MinFilter == SAMPLER_FILTER::NEAREST &&
			MagFilter == SAMPLER_FILTER::NEAREST &&
			MipFilter == SAMPLER_FILTER::LINEAR)
		{
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		}

		// Min=NEAREST, Mag=LINEAR, Mip=NEAREST
		if (MinFilter == SAMPLER_FILTER::NEAREST &&
			MagFilter == SAMPLER_FILTER::LINEAR &&
			MipFilter == SAMPLER_FILTER::NEAREST)
		{
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}

		// Min=NEAREST, Mag=LINEAR, Mip=LINEAR
		if (MinFilter == SAMPLER_FILTER::NEAREST &&
			MagFilter == SAMPLER_FILTER::LINEAR &&
			MipFilter == SAMPLER_FILTER::LINEAR)
		{
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		}

		// Min=LINEAR, Mag=NEAREST, Mip=NEAREST
		if (MinFilter == SAMPLER_FILTER::LINEAR &&
			MagFilter == SAMPLER_FILTER::NEAREST &&
			MipFilter == SAMPLER_FILTER::NEAREST)
		{
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}

		// Min=LINEAR, Mag=NEAREST, Mip=LINEAR
		if (MinFilter == SAMPLER_FILTER::LINEAR &&
			MagFilter == SAMPLER_FILTER::NEAREST &&
			MipFilter == SAMPLER_FILTER::LINEAR)
		{
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		}

		// Min=LINEAR, Mag=LINEAR, Mip=NEAREST
		if (MinFilter == SAMPLER_FILTER::LINEAR &&
			MagFilter == SAMPLER_FILTER::LINEAR &&
			MipFilter == SAMPLER_FILTER::NEAREST)
		{
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}

		// Min=LINEAR, Mag=LINEAR, Mip=LINEAR
		if (MinFilter == SAMPLER_FILTER::LINEAR &&
			MagFilter == SAMPLER_FILTER::LINEAR &&
			MipFilter == SAMPLER_FILTER::LINEAR)
		{
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}

		// 기본값 (도달하면 안 됨)
		assert(false && "Invalid filter combination");
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}

	// Border Color를 FLOAT[4] 배열로 변환
	inline void __D3D11ConvBorderColor(BORDER_COLOR BorderColor, const FLOAT4* pCustomBorderColor, FLOAT* pOut)
	{
		assert(pOut != nullptr && "pOut cannot be null");

		if (BorderColor == BORDER_COLOR::TRANSPARENT_BLACK)
		{
			pOut[0] = 0.0f;  // R
			pOut[1] = 0.0f;  // G
			pOut[2] = 0.0f;  // B
			pOut[3] = 0.0f;  // A
		}
		else if (BorderColor == BORDER_COLOR::OPAQUE_BLACK)
		{
			pOut[0] = 0.0f;  // R
			pOut[1] = 0.0f;  // G
			pOut[2] = 0.0f;  // B
			pOut[3] = 1.0f;  // A
		}
		else if (BorderColor == BORDER_COLOR::OPAQUE_WHITE)
		{
			pOut[0] = 1.0f;  // R
			pOut[1] = 1.0f;  // G
			pOut[2] = 1.0f;  // B
			pOut[3] = 1.0f;  // A
		}
		else if (BorderColor == BORDER_COLOR::CUSTOM)
		{
			// CUSTOM의 경우 호출자가 직접 설정해야 함
			// 기본값으로 Transparent Black 설정
			if (pCustomBorderColor)
			{
				pOut[0] = pCustomBorderColor->x;
				pOut[1] = pCustomBorderColor->y;
				pOut[2] = pCustomBorderColor->z;
				pOut[3] = pCustomBorderColor->w;
			}
			else
			{
				pOut[0] = 0.0f;
				pOut[1] = 0.0f;
				pOut[2] = 0.0f;
				pOut[3] = 0.0f;
			}
		}
		else
		{
			pOut[0] = 0.0f;
			pOut[1] = 0.0f;
			pOut[2] = 0.0f;
			pOut[3] = 0.0f;
		}
	}

	struct D3D11_RENDER_HANDLE
	{
		union
		{
			struct
			{
				uint64_t TypeIndex : 8;		// [0:7]   리소스 타입 (최대 255)
				uint64_t PageIndex : 14;	// [8:21]  페이지 인덱스 (최대 16383)
				uint64_t Index : 14;		// [22:35] 리소스 인덱스 (최대 16383)
				uint64_t Version : 10;		// [36:45] 버전 (최대 1023)
				uint64_t RefCnt : 14;		// [46:59] 참조 카운트 (최대 16383)
				uint64_t Stats : 4;			// [60:63] 리소스 상태 (최대 15)
			};

			uint64_t id; // 전체 64비트를 한번에 비교/복사할 때 사용
		};
	};

	enum class D3D11_RESOURCE_TYPE : uint8_t
	{
		PRIMITIVE_BUFFER = 0,
		MAX
	};

	enum class RESOURCE_STATS : uint8_t
	{
		EMPTY = 0,
		LOADING,
		UNLOADING,
		LOADED,

		MAX
	};

}
