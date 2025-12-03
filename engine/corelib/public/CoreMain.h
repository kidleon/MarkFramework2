#ifndef __CORE_MAIN_H__
#define __CORE_MAIN_H__


namespace mark
{
	class MARKENGINE_API CoreMain
	{
	public:
		CoreMain();
		virtual ~CoreMain();

		BOOL Initialize();
		void Shutdown();
	};
}


#endif // __CORE_MAIN_H__
