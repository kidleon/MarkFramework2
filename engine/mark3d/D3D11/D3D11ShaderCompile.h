#ifndef __D3D11_SHADER_COMPILE_H__
#define __D3D11_SHADER_COMPILE_H__


extern "C" BOOL __stdcall D3D11CompileShader(
	const D3D11_SHADER_COMPILE_DESC& CompileDesc,
	D3D11_SHADER_COMPILE_RESULT& Result
);


#endif // __D3D11_SHADER_COMPILE_H__
