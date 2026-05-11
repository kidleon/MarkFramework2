#pragma once
#include <d3d11.h>
#include "RenderDef.h"


namespace mark
{
	static constexpr D3D11_USAGE D3D11_IMPL_BUFFER_USAGE[(int)BUFFER_USAGE::EMAX] =
	{
		D3D11_USAGE_DEFAULT,
		D3D11_USAGE_DYNAMIC
	};

	static constexpr D3D11_BIND_FLAG D3D11_IMPL_BUFFER_BIND_FLAGS[(int)BUFFER_TYPE::EMAX] =
	{
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_BIND_INDEX_BUFFER,
		D3D11_BIND_CONSTANT_BUFFER
	};



	//---------------------------------------------------------------------------------------------
	// D3D11 셰이더 리플렉션 관련 구조체 정의
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
		CONSTANT,
		SAMPLER,
		TEXTURE,

		EMAX
	};

	constexpr size_t SHADER_PARAM_NAME_SIZE = 64;

	struct D3D11_SHADER_PARAM
	{
#if defined(_DEBUG) || defined(DEBUG)
		char Name[SHADER_PARAM_NAME_SIZE];
#endif //
		D3D11_SHADER_PARAM_TYPE Type;
		name_hash NameHash;
		uint32_t BindPoint;
		uint32_t Size;
	};


	constexpr uint32_t MAX_SHADER_PARAMS = 16;

	struct D3D11_SHADER_REFLECT_DESC
	{
		D3D11_SHADER_INPUTLAYOUT InputLayout;
		D3D11_SHADER_PARAM Constants[MAX_SHADER_PARAMS];
		D3D11_SHADER_PARAM Samplers[MAX_SHADER_PARAMS];
		D3D11_SHADER_PARAM Textures[MAX_SHADER_PARAMS];
		D3D11_SHADER_PARAM Unknowns[MAX_SHADER_PARAMS];
	};

	//---------------------------------------------------------------------------------------------
}
