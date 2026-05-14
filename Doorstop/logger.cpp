#include <rivet/logging.h>

using namespace Rivet;

void Logger::Log(const LogLevel logLevel, const std::string_view& format, const va_list arguments) const {
	LoggerManager& loggerMgr = LoggerManager::GetInstance();
	loggerMgr.SendRawLog(logLevel, name_, format, arguments);
}

Logger::Logger(const std::string_view& name) : name_(name) {}

void Logger::Info(const std::string_view format, ...) const {
	va_list args;
	va_start(args, format);
	Log(LogLevel::Info, format, args);
	va_end(args);
}

void Logger::Warn(const std::string_view format, ...) const {
	va_list args;
	va_start(args, format);
	Log(LogLevel::Warning, format, args);
	va_end(args);
}

void Logger::Error(const std::string_view format, ...) const {
	va_list args;
	va_start(args, format);
	Log(LogLevel::Error, format, args);
	va_end(args);
}

void Logger::Debug(const std::string_view format, ...) const {
	va_list args;
	va_start(args, format);
	Log(LogLevel::Debug, format, args);
	va_end(args);
}
