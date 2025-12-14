#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "RenderState.h"


/**
* @brief 렌더 패스 구조체
*/
struct RENDER_PASS
{
	int32 Pass; // 패스 인덱스
	NameHash Name; // 패스 이름
	IShaderProgram* pVertexShader; // 버텍스 셰이더
	IShaderProgram* pPixelShader; // 픽셀 셰이더

	size_t NumTextureState; // 텍스처 상태 수
	TEXTURE_STATE TextureState[MAX_TEXTURE_SLOT]; // 1D 텍스처 상태
	RS_SAMPLER_STATE SamplerState[MAX_SAMPLER_SLOT]; // 샘플러 상태
	RS_BLEND_STATE BlendState; // 블렌드 상태
	FLOAT4 BlendFactor; // 블렌드 팩터
	RS_DEPTH_STENCIL_STATE DepthStencilState; // 깊이-스텐실 상태
	RS_RASTERIZER_STATE RasterizerState; // 래스터라이저 상태

	constexpr RENDER_PASS()
		: Pass(-1)
		, pVertexShader(nullptr)
		, pPixelShader(nullptr)
		, NumTextureState(0)
		, BlendState()
		, DepthStencilState()
		, RasterizerState()
	{
	}

	~RENDER_PASS() noexcept;
};


#endif // __RENDER_PASS_H__
