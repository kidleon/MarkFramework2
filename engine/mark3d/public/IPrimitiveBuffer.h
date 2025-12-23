#ifndef __I_PRIMITIVE_BUFFER_H__
#define __I_PRIMITIVE_BUFFER_H__

#include "IAsset.h"


interface IPrimitiveBuffer : public IAsset
{
	virtual size_t GetBufferSize() const noexcept = 0;

};


#endif // __I_PRIMITIVE_BUFFER_H__
