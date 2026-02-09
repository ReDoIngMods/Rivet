#pragma once
#include "includes.h"
#include "defines.h"
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

		static void Log(LogLevel logLevel, const std::string_view& format, va_list arguments) {}

	public:
		explicit Logger(const std::string_view& name);

		RIVET_DOORSTOP_API void Info(std::string_view format, ...);
		RIVET_DOORSTOP_API void Warn(std::string_view format, ...);
		RIVET_DOORSTOP_API void Error(std::string_view format, ...);
		RIVET_DOORSTOP_API void Debug(std::string_view format, ...);
	};

	class LoggerManager {
		LoggerManager() = delete;
		LoggerManager(const LoggerManager&) = delete;
		LoggerManager& operator=(const LoggerManager&) = delete;
		
	public:
		RIVET_DOORSTOP_API static LoggerManager& GetInstance();
		RIVET_DOORSTOP_API Logger* GetLogger(std::string_view name);
	};
}