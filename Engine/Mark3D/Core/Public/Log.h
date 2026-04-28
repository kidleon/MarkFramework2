#pragma once
#include <source_location>
#include "CoreGeneric.h"
#include "StringBuffer.h"


namespace mark
{
	class MARKENGINE_API log
	{
	public:
		enum class category : uint32_t
		{
			system = 0,
			gameplay,
		};

		enum class level : uint32_t
		{
			info = 0,
			warning,
			error,
			critical,

			max_lv
		};

		enum class target : uint32_t
		{
			console = 0x0001,
			file = 0x0002,
			all = console | file
		};

	public:
		static void init(uint32_t level, uint32_t target, bool multi_thread);
		static void shutdown();

		template <typename... Args>
		static void dispatch(
			std::source_location loc,
			category cat,
			level lv,
			std::format_string<Args...> fmt,   // 컴파일 타임 검증되는 포맷
			Args&&... args
		)
		{
			// make_format_args로 가변 인자를 타입 소거 컨테이너로 변환
			// 이 시점에 타입 검증/포인터 캡처가 끝남
			dispatch_impl(loc, cat, lv, fmt.get(), std::make_format_args(args...));
		}

		static void dispatch_impl(
			std::source_location loc,
			category cat,
			level lv,
			std::string_view fmt,
			std::format_args args
		);
	};
}


#if defined(__LOG_ENABLED__)
#define SYS_LOG_F(...)				mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::info, __VA_ARGS__)
#define SYS_LOG_WRN_F(...)			mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::warning, __VA_ARGS__)
#define SYS_LOG_ERR_F(...)			mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::error, __VA_ARGS__)
#define SYS_LOG_CRIT_F(...)			mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::critical, __VA_ARGS__)

#define LOG_F(...)					mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::info, __VA_ARGS__)
#define LOG_WRN_F(...)				mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::warning, __VA_ARGS__)
#define LOG_ERR_F(...)				mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::error, __VA_ARGS__)
#define LOG_CRIT_F(...)				mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::critical, __VA_ARGS__)

#define SYS_LOG(msg)				mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::info, msg)
#define SYS_LOG_WRN(msg)			mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::warning, msg)
#define SYS_LOG_ERR(msg)			mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::error, msg)
#define SYS_LOG_CRIT(msg)			mark::log::dispatch(std::source_location::current(), mark::log::category::system, mark::log::level::critical, msg)

#define LOG(msg)					mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::info, msg)
#define LOG_WRN(msg)				mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::warning, msg)
#define LOG_ERR(msg)				mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::error, msg)
#define LOG_CRIT(msg)				mark::log::dispatch(std::source_location::current(), mark::log::category::gameplay, mark::log::level::critical, msg)

#else
#define SYS_LOG_F(...)				0
#define SYS_LOG_WRN_F(...)			0
#define SYS_LOG_ERR_F(...)			0
#define SYS_LOG_CRIT_F(...)			0

#define LOG_F(...)					0
#define LOG_WRN_F(...)				0
#define LOG_ERR_F(...)				0
#define LOG_CRIT_F(...)				0

#define SYS_LOG(...)				0
#define SYS_LOG_WRN(...)			0
#define SYS_LOG_ERR(...)			0
#define SYS_LOG_CRIT(...)			0

#define LOG(...)					0
#define LOG_WRN(...)				0
#define LOG_ERR(...)				0
#define LOG_CRIT(...)				0
#endif // __LOG_ENABLED__
