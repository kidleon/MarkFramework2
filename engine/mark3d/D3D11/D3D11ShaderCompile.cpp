#include "pch.h"
#include "D3D11RenderDef.h"
#include "D3D11ShaderCompile.h"

#include "Log.h"

BOOL __stdcall D3D11ShaderReflect(
	ID3DBlob* pShaderBlob,
	D3D11_SHADER_PARAMS** ppShaderParams,
	UINT32* pNumShaderParams
);

BOOL __stdcall D3D11CompileShader(
	const D3D11_SHADER_COMPILE_DESC& CompileDesc,
	D3D11_SHADER_COMPILE_RESULT& Result
)
{
	UINT32 ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	if (CompileDesc.Debug)
	{
		ShaderFlags |= D3DCOMPILE_DEBUG;
		ShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	ID3DBlob* pResultBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	D3D_SHADER_MACRO D3D11_SHADER_MACROS[32 + 1] = {};

	if (CompileDesc.DefineCount)
	{
		DWORD MaxDefines = T_MIN(32u, CompileDesc.DefineCount);

		for (DWORD i = 0; i < MaxDefines; ++i)
		{
			D3D11_SHADER_MACROS[i].Name = CompileDesc.szShaderDefines[i];
			D3D11_SHADER_MACROS[i].Definition = CompileDesc.szShaderDefines[i];
		}
	}

	HRESULT hr = D3DCompile(
		CompileDesc.pBuffer,
		CompileDesc.BufferSize,
		nullptr,
		(CompileDesc.DefineCount == 0) ? nullptr : D3D11_SHADER_MACROS,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		CompileDesc.szEntryPoint,
		CompileDesc.szShaderModel,
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

	D3D11_SHADER_PARAMS* pShaderParams = nullptr;
	UINT32 numShaderParams = 0;
	BOOL reflectResult = D3D11ShaderReflect(
		pResultBlob,
		&pShaderParams,
		&numShaderParams
	);

	if (!reflectResult)
	{
		SYS_LOG_E("D3D11CompileShader: Shader reflection failed");
		pResultBlob->Release();
		return FALSE;
	}

	Result.pShaderBlob = pResultBlob;
	Result.pShaderParams = pShaderParams;
	Result.NumShaderParams = numShaderParams;

	return TRUE;
}

BOOL __stdcall D3D11ShaderReflect(
	ID3DBlob* pShaderBlob,
	D3D11_SHADER_PARAMS** ppShaderParams,
	UINT32* pNumShaderParams
)
{
	if (!pShaderBlob || !ppShaderParams || !pNumShaderParams)
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

	int numShaderParams = shader_desc.BoundResources + shader_desc.ConstantBuffers;

	(*ppShaderParams) = (D3D11_SHADER_PARAMS*)MARK_POOL_ALLOC(sizeof(D3D11_SHADER_PARAMS) * numShaderParams);
	(*pNumShaderParams) = numShaderParams;

	int index = 0;
	for (int c = 0; c < shader_desc.ConstantBuffers; ++c)
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

		(*ppShaderParams)[index].ParamType = CPARAM_CONSTANT;
		(*ppShaderParams)[index].Name = buffer_desc.Name;
		(*ppShaderParams)[index].BindPoint = bindDesc.BindPoint;
		(*ppShaderParams)[index].Size = buffer_desc.Size;
		index++;
	}

	for(int r = 0; r < shader_desc.BoundResources; ++r)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
		hr = pReflector->GetResourceBindingDesc(r, &bindDesc);
		if (FAILED(hr))
		{
			SYS_LOG_E("D3D11ShaderReflect: GetResourceBindingDesc failed");
			continue;
		}

		SHADER_PARAM_TYPE paramType;
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

		(*ppShaderParams)[index].ParamType = paramType;
		(*ppShaderParams)[index].Name = bindDesc.Name;
		(*ppShaderParams)[index].BindPoint = bindDesc.BindPoint;
		(*ppShaderParams)[index].Size = bindDesc.BindCount;
		index++;
	}

	return TRUE;
}