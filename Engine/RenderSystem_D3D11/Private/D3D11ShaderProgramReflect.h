#pragma once


namespace mark
{
	struct D3D11_SHADER_INPUT_ELEMENT
	{
		VERTEX_FORMAT VertexFormat;
		uint32_t SemanticIndex = 0;
	};

	struct D3D11_SHADER_INPUTLAYOUT
	{
		D3D11_SHADER_INPUT_ELEMENT InputElements[(int)VERTEX_FORMAT_INDEX::EMAX];
		uint32_t VertexFormats;
	};

	enum class D3D11_SHADER_PARAM_TYPE : uint32_t
	{
		UNKNOWN = 0,
		CONSTANT_BUFFER,
		SAMPLER,
		TEXTURE,

		EMAX
	};

	struct D3D11_SHADER_PARAM
	{
#if defined(_DEBUG) || defined(DEBUG)
		char Name[64];
#endif //
		D3D11_SHADER_PARAM_TYPE Type;
		name_hash NameHash;
		uint32_t BindPoint;
		uint32_t Size;
	};

	struct D3D11_SHADER_REFLECT_DESC
	{
		D3D11_SHADER_INPUTLAYOUT InputLayout;
		D3D11_SHADER_PARAM Constants[16];
		D3D11_SHADER_PARAM Samplers[16];
		D3D11_SHADER_PARAM Textures[16];
	};

}
