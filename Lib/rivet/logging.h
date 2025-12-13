#pragma once
#include "includes.h"
#include "sharedmacros.h"
#include <mutex>

namespace Rivet {
	enum class LogLevel {
		Info,
		Warning,
		Error,
		Debug
	};

	class Logger {
		std::string_view name_;

		void _log(Rivet::LogLevel logLevel, const std::string_view& format, va_list arguments);
	public:
		Logger(const std::string_view& name);

		RIVET_DOORSTOP_API void info(const std::string_view format, ...);
		RIVET_DOORSTOP_API void warn(const std::string_view format, ...);
		RIVET_DOORSTOP_API void error(const std::string_view format, ...);
		RIVET_DOORSTOP_API void debug(const std::string_view format, ...);
	};

	class LoggerManager {
		LoggerManager() = delete;
		LoggerManager(const LoggerManager&) = delete;
		LoggerManager& operator=(const LoggerManager&) = delete;
	public:
		RIVET_DOORSTOP_API static LoggerManager& GetInstance();
		RIVET_DOORSTOP_API Rivet::Logger* getLogger(const std::string_view& name);
	};
}