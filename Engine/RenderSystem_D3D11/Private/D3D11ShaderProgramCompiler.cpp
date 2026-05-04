#include "pch.h"
#include "D3D11ShaderProgramCompiler.h"


namespace mark
{
	BOOL D3D11ShaderReflect(
		ID3DBlob* pShaderBlob,
		D3D11_SHADER_REFLECT_DESC* pShaderReflect
	);


	BOOL CompileShaderProgram(
		const ShaderProgramCreateDesc& CreateDesc,
		D3D11_SHADER_COMPILE_RESULT& Result
	)
	{
		UINT CompileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		if (CreateDesc.DebugMode)
		{
			CompileFlags |= D3DCOMPILE_DEBUG;
			CompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		}

		ID3DBlob* pResultBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;

		D3D_SHADER_MACRO D3D11_SHADER_MACROS[MAX_SHADER_DEFINE + 1] = {};

		if (CreateDesc.NumDefines)
		{
			uint32_t_t MaxDefines = std::min(MAX_SHADER_DEFINE, CreateDesc.NumDefines);
			for (uint32_t_t i = 0; i < MaxDefines; ++i)
			{
				D3D11_SHADER_MACROS[i].Name = CreateDesc.szShaderDefines[i];
				D3D11_SHADER_MACROS[i].Definition = CreateDesc.szShaderDefines[i];
			}
		}

		HRESULT hr = D3DCompile(
			CreateDesc.pShaderBytecode,
			CreateDesc.BytecodeSize,
			nullptr,
			(CreateDesc.NumDefines == 0) ? nullptr : D3D11_SHADER_MACROS,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			CreateDesc.szEntryPoint,
			CreateDesc.szShaderModel,
			CompileFlags,
			0,
			&pResultBlob,
			&pErrorBlob
		);

		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				const char* szErrorMessage = reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer());
				Result.Success = false;
				Result.ErrorMessage = szErrorMessage;
				pErrorBlob->Release();
			}

			return FALSE;
		}

		if (pErrorBlob)
			pErrorBlob->Release();

		BOOL ReflectResult = D3D11ShaderReflect(
			pResultBlob,
			&Result
		);

		if (!ReflectResult)
		{
			SYS_LOG_ERR("D3D11CompileShader: Shader reflection failed");
			pResultBlob->Release();
			return FALSE;
		}

		Result.pShaderBlob = pResultBlob;

		return TRUE;
	}

	BOOL D3D11ShaderReflect(
		ID3DBlob* pShaderBlob,
		D3D11_SHADER_REFLECT_DESC* pShaderReflect
	)
	{
		if (!pShaderBlob || !pShaderReflect)
			return FALSE;

		ID3D11ShaderReflection* pReflector = nullptr;

		HRESULT hr = D3DReflect(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			IID_ID3D11ShaderReflection,
			(void**)&pReflector
		);

		if (FAILED(hr))
		{
			return FALSE;
		}

		D3D11_SHADER_DESC ShaderDesc = {};
		hr = pReflector->GetDesc(&ShaderDesc);
		if (FAILED(hr))
		{
			pReflector->Release();
			return FALSE;
		}

		// SHADER INPUT_LAYOUT
		uint32_t VertexFormats = 0;
		uint32_t NumVertexFormat = 0;

		uint32_t MaxVertexFormat = std::min((uint32_t)ShaderDesc.InputParameters, (uint32_t)VERTEX_FORMAT_INDEX::EMAX);

		for (uint32_t i = 0; i < MaxVertexFormat; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC ParamDesc;
			pReflector->GetInputParameterDesc(i, &ParamDesc);

			pShaderReflect->InputLayout.InputElements[NumVertexFormat].SemanticIndex = ParamDesc.SemanticIndex;

			if (!safe_stricmp(ParamDesc.SemanticName, "POSITION"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::POSITION;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::POSITION;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "NORMAL"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::NORMAL;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::NORMAL;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "TEXCOORD") || !safe_stricmp(ParamDesc.SemanticName, "TEXCOORD0"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::TEXCOORD0;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::TEXCOORD0;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "COLOR") || !safe_stricmp(ParamDesc.SemanticName, "COLOR0"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::COLOR;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::COLOR;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "TANGENT") || !safe_stricmp(ParamDesc.SemanticName, "TANGENT0"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::TANGENT;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::TANGENT;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "BLENDINDICES") || !safe_stricmp(ParamDesc.SemanticName, "BLENDINDICES0"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::BONE;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::BONE;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "BLENDWEIGHT") || !safe_stricmp(ParamDesc.SemanticName, "BLENDWEIGHT0"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::WEIGHT;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::WEIGHT;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "TEXCOORD1"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::TEXCOORD1;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::TEXCOORD1;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "CUSTOM0") || !safe_stricmp(ParamDesc.SemanticName, "CUSTOM"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::CUSTOM0;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::CUSTOM0;
			}
			else if (!safe_stricmp(ParamDesc.SemanticName, "CUSTOM1"))
			{
				VertexFormats |= (uint32_t)VERTEX_FORMAT::CUSTOM1;
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::CUSTOM1;
			}
			else
			{
				pShaderReflect->InputLayout.InputElements[NumVertexFormat].VertexFormat = VERTEX_FORMAT::NONE;
				SYS_LOG_WRN_F("D3D11ShaderReflect: Unrecognized semantic name {}", ParamDesc.SemanticName);
			}

			NumVertexFormat++;
		}

		pShaderReflect->InputLayout.VertexFormats = VertexFormats;

		int NumShaderParams = ShaderDesc.BoundResources + ShaderDesc.ConstantBuffers;
		pShaderReflect->NumShaderParams = NumShaderParams;

		int index = 0;
		for (uint32_t c = 0; c < ShaderDesc.ConstantBuffers; ++c)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstBuffer = pReflector->GetConstantBufferByIndex(c);
			D3D11_SHADER_BUFFER_DESC buffer_desc = {};
			hr = pConstBuffer->GetDesc(&buffer_desc);
			if (FAILED(hr))
			{
				SYS_LOG_E("D3D11ShaderReflect: GetConstantBufferByIndex failed");
				continue;
			}

			if (!buffer_desc.Name || !fstrlen(buffer_desc.Name))
			{
				SYS_LOG_E("D3D11ShaderReflect: Constant buffer has no name");
				continue;
			}

			D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
			pReflector->GetResourceBindingDescByName(buffer_desc.Name, &bindDesc);

			pShaderReflect->ShaderParams[index].ParamType = CPARAM_CONSTANT;
			pShaderReflect->ShaderParams[index].Name = buffer_desc.Name;
			pShaderReflect->ShaderParams[index].BindPoint = bindDesc.BindPoint;
			pShaderReflect->ShaderParams[index].Size = buffer_desc.Size;
			index++;
		}

		for (uint32_t r = 0; r < ShaderDesc.BoundResources; ++r)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
			hr = pReflector->GetResourceBindingDesc(r, &bindDesc);
			if (FAILED(hr))
			{
				SYS_LOG_E("D3D11ShaderReflect: GetResourceBindingDesc failed");
				continue;
			}

			D3D11_SHADER_PARAM_TYPE paramType;
			switch (bindDesc.Type)
			{
				case D3D_SIT_TEXTURE:
					paramType = CPARAM_TEXTURE;
					break;

				case D3D_SIT_SAMPLER:
					paramType = CPARAM_SAMPLER;
					break;

				default:
					paramType = CPARAM_UNKNOWN;
					break;
			}

			pShaderReflect->ShaderParams[index].ParamType = paramType;
			pShaderReflect->ShaderParams[index].Name = bindDesc.Name;
			pShaderReflect->ShaderParams[index].BindPoint = bindDesc.BindPoint;
			pShaderReflect->ShaderParams[index].Size = bindDesc.BindCount;
			index++;
		}

		return TRUE;
	}
}
