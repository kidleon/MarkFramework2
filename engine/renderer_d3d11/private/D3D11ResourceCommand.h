#pragma once


class D3D11PrimitiveBuffer;

struct D3D11_RESOURCE_COMMAND
{
	enum class COMMAND_FUNC : unsigned
	{
		UPDATE_PRIMITIVE_BUFFER,
	};

	COMMAND_FUNC CommandFunc;
	UINT32 PADDING_OR_RESERVED = 0;
	D3D11PrimitiveBuffer* pPrimitiveBuffer;

	LINK_NODE LinkNode;

	void Reset();
	
};
