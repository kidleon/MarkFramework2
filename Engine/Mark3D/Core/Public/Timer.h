#pragma once


namespace mark
{
	/**
	* @brief 고해상도 타이머를 제공하는 클래스입니다.
	*/
	class MARKENGINE_API timer
	{
	public:
		/**
		* @brief 고해상도 타이머 객체를 생성한다.
		* @return 성공시 타이머 핸들 반환 실패시 nullptr 반환한다.
		*/
		static HANDLE highrestimer_create();

		/**
		* @brief 고해상도 타이머 객체를 파괴한다. 파괴를 안할 경우 메모리 누수가 발생한다.
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 없음
		*/
		static void highrestimer_destroy(HANDLE handle);

		/**
		* @brief 고해상도 타이머를 시작한다
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 없음
		*/
		static void highrestimer_start(HANDLE handle);

		/**
		* @brief 고해상도 타이머를 정지한다.
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 없음
		*/
		static void highrestimer_stop(HANDLE handle);

		/**
		* @brief highrestimer_stop 함수로 부터 정지된 타이머를 다시 시작한다.
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 없음
		*/
		static void highrestimer_resume(HANDLE handle);

		/**
		* @brief 고해상도 타이머를 리셋한다. GetTime으로 얻을수 있는 시작점을 다시 0부터 시작한다.
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 없음
		*/
		static void highrestimer_reset(HANDLE handle);

		/**
		* @brief 고해상도 타이머가 생성된 시점으로 부터 지난 시간(1.0 = 1초)을 반환한다.
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 생성 시점으로 부터 지난 시간. (1.0 = 1초)
		*/
		static double highrestimer_gettime(HANDLE handle);

		/**
		* @brief 이전 호출된 highrestimer_getelapsedtime으로부터 지난 시간.
		* @param timer 생성된 고해상도 타이머 핸들.
		* @return 이전에 호출된 시점으로 부터 Delta Time을 반환한다 (1.0 = 1초)
		*/
		static double highrestimer_getelapsedtime(HANDLE handle);

		/**
		* @brief 현재 시간을 "YYYY-MM-DD HH:MM:SS" 형식의 문자열로 가져옵니다.
		* @param buffer 시간을 저장할 버퍼
		* @param size 버퍼의 크기
		* @return 없음
		*/
		static void get_timestamp(
			char* buffer,
			size_t size
		);

	};
}
