#pragma once


namespace mark
{
	struct unknown
	{
		virtual void addref() = 0;
		virtual void release() = 0;
	};

	using Unknown = unknown;
}
