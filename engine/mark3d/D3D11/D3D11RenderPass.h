#ifndef __D3D11_RENDER_PASS_H__
#define __D3D11_RENDER_PASS_H__

#include "RenderState.h"


class D3D11ShaderProgram;

/**
* @brief 렌더 패스 구조체
*/
struct D3D11_RENDER_PASS
{
	int32 Pass; // 패스 인덱스
	NameHash Name; // 패스 이름
	D3D11ShaderProgram* pVertexShader; // 버텍스 셰이더
	D3D11ShaderProgram* pPixelShader; // 픽셀 셰이더

	CONSTANT_STATE ConstantState[MAX_CONSTANT_SLOT]; // 상수 버퍼 상태

	size_t NumTextureState; // 텍스처 상태 수
	TEXTURE_STATE TextureState[MAX_TEXTURE_SLOT]; // 텍스처 상태
	RS_SAMPLER_STATE SamplerState[MAX_SAMPLER_SLOT]; // 샘플러 상태
	RS_BLEND_STATE BlendState; // 블렌드 상태
	RS_DEPTH_STENCIL_STATE DepthStencilState; // 깊이-스텐실 상태
	RS_RASTERIZER_STATE RasterizerState; // 래스터라이저 상태
};


#endif // __D3D11_RENDER_PASS_H__
