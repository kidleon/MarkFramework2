#pragma once


namespace mark
{
	struct D3D11_SHADER_COMPILE_RESULT
	{
		bool Success = false;
		sys_string ErrorMessage;
		ID3DBlob* pShaderBlob = nullptr;
		D3D11_SHADER_REFLECT_DESC ReflectDesc = {};
	};

	BOOL __CompileShaderProgram(
		const ShaderProgramCreateDesc& CreateDesc,
		D3D11_SHADER_COMPILE_RESULT& Result
	);
}
