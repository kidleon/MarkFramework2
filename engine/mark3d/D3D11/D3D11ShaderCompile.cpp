#include "pch.h"
#include "D3D11RenderDef.h"
#include "D3D11ShaderCompile.h"

#include "Log.h"


BOOL __stdcall D3D11CompileShader(
	const D3D11_SHADER_COMPILE_DESC& CompileDesc,
	ID3DBlob** ppBlob
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

		(*ppBlob) = nullptr;

		return FALSE;
	}

	if (pErrorBlob)
		pErrorBlob->Release();

	(*ppBlob) = pResultBlob;

	return TRUE;
}

