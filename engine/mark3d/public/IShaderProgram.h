#ifndef __I_SHADER_PROGRAM_H__
#define __I_SHADER_PROGRAM_H__

#include "IAssetImpl.h"


interface IShaderProgram : IASSET_IMPL<ASSET_TYPE::SHADER_PROGRAM>
{
public:
	virtual int32 GetBindIndexByName(const NameHash& Name) const = 0;

	virtual void SetConstant(const NameHash& Name, const void* pData, uint32 DataSize) = 0;
	virtual void SetConstant(int32 BindIndex, const void* pData, uint32 DataSize) = 0;

	virtual void SetTexture1D(const NameHash& Name, ITexture1D* pTexture) = 0;
	virtual void SetTexture1D(int32 BindIndex, ITexture1D* pTexture) = 0;

	virtual void SetTexture2D(const NameHash& Name, ITexture2D* pTexture) = 0;
	virtual void SetTexture2D(int32 BindIndex, ITexture2D* pTexture) = 0;

};

#endif // __I_SHADER_PROGRAM_H__