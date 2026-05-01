#pragma once


namespace mark
{
	class MARKENGINE_API PrimitiveBuffer
	{
	public:
		PrimitiveBuffer();
		~PrimitiveBuffer() noexcept;

	private:
		PrimitiveBufferHandle m_PrimitiveBufferHandle = 0;

	};

}
