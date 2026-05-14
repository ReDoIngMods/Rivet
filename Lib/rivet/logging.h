#pragma once
#include "defines.h"

#include <Windows.h>
#include <cstdarg>
#include <mutex>
#include <string_view>
#include <unordered_map>

namespace Rivet {
enum class LogLevel {
	Info,
	Warning,
	Error,
	Debug
};

class Logger {
	std::string_view name_;

	void Log(LogLevel logLevel, const std::string_view& format, va_list arguments) const;

public:
	explicit Logger(const std::string_view& name);

	RIVET_DOORSTOP_API void Info(std::string_view format, ...) const;
	RIVET_DOORSTOP_API void Warn(std::string_view format, ...) const;
	RIVET_DOORSTOP_API void Error(std::string_view format, ...) const;
	RIVET_DOORSTOP_API void Debug(std::string_view format, ...) const;
};

class LoggerManager {
public:
	LoggerManager();
	~LoggerManager();

	RIVET_DOORSTOP_API static LoggerManager& GetInstance() {
		static LoggerManager instance;
		return instance;
	}

	RIVET_DOORSTOP_API Logger* GetLogger(std::string_view name);

	void SendRawLog(LogLevel logLevel, std::string_view loggerName, std::string_view format, va_list arguments);

	struct Buffer {
		char* buffer = nullptr;
		int size = 0;
	};

private:
	LoggerManager(const LoggerManager&) = delete;

	HANDLE hStdOut_ = INVALID_HANDLE_VALUE;
	HANDLE hFileOut_ = INVALID_HANDLE_VALUE;

	std::mutex consoleMutex_;

	Buffer formatBuffer_;
	Buffer logBuffer_;

	std::unordered_map<std::string_view, Logger*> loggers_;
};
} // namespace Rivet
