#pragma once


namespace mark
{
	struct Unknown
	{
		virtual void AddRef() = 0;
		virtual void Release() = 0;
	};

	/**
	 * @brief 해당 인터페이스는 외부에서 참조할 수 없는 인터페이스입니다. AddRef와 Release 메서드는 private으로 선언되어 있어 외부에서 호출할 수 없습니다.
	 * 해당 인터페이스는 외부에서 참조카운팅을 할수 없도록 하기위해 존재 합니다.
	 * 외부에서 참조카운팅이 불가능하기 때문에 해당 인터페이스를 상속받는 클래스는 외부에서 참조카운팅이 불가능한 객체가 됩니다.
	 */
	struct PrivateUnknown
	{
	private:
		virtual void AddRef() = 0;
		virtual void Release() = 0;
	};

}
