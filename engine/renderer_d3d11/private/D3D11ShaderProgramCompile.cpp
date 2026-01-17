#include "pch.h"
#include "D3D11ShaderProgramCompile.h"


BOOL __stdcall D3D11ShaderReflect(
	ID3DBlob* pShaderBlob,
	D3D11_SHADER_COMPILE_RESULT* pCompileResult
);

BOOL __stdcall D3D11CompileShaderProgram(
	const D3D11_SHADER_COMPILE_DESC* pCompileDesc,
	D3D11_SHADER_COMPILE_RESULT* pResult
)
{
	UINT32 ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	if (pCompileDesc->Debug)
	{
		ShaderFlags |= D3DCOMPILE_DEBUG;
		ShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	ID3DBlob* pResultBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	D3D_SHADER_MACRO D3D11_SHADER_MACROS[32 + 1] = {};

	if (pCompileDesc->NumDefines)
	{
		DWORD MaxDefines = T_MIN(32u, pCompileDesc->NumDefines);

		for (DWORD i = 0; i < MaxDefines; ++i)
		{
			D3D11_SHADER_MACROS[i].Name = pCompileDesc->szShaderDefines[i];
			D3D11_SHADER_MACROS[i].Definition = pCompileDesc->szShaderDefines[i];
		}
	}

	HRESULT hr = D3DCompile(
		pCompileDesc->pBuffer,
		pCompileDesc->BufferSize,
		nullptr,
		(pCompileDesc->NumDefines == 0) ? nullptr : D3D11_SHADER_MACROS,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pCompileDesc->szEntryPoint,
		pCompileDesc->szShaderModel,
		ShaderFlags,
		0,
		&pResultBlob,
		&pErrorBlob
	);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			const char* szErrorMessage = reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer());
			SYS_LOG_E("[SHADER COMPILE ERROR]");
			SYS_LOG_E(szErrorMessage);
			pErrorBlob->Release();
		}

		return FALSE;
	}

	if (pErrorBlob)
		pErrorBlob->Release();

	BOOL ReflectResult = D3D11ShaderReflect(
		pResultBlob,
		pResult
	);

	if (!ReflectResult)
	{
		SYS_LOG_E("D3D11CompileShader: Shader reflection failed");
		pResultBlob->Release();
		return FALSE;
	}

	pResult->pShaderBlob = pResultBlob;

	return TRUE;
}


BOOL __stdcall D3D11ShaderReflect(
	ID3DBlob* pShaderBlob,
	D3D11_SHADER_COMPILE_RESULT* pCompileResult
)
{
	if (!pShaderBlob || !pCompileResult)
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
		SYS_LOG_E("D3D11ShaderReflect: D3DReflect failed");
		return FALSE;
	}

	D3D11_SHADER_DESC shader_desc = {};
	hr = pReflector->GetDesc(&shader_desc);
	if (FAILED(hr))
	{
		SYS_LOG_E("D3D11ShaderReflect: GetDesc failed");
		pReflector->Release();
		return FALSE;
	}

	// SHADER INPUT_LAYOUT
	VERTEX_FORMAT VertexFormats[MAX_VERTEX_FORMAT] = {};
	UINT32 VertexFormat = 0;
	UINT32 NumVertexFormat = 0;

	for (UINT32 i = 0; i < shader_desc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC ParamDesc;
		pReflector->GetInputParameterDesc(i, &ParamDesc);

		VERTEX_FORMAT& VertexFormatRef = VertexFormats[NumVertexFormat++];
		if (!fstrcmp(ParamDesc.SemanticName, "POSITION"))
		{
			VertexFormatRef = VERTEX_FORMAT::POSITION;
			VertexFormat |= (UINT32)VERTEX_FORMAT::POSITION;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "NORMAL"))
		{
			VertexFormatRef = VERTEX_FORMAT::NORMAL;
			VertexFormat |= (UINT32)VERTEX_FORMAT::NORMAL;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD") || !fstrcmp(ParamDesc.SemanticName, "TEXCOORD0"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "COLOR") || !fstrcmp(ParamDesc.SemanticName, "COLOR0"))
		{
			VertexFormatRef = VERTEX_FORMAT::COLOR;
			VertexFormat |= (UINT32)VERTEX_FORMAT::COLOR;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TANGENT") || !fstrcmp(ParamDesc.SemanticName, "TANGENT0"))
		{
			VertexFormatRef = VERTEX_FORMAT::TANGENT;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TANGENT;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "BINORMAL") || !fstrcmp(ParamDesc.SemanticName, "BINORMAL0"))
		{
			VertexFormatRef = VERTEX_FORMAT::BINORMAL;
			VertexFormat |= (UINT32)VERTEX_FORMAT::BINORMAL;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "BLENDINDICES") || !fstrcmp(ParamDesc.SemanticName, "BLENDINDICES0"))
		{
			VertexFormatRef = VERTEX_FORMAT::BONE;
			VertexFormat |= (UINT32)VERTEX_FORMAT::BONE;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "BLENDWEIGHT") || !fstrcmp(ParamDesc.SemanticName, "BLENDWEIGHT0"))
		{
			VertexFormatRef = VERTEX_FORMAT::WEIGHT;
			VertexFormat |= (UINT32)VERTEX_FORMAT::WEIGHT;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD1"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD1;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD1;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD2"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD2;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD2;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD3"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD3;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD3;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD4"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD4;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD4;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD5"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD5;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD5;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD6"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD6;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD6;
		}
		else if (!fstrcmp(ParamDesc.SemanticName, "TEXCOORD7"))
		{
			VertexFormatRef = VERTEX_FORMAT::TEXCOORD7;
			VertexFormat |= (UINT32)VERTEX_FORMAT::TEXCOORD7;
		}
		else
		{
		}
	}

	pCompileResult->BufferFormat = VertexFormat;
	memcpy(pCompileResult->VertexFormats, VertexFormats, sizeof(VERTEX_FORMAT) * MAX_VERTEX_FORMAT);
	pCompileResult->NumVertexFormat = NumVertexFormat;

	int NumShaderParams = shader_desc.BoundResources + shader_desc.ConstantBuffers;
	pCompileResult->NumShaderParams = NumShaderParams;

	int index = 0;
	for (UINT32 c = 0; c < shader_desc.ConstantBuffers; ++c)
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

		pCompileResult->ShaderParams[index].ParamType = CPARAM_CONSTANT;
		pCompileResult->ShaderParams[index].Name = buffer_desc.Name;
		pCompileResult->ShaderParams[index].BindPoint = bindDesc.BindPoint;
		pCompileResult->ShaderParams[index].Size = buffer_desc.Size;
		index++;
	}

	for (UINT32 r = 0; r < shader_desc.BoundResources; ++r)
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

		pCompileResult->ShaderParams[index].ParamType = paramType;
		pCompileResult->ShaderParams[index].Name = bindDesc.Name;
		pCompileResult->ShaderParams[index].BindPoint = bindDesc.BindPoint;
		pCompileResult->ShaderParams[index].Size = bindDesc.BindCount;
		index++;
	}

	return TRUE;
}