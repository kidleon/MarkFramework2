#ifndef __I_VERTEX_SHADER_H__
#define __I_VERTEX_SHADER_H__


interface IVertexShader : IAsset
{
public:
	virtual int32 GetBindIndexByName(const NameHash& Name) const = 0;

	virtual void SetConstant(const NameHash& Name, const void* pData, uint32 DataSize) = 0;
	virtual void SetConstant(int32 BindIndex, const void* pData, uint32 DataSize) = 0;

};


#endif // __I_VERTEX_SHADER_H__
