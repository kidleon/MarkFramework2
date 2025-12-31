#ifndef __D3D11_RENDER_PASS_H__
#define __D3D11_RENDER_PASS_H__


class D3D11ShaderProgram;
struct D3D11RenderPipelineState;
struct D3D11DynamicRenderPipelineState;


enum RENDER_STATE_CHANGE_FLAG : uint64
{
	RS_CHANGE_FLAG_NONE					= 0x0000000000000000ULL,
	RS_CHANGE_FLAG_BLEND_STATE			= 0x0000000000000001ULL,
	RS_CHANGE_FLAG_DEPTH_STENCIL_STATE	= 0x0000000000000002ULL,
	RS_CHANGE_FLAG_RASTERIZER_STATE		= 0x0000000000000004ULL,
	RS_CHANGE_FLAG_CONSTANT_BUFFER		= 0x0000000000000008ULL,
	RS_CHANGE_FLAG_TEXTURE				= 0x0000000000000010ULL,
	RS_CHANGE_FLAG_SAMPLER_STATE		= 0x0000000000000020ULL,
};

/**
* @brief 렌더 패스 구조체
*/
struct D3D11_RENDER_PASS
{
	int32 Pass; // 패스 인덱스
	NameHash Name; // 패스 이름

	// Shaders..
	D3D11ShaderProgram* pVertexShader; // 버텍스 셰이더
	D3D11ShaderProgram* pPixelShader; // 픽셀 셰이더

	// RenderPipelineState
	RS_BLEND_STATE BlendState; // 블렌드 상태
	RS_DEPTH_STENCIL_STATE DepthStencilState; // 깊이-스텐실 상태
	RS_RASTERIZER_STATE RasterizerState; // 래스터라이저 상태
	uint64 BlendStateHash; // BlendState 해시값
	uint64 DepthStencilStateHash; // DepthStencilState 해시값
	uint64 RasterizerStateHash; // RasterizerState 해시값
	BOOL DirtyBlendState;
	BOOL DirtyDepthStencilState;
	BOOL DirtyRasterizerState;

	// DynamicRenderPipelineState
	CONSTANT_STATE ConstantState[MAX_CONSTANT_SLOT]; // 상수 버퍼 상태
	size_t NumTextureState; // 텍스처 상태 수
	TEXTURE_STATE TextureState[MAX_TEXTURE_SLOT]; // 텍스처 상태
	RS_SAMPLER_STATE SamplerState[MAX_SAMPLER_SLOT]; // 샘플러 상태

	uint64 ChangedFlags; // 변경 플래그
	D3D11RenderPipelineState* m_pRenderPipelineState;
	D3D11DynamicRenderPipelineState* m_pDynamicRenderPipelineState;
};


#endif // __D3D11_RENDER_PASS_H__
