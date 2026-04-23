#include "pch.h"
#include "log.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <cstdarg> 
#include <cstdio> 
#include <cwchar>
#include <memory>
#include <mutex> 
#include <string>
#include <vector>


namespace mark
{
	namespace
	{
		// 카테고리별 로거 인스턴스.
		std::shared_ptr<spdlog::logger> g_system_logger;
		std::shared_ptr<spdlog::logger> g_gameplay_logger;

		// initialize() 가 여러 번 호출되어도 안전하도록 가드.
		std::once_flag g_init_flag;
		bool g_initialized = false;
		
		/**
		* @brief log_level enum 값을 spdlog::level::level_enum 값으로 변환.
		 * - log_level::info     -> spdlog::level::info
		 * - log_level::warning  -> spdlog::level::warn
		 * - log_level::error    -> spdlog::level::err
		 * - log_level::critical -> spdlog::level::critical
		 *
		 * @param lv log_level enum 값을 uint32_t 로 전달받음.
		 * @return 대응하는 spdlog 레벨. 알 수 없는 값이면 info 로 fallback.
		*/
		spdlog::level::level_enum to_spdlog_level(uint32_t lv)
		{
			switch (static_cast<log_level>(lv))
			{
			case log_level::info:
				return spdlog::level::info;
			case log_level::warning:
				return spdlog::level::warn;
			case log_level::error:
				return spdlog::level::err;
			case log_level::critical:
				return spdlog::level::critical;
			}
			// 알 수 없는 값이면 가장 관대한 info 로 fallback.
			return spdlog::level::info;
		}

		spdlog::logger* get_logger(log_category cat)
		{
			switch (cat)
			{
			case log_category::system:
				return g_system_logger.get();

			case log_category::gameplay:
				return g_gameplay_logger.get();
			}
			return nullptr;
		}

		std::string format_va(const char* fmt, va_list args)
		{
			if (fmt == nullptr) return {};

			va_list args_copy;
			va_copy(args_copy, args);
			const int required = std::vsnprintf(nullptr, 0, fmt, args_copy);
			va_end(args_copy);

			if (required <= 0) return {};

			std::string buf(static_cast<size_t>(required), '\0');
			std::vsnprintf(buf.data(), buf.size() + 1, fmt, args);

			return buf;
		}

		void dispatch_narrow(
			log_category cat,
			spdlog::level::level_enum lv,
			const char* fmt, va_list args
		)
		{
			spdlog::logger* lg = get_logger(cat);
			if (lg == nullptr) return;                 // 초기화 전 호출 방어
			if (!lg->should_log(lv)) return;           // 레벨 필터 (성능)

			std::string msg = format_va(fmt, args);
			lg->log(lv, msg);
		}

	} // anonymous namespace


	// =============================================================================
	//  공개 API 구현
	// =============================================================================

	void log::initialize(uint32_t level, uint32_t target)
	{
		// 두 번 이상 호출되어도 한 번만 셋업되도록 방어.
		std::call_once(g_init_flag, [&]()
			{
				// 1) 타겟 플래그에 맞춰 sink 구성.
				std::vector<spdlog::sink_ptr> sinks;

				if (target & static_cast<uint32_t>(log_target::console))
				{
					// 콘솔 컬러 싱크 (멀티스레드 안전 버전 _mt).
					sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
				}

				if (target & static_cast<uint32_t>(log_target::file))
				{
					// 회전 파일 싱크: 10MB 단위, 최대 5개 파일로 순환.
					// 경로/용량/개수는 프로젝트 정책에 맞게 조절.
					constexpr size_t kMaxFileSize = 10 * 1024 * 1024;
					constexpr size_t kMaxFiles = 5;
					sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/mark.log", kMaxFileSize, kMaxFiles));
				}

				// 2) 공통 패턴. %n 자리에 로거 이름(system/gameplay)이 찍힌다.
				//    [2026-04-23 10:12:34.567] [system] [info] 메시지
				const std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v";

				const auto spd_level = to_spdlog_level(level);

				// 3) 카테고리별 로거 생성. sink 리스트를 공유하도록 동일하게 전달.
				g_system_logger = std::make_shared<spdlog::logger>("system", sinks.begin(), sinks.end());
				g_system_logger->set_level(spd_level);
				g_system_logger->set_pattern(pattern);
				// critical 이상 로그는 즉시 flush 하여 크래시 직전 유실 방지.
				g_system_logger->flush_on(spdlog::level::critical);

				g_gameplay_logger = std::make_shared<spdlog::logger>("gameplay", sinks.begin(), sinks.end());
				g_gameplay_logger->set_level(spd_level);
				g_gameplay_logger->set_pattern(pattern);
				g_gameplay_logger->flush_on(spdlog::level::critical);

				// 4) 전역 레지스트리에 등록(선택). spdlog::get("system") 으로 찾을 수 있음.
				//    이미 같은 이름이 등록돼 있으면 예외가 나므로 try/catch 로 감싼다.
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

	void log::log_impl(log_category category, log_level level, std::string_view message)
	{
		spdlog::logger* lg = get_logger(category);
		if (lg == nullptr) [[unlikely]]
			return; // 초기화 전 호출 방어

		spdlog::level::level_enum spd_level = to_spdlog_level(static_cast<uint32_t>(level));
		lg->log(spd_level, message);
	}

} // namespace mark
