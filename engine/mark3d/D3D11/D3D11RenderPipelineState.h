#ifndef __D3D11_RENDER_PIPELINE_STATE_H__
#define __D3D11_RENDER_PIPELINE_STATE_H__


class D3D11Shader;
struct D3D11BlendState;
struct D3D11RasterizerState;
struct D3D11DepthStencilState;
struct D3D11SamplerState;

struct D3D11RenderPipelineState
{
	union
	{
		uint64 Hash;
		struct
		{
			uint64 VertexShaderIndex : 16;
			uint64 PixelShaderIndex : 16;
			uint64 BlendStateIndex : 8;
			uint64 RasterizerStateIndex : 8;
			uint64 DepthStencilStateIndex : 8;
			uint64 PADDING : 8;
		};
	};

	D3D11Shader* pVertexShader;
	D3D11Shader* pPixelShader;

	D3D11BlendState* pBlendState;
	D3D11RasterizerState* pRasterizerState;
	D3D11DepthStencilState* pDepthStencilState;

	__FORCEINLINE bool operator==(const D3D11RenderPipelineState& Other) const noexcept
	{
		return Hash == Other.Hash;
	}

	__FORCEINLINE bool operator!=(const D3D11RenderPipelineState& Other) const noexcept
	{
		return Hash != Other.Hash;
	}

	__FORCEINLINE bool operator<(const D3D11RenderPipelineState& Other) const noexcept
	{
		return Hash < Other.Hash;
	}

	__FORCEINLINE bool operator>(const D3D11RenderPipelineState& Other) const noexcept
	{
		return Hash > Other.Hash;
	}
};

struct D3D11DynamicRenderPipelineState
{
	// 뷰포트
	// 시저
	// 텍스쳐 바인딩
	// 샘플러 바인딩
	// CB 바인딩

	// 버텍스 버퍼
	// 인덱스 버퍼

	// 스텐실 REF

	// 블렌드 팩터
};


#endif // __D3D11_RENDER_PIPELINE_STATE_H__