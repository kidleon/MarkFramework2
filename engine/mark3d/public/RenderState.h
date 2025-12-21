#ifndef __RENDER_STATE_H__
#define __RENDER_STATE_H__

#include "RenderDef.h"


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

	COMPARISON_FUNC ComparisonFunc; // NEVER = 비활성화

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
			uint8_t RenderTargetWriteMask; // 쓰기 마스크
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
	INT32 NumBlendTargets; // 블렌드 타겟 수
	BOOL AlphaToCoverageEnable; // 알파 투 커버리지 활성화 여부 
	BOOL IndependentBlendEnable; // 독립적 블렌드 활성화 여부 (MRT용..)
	UINT32 PADDING;
	RS_BLEND_TARGET BlendTarget[MAX_BLEND_TARGET]; // 블렌드 타겟 배열 (최대 8개:D3D11 기준)
	FLOAT4 BlendFactor[MAX_BLEND_TARGET]; // 블렌드 팩터 배열
	constexpr RS_BLEND_STATE()
		: NumBlendTargets(1)
		, AlphaToCoverageEnable(FALSE)
		, IndependentBlendEnable(FALSE)
		, PADDING(0)
		, BlendTarget{}
		, BlendFactor{}
	{
	}
};

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
	DEPTH_FUNC DepthFunc;

	// 스텐실 설정
	BOOL8 StencilEnable;
	uint8_t StencilReadMask;
	uint8_t StencilWriteMask;

	uint16_t PADDING;

	// 앞면/뒷면 스텐실 연산
	RS_STENCIL_OP FrontFace;
	RS_STENCIL_OP BackFace;

	constexpr RS_DEPTH_STENCIL_STATE()
		: DepthEnable(TRUE)
		, DepthWriteEnable(TRUE)
		, DepthFunc(DEPTH_FUNC::LESS)
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
		return !DepthEnable || DepthFunc == DEPTH_FUNC::ALWAYS;
	}

	// 깊이 쓰기 전용 모드
	__FORCEINLINE BOOL IsDepthWriteOnly() const
	{
		return DepthWriteEnable && (!DepthEnable || DepthFunc == DEPTH_FUNC::ALWAYS);
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

/**
* @brief 래스터라이저 상태 구조체
*/
struct MARKENGINE_API RS_RASTERIZER_STATE
{
	static RS_RASTERIZER_STATE DEFAULT; // 기본 래스터라이저 상태
	static RS_RASTERIZER_STATE WIREFRAME; // 와이어프레임 모드
	static RS_RASTERIZER_STATE WIREFRAME_TWOSIDE; // 와이어프레임 + 양면 렌더링
	static RS_RASTERIZER_STATE TWO_SIDE; // 양면 렌더링

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
		, CullMode(CULL_MODE::CCW)
		, ConservativeRaster(CONSERVATIVE_RASTER_MODE::OFF)
		, Flags(FLAG_DEFAULT)
		, DepthBias(0)
		, DepthBiasClamp(0.0f)
		, SlopeScaledDepthBias(0.0f)
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

/**
* @brief 텍스처 상태 구조체
*/
struct MARKENGINE_API TEXTURE_STATE
{
	NameHash Name; // 텍스처 이름 해시
	INT32 BindIndex; // 바인드 인덱스
	
	TEXTURE_TYPE TextureType; // 텍스처 타입
	UINT32 SamplerIndex; // 샘플러 인덱스

	union
	{
		ITexture1D* pTexture1D; // 1D 텍스처 포인터
		ITexture2D* pTexture2D; // 2D 텍스처 포인터
	};

};


interface IConstantBuffer;

struct MARKENGINE_API CONSTANT_STATE
{
	NameHash Name; // 상수 버퍼 이름 해시
	INT32 BindIndex; // 바인드 인덱스
	IConstantBuffer* pCB; // 상수 버퍼 포인터

	constexpr CONSTANT_STATE()
		: Name()
		, BindIndex(-1)
		, pCB(nullptr)
	{
	}
};


#endif // __RENDER_STATE_H__
