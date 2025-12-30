#ifndef __D3D11_RENDER_COMMAND_H__
#define __D3D11_RENDER_COMMAND_H__


class D3D11Shader;
struct D3D11BlendState;
struct D3D11RasterizerState;
struct D3D11DepthStencilState;
struct D3D11SamplerState;
struct D3D11RenderPipelineState;
struct D3D11DynamicRenderPipelineState;

struct MESH_RENDER_SORT_KEY
{
	union
	{
		uint64 Hash;
		struct
		{
			uint64 Pass : 4; // 16
			uint64 VertexShaderIndex : 10; // 1024
			uint64 PixelShaderIndex : 10; // 1024
			uint64 RenderStateHash : 16; // 65536
			uint64 Depth : 14; // 16384
		};
	};
};

struct BASE_RENDER_COMMAND
{
	LINK_NODE LinkNode;
};

struct MESH_RENDER_COMMAND : public BASE_RENDER_COMMAND
{
	MESH_RENDER_SORT_KEY SortKey;
	D3D11Shader* pVertexShader;
	D3D11Shader* pPixelShader;
	D3D11RenderPipelineState* pRPS;
	D3D11DynamicRenderPipelineState* pDynamicRPS;

	__FORCEINLINE bool operator==(const MESH_RENDER_COMMAND& Other) const noexcept
	{
		return SortKey.Hash == Other.SortKey.Hash;
	}

	__FORCEINLINE bool operator!=(const MESH_RENDER_COMMAND& Other) const noexcept
	{
		return SortKey.Hash != Other.SortKey.Hash;
	}

	__FORCEINLINE bool operator<(const MESH_RENDER_COMMAND& Other) const noexcept
	{
		return SortKey.Hash < Other.SortKey.Hash;
	}

	__FORCEINLINE bool operator>(const MESH_RENDER_COMMAND& Other) const noexcept
	{
		return SortKey.Hash > Other.SortKey.Hash;
	}
};


#endif // __D3D11_RENDER_COMMAND_H__