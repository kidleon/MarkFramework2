#ifndef __D3D11_SHADER_META_H__
#define __D3D11_SHADER_META_H__


class D3D11ShaderMeta
{
	struct CBuffer
	{
		char Name[64];
		uint32 Size;
		uint32 BindPoint;
	};

	struct Texture
	{
		char Name[64];
		uint32 BindPoint;
	};

	struct Sampler
	{
		char Name[64];
		uint32 BindPoint;
	};

public:



};

#endif // __D3D11_SHADER_META_H__