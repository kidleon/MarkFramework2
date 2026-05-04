#pragma once
#include <memory_resource>
#include <unordered_map>
#include <source_location>
#include <atomic>
#include <functional>


namespace mark
{
	/**
	* @brief 메모리 할당/해제 통계를 추적하는 메모리 프로파일러 클래스
	*/
	class memory_tracker
	{
		static constexpr size_t MAX_FILE_NAME = 256;
		static constexpr size_t MAX_FUNCTION_NAME = 64;

		/**
		* @brief 개별 할당에 대한 정보 구조체
		* @note 이 구조체는 각 할당에 대한 상세 정보를 저장하는 데 사용된다. 할당된 바이트 수, 정렬, 소스 파일, 라인 번호, 함수 이름 등을 포함한다.
		*/
		struct allocation_info
		{
			uint32_t bytes;
			uint32_t alignment;
			char file[MAX_FILE_NAME];
			char function[MAX_FUNCTION_NAME];
			uint32_t line;

			allocation_info()
				: bytes(0)
				, alignment(0)
				, line(0)
			{
				file[0] = '\0';
				function[0] = '\0';
			}

			allocation_info(const allocation_info& other)
			{
				bytes = other.bytes;
				alignment = other.alignment;
				safe_strcpy(file, MAX_FILE_NAME, other.file);
				safe_strcpy(function, MAX_FUNCTION_NAME, other.function);
				line = other.line;
			}

			allocation_info(allocation_info&& other) noexcept
			{
				bytes = other.bytes;
				alignment = other.alignment;
				safe_strcpy(file, MAX_FILE_NAME, other.file);
				safe_strcpy(function, MAX_FUNCTION_NAME, other.function);
				line = other.line;
			}

			inline allocation_info& operator=(const allocation_info& other)
			{
				if (this != &other)
				{
					bytes = other.bytes;
					alignment = other.alignment;
					safe_strcpy(file, MAX_FILE_NAME, other.file);
					safe_strcpy(function, MAX_FUNCTION_NAME, other.function);
					line = other.line;
				}
				return *this;
			}

			inline allocation_info& operator=(allocation_info&& other) noexcept
			{
				if (this != &other)
				{
					bytes = other.bytes;
					alignment = other.alignment;
					safe_strcpy(file, MAX_FILE_NAME, other.file);
					safe_strcpy(function, MAX_FUNCTION_NAME, other.function);
					line = other.line;
				}
				return *this;
			}


		};

	public:
		static memory_tracker& get()
		{
			static memory_tracker get; // 함수 내 정적 변수로 싱글톤 인스턴스 생성
			return get;
		}

		memory_tracker();
		virtual ~memory_tracker();

		// 복사 및 이동 생성자/할당 연산자 삭제 (싱글톤 또는 고유 인스턴스 용도)
		memory_tracker(const memory_tracker&) = delete;
		memory_tracker(memory_tracker&&) = delete;
		memory_tracker& operator=(const memory_tracker&) = delete;
		memory_tracker& operator=(memory_tracker&&) = delete;

		/**
		* @brief 메모리 할당 이벤트 핸들러, 이 함수는 메모리 할당이 발생할 때마다 호출되어야 하며, 할당된 메모리에 대한 정보를 기록하고 통계를 업데이트한다.
		* @param ptr 할당된 메모리의 포인터
		* @param bytes 할당된 바이트 수
		* @param alignment 할당된 메모리의 정렬
		* @param file 할당이 발생한 소스 파일 이름
		* @param line 할당이 발생한 소스 파일의 라인 번호
		* @param function 할당이 발생한 함수 이름
		* @return 없음
		*/
		void on_allocate(void* ptr, size_t bytes, size_t alignment, std::source_location location);

		/**
		* @brief 메모리 해제 이벤트 핸들러, 이 함수는 메모리 해제가 발생할 때마다 호출되어야 하며, 해제된 메모리에 대한 정보를 기록하고 통계를 업데이트한다.
		* @param ptr 해제된 메모리의 포인터
		* @return 없음
		*/
		void on_deallocate(void* ptr);

		/**
		* @brief 프로파일러 통계 및 기록을 초기 상태로 재설정하는 함수, 이 함수는 모든 통계와 기록을 초기화하여 프로파일러를 새롭게 시작할 수 있도록 한다.
		* @return 없음
		*/
		void reset();


		/**
		* @brief 메모리 할당/해제 통계 정보 접근자
		* @note 해제된 바이트 수는 빼지 않는다.
		* @return 총 할당된 모든 바이트 수
		*/
		inline size_t get_total_allocated() const noexcept { return m_total_allocated.load(); }

		/**
		* @brief 메모리 할당/해제 통계 정보 접근자
		* @return 총 해제된 모든 바이트 수
		*/
		inline size_t get_total_deallocated() const noexcept { return m_total_deallocated.load(); }

		/**
		* @brief 현재 실제 사용 중인 메모리 양 (할당된 바이트 수 - 해제된 바이트 수)
		* @return 현재 사용 중인 메모리 양
		*/
		inline size_t get_current_usage() const noexcept { return m_current_usage.load(); }

		/**
		* @brief 메모리 사용량의 최대값 (peak usage)
		* @return 메모리 사용량의 최대값
		*/
		inline size_t get_peak_usage() const noexcept { return m_peak_usage.load(); }

		/**
		* @brief 총 할당 횟수
		* @return 총 할당 횟수
		*/
		inline size_t get_alloc_count() const noexcept { return m_alloc_count.load(); }

		/**
		* @brief 프로파일러가 내부적으로 사용하는 메모리 양, 각 할당 정보 구조체의 크기와 총 할당 횟수를 곱하여 계산한다.
		* @return 프로파일러가 내부적으로 사용하는 메모리 양 (예측치)
		*/
		inline size_t get_profile_usage() const noexcept { return m_alloc_count.load() * sizeof(allocation_info); }

		/**
		* @brief 현재까지의 메모리 할당/해제 통계 정보를 콘솔에 출력하는 함수
		* @return 없음
		*/
		void print_report(std::function<void(const char*)> func) const;


	private:
		std::atomic<size_t> m_total_allocated;
		std::atomic<size_t> m_total_deallocated;
		std::atomic<size_t> m_current_usage;
		std::atomic<size_t> m_peak_usage;
		std::atomic<size_t> m_alloc_count;

		std::unordered_map<uintptr_t, allocation_info> m_allocations; // 할당된 메모리 주소를 키로, 할당 정보를 값으로 저장하는 맵
		

	};
}

