#pragma once


namespace mark
{
	class MARKENGINE_API random
	{
	public:
		random() noexcept;
		explicit random(uint64_t s) noexcept;

		/**
		 * @brief 시드를 세팅
		 * @param s 시드값
		 */
		void seed(uint64_t s);

		/**
		 * @brief 다음 난수값을 반환 (signed 16비트 정수)
		 * @return 다음 난수값
		 */
		int16_t next_int16();

		/**
		 * @brief 다음 난수값을 반환 (signed 16비트 정수), min과 max값이 포함됨.
		 * @param min 난수값의 최소값
		 * @param max 난수값의 최대값
		 * @return 다음 난수값
		 */
		int16_t next_int16(int16_t min, int16_t max);

		/**
		 * @brief 다음 난수값을 반환 (unsigned 16비트 정수)
		 * @return 다음 난수값
		 */
		uint16_t next_uint16();

		/**
		* @brief 다음 난수값을 반환 (unsigned 16비트 정수), min과 max값이 포함됨.
		* @param min 난수값의 최소값
		*/
		uint16_t next_uint16(uint16_t min, uint16_t max);

		/**
		 * @brief 다음 난수값을 반환 (signed 32비트 정수)
		 * @return 다음 난수값
		 */
		int32_t next_int32();

		/**
		 * @brief 다음 난수값을 반환 (signed 32비트 정수), min과 max값이 포함됨.
		 * @param min 난수값의 최소값
		 * @param max 난수값의 최대값
		 * @return 다음 난수값
		 */
		int32_t next_int32(int min, int max);

		/**
		 * @brief 다음 난수값을 반환 (unsigned 32비트 정수)
		 * @return 다음 난수값
		 */
		uint32_t next_uint32();

		/**
		 * @brief 다음 난수값을 반환 (unsigned 32비트 정수), min과 max값이 포함됨.
		 * @param min 난수값의 최소값
		 * @param max 난수값의 최대값
		 * @return 다음 난수값
		 */	
		uint32_t next_uint32(uint32_t min, uint32_t max);

		/**
		 * @brief 다음 난수값을 반환 (signed 64비트 정수)
		 * @return 다음 난수값
		 */
		int64_t next_int64();

		/**
		* @brief 다음 난수값을 반환 (signed 64비트 정수), min과 max값이 포함됨.
		* @param min 난수값의 최소값
		*/
		int64_t next_int64(int64_t min, int64_t max);

		/**
		* @brief 다음 난수값을 반환 (unsigned 64비트 정수)
		* @return 다음 난수값
		*/
		uint64_t next_uint64();

		/**
		 * @brief 다음 난수값을 반환 (unsigned 64비트 정수), min과 max값이 포함됨.
		 * @param min 난수값의 최소값
		 * @param max 난수값의 최대값
		 * @return 다음 난수값
		 */
		uint64_t next_uint64(uint64_t min, uint64_t max);

		/**
		 * @brief 다음 난수값을 반환 (float), 0.0f 이상 1.0f 미만의 값을 반환
		 * @return 다음 난수값
		 */
		float next_float();

		/**
		 * @brief 다음 난수값을 반환 (float), min값은 포함되지만 max값은 포함되지 않음.
		 * @param min 난수값의 최소값
		 * @param max 난수값의 최대값
		 * @return 다음 난수값
		 */
		float next_float(float min, float max);

	private:
		uint64_t _state;

	};
}
