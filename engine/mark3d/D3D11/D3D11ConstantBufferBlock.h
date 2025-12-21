#ifndef __D3D11_CONSTANT_BUFFER_BLOCK_H__
#define __D3D11_CONSTANT_BUFFER_BLOCK_H__


struct D3D11_CONSTANT_BUFFER_BLOCK
{
	void* pData;
	size_t BufferSize;
	size_t BufferSizeIndex;
	LINK_NODE LinkNode;

	constexpr D3D11_CONSTANT_BUFFER_BLOCK()
		: pData(nullptr)
		, BufferSize(0)
		, BufferSizeIndex(0)
		, LinkNode{}
	{
		LinkNode.data = this;
	}
};


#endif // __D3D11_CONSTANT_BUFFER_BLOCK_H__
