#include "pch.h"
#include "log.h"

#include <cstdarg> 
#include <cstdio> 
#include <cwchar>
#include <memory>
#include <mutex> 
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "string_buffer.h"


namespace mark
{
	// 카테고리별 로거 인스턴스.
	std::shared_ptr<spdlog::logger> g_system_logger;
	std::shared_ptr<spdlog::logger> g_gameplay_logger;

	// initialize() 가 여러 번 호출되어도 안전하도록 가드.
	std::once_flag g_init_flag;
	bool g_initialized = false;

	static int SPDLOG_LEVEL_IMPL[static_cast<size_t>(log::level::max_lv)] = {
		spdlog::level::info,
		spdlog::level::warn,
		spdlog::level::err,
		spdlog::level::critical
	};

	inline spdlog::level::level_enum to_spdlog_level(uint32_t lv)
	{
		return (lv < static_cast<uint32_t>(log::level::max_lv)) ?
			static_cast<spdlog::level::level_enum>(SPDLOG_LEVEL_IMPL[lv]) : spdlog::level::info;
	}

	inline spdlog::logger* get_logger(log::category cat)
	{
		switch (cat)
		{
		case log::category::system:
			return g_system_logger.get();

		case log::category::gameplay:
			return g_gameplay_logger.get();
		}
		return nullptr;
	}

	void log::init(uint32_t level, uint32_t target, bool multi_thread)
	{
		// 두 번 이상 호출되어도 한 번만 셋업되도록 방어.
		std::call_once(g_init_flag, [&]()
			{
				std::vector<spdlog::sink_ptr> sinks;

				if (target & static_cast<uint32_t>(target::console))
				{
					// 콘솔 컬러 싱크 (멀티스레드 안전 버전 _mt).
					if (multi_thread)
						sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
					else
						sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
				}

				if (target & static_cast<uint32_t>(target::file))
				{
					// 회전 파일 싱크: 10MB 단위, 최대 5개 파일로 순환.
					// 경로/용량/개수는 프로젝트 정책에 맞게 조절.
					constexpr size_t kMaxFileSize = 10 * 1024 * 1024;
					constexpr size_t kMaxFiles = 5;
					if (multi_thread)
						sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/mark.log", kMaxFileSize, kMaxFiles));
					else
						sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>("logs/mark.log", kMaxFileSize, kMaxFiles));
				}

				// [2026-04-23 10:12:34.567] [system] [info] 메시지
				const std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v";

				const auto spd_level = to_spdlog_level(level);

				g_system_logger = std::make_shared<spdlog::logger>("system", sinks.begin(), sinks.end());
				g_system_logger->set_level(spd_level);
				g_system_logger->set_pattern(pattern);
				// critical 이상 로그는 즉시 flush 하여 크래시 직전 유실 방지.
				g_system_logger->flush_on(spdlog::level::critical);

				g_gameplay_logger = std::make_shared<spdlog::logger>("gameplay", sinks.begin(), sinks.end());
				g_gameplay_logger->set_level(spd_level);
				g_gameplay_logger->set_pattern(pattern);
				g_gameplay_logger->flush_on(spdlog::level::critical);

				try { spdlog::register_logger(g_system_logger); }
				catch (...) {}
				try { spdlog::register_logger(g_gameplay_logger); }
				catch (...) {}

				g_initialized = true;
			});
	}

	void log::shutdown()
	{
		if (!g_initialized) return;

		if (g_system_logger) [[likely]]
			g_system_logger->flush();

		if (g_gameplay_logger) [[likely]]
			g_gameplay_logger->flush();

		spdlog::shutdown();
		g_system_logger.reset();
		g_gameplay_logger.reset();

		g_initialized = false;
	}

	void log::dispatch_impl(
		std::source_location loc,
		log::category cat,
		log::level lv,
		std::string_view fmt,
		std::format_args args
	)
	{
		if (0 == fmt.length())
			return;

		auto logger = get_logger(cat);
		if (!logger)
			return;

		//thread_local string_buffer<4096, ALLOC::SYS<char>> str_buf;
		thread_local string_buffer<4096> str_buf;
		str_buf.clear();
		str_buf.format(fmt, args);
		str_buf.append(" (at ");
		str_buf.append(loc.file_name());
		str_buf.append(":");
		str_buf.append(std::to_string(loc.line()));
		str_buf.append(")");
		std::string_view final_msg = str_buf.to_string_view();
		auto spd_level = to_spdlog_level(static_cast<uint32_t>(lv));

		logger->log(spd_level, final_msg);
	}
} // namespace mark
