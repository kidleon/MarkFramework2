#include "pch.h"
#include "Timer.h"

#include <chrono>


namespace mark
{
	constexpr uint64_t TIMER_MAGIC = 0xDEADBEEFDEADBEEF;

	struct high_res_timer
	{
		using clock_t = std::chrono::high_resolution_clock;
		using time_point = std::chrono::time_point<clock_t>;
		using duration = std::chrono::duration<double>;

		uint64_t magic = TIMER_MAGIC; // 디버깅용 매직 넘버
		time_point _created_time{};
		time_point _start_time{};
		time_point _stop_time{};
		mutable time_point _last_elapsed_time{};
		duration   _accumulated{};
		bool       _is_running{ false };
	};

	HANDLE timer::highrestimer_create()
	{
		high_res_timer* timer = CORE_SPOOL_NEW(high_res_timer);

		if (!timer) return nullptr;

		timer->_created_time = high_res_timer::clock_t::now();
		timer->_last_elapsed_time = timer->_created_time;
		timer->_accumulated = high_res_timer::duration::zero();
		timer->_is_running = false;

		return static_cast<HANDLE>(timer);
	}

	void timer::highrestimer_destroy(HANDLE handle)
	{
		if (!handle) return;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);

		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return;
		}

		CORE_SPOOL_DELETE(high_res_timer, timer);
	}

	void timer::highrestimer_start(HANDLE handle)
	{
		if (!handle) return;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);
		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return;
		}

		timer->_accumulated = high_res_timer::duration::zero();
		timer->_start_time = high_res_timer::clock_t::now();
		timer->_is_running = true;
	}

	void timer::highrestimer_stop(HANDLE handle)
	{
		if (!handle) return;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);
		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return;
		}

		if (!timer->_is_running) return;

		timer->_stop_time = high_res_timer::clock_t::now();
		timer->_accumulated += std::chrono::duration_cast<high_res_timer::duration>(timer->_stop_time - timer->_start_time);
		timer->_is_running = false;
	}

	void timer::highrestimer_resume(HANDLE handle)
	{
		if (!handle) return;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);
		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return;
		}

		if (timer->_is_running) return;

		timer->_start_time = high_res_timer::clock_t::now();
		timer->_is_running = true;
	}

	void timer::highrestimer_reset(HANDLE handle)
	{
		if (!handle) return;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);
		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return;
		}

		timer->_accumulated = high_res_timer::duration::zero();
		timer->_start_time = high_res_timer::clock_t::now();
	}

	double timer::highrestimer_gettime(HANDLE handle)
	{
		if (!handle) return 0.0;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);
		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return 0.0;
		}

		const auto now = high_res_timer::clock_t::now();
		const auto elapsed = std::chrono::duration_cast<high_res_timer::duration>(now - timer->_created_time);

		return elapsed.count();
	}

	double timer::highrestimer_getelapsedtime(HANDLE handle)
	{
		if (!handle) return 0.0;

		high_res_timer* timer = static_cast<high_res_timer*>(handle);
		if (timer->magic != TIMER_MAGIC)
		{
			assert(false && "Invalid high_res_timer handle");
			return 0.0f;
		}

		const auto now = high_res_timer::clock_t::now();
		const auto elapsed = std::chrono::duration_cast<high_res_timer::duration>(now - timer->_last_elapsed_time);

		timer->_last_elapsed_time = now;

		return elapsed.count();
	}

	void timer::get_timestamp(
		char* buffer,
		size_t size
	)
	{
		if (!buffer || size == 0) return;

		time_t now;
		struct tm* timeinfo;

		time(&now);
		timeinfo = localtime(&now);
		strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
	}
}
