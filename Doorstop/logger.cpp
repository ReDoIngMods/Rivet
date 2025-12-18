#include "logger.h"
#include "loggerManager.h"

void Rivet::Logger::_log(Rivet::LogLevel logLevel, const std::string_view& format, va_list arguments) {
	Rivet::LoggerManager& loggerMgr = Rivet::LoggerManager::getInstance();
	loggerMgr.sendRawLog(logLevel, name_, format, arguments);
}

Rivet::Logger::Logger(const std::string_view& name) : name_(name) {}

void Rivet::Logger::info(const std::string_view format, ...) {
	va_list args;
	va_start(args, format);
	_log(LogLevel::Info, format, args);
	va_end(args);
}

void Rivet::Logger::warn(const std::string_view format, ...) {
	va_list args;
	va_start(args, format);
	_log(LogLevel::Warning, format, args);
	va_end(args);
}

void Rivet::Logger::error(const std::string_view format, ...) {
	va_list args;
	va_start(args, format);
	_log(LogLevel::Error, format, args);
	va_end(args);
}

void Rivet::Logger::debug(const std::string_view format, ...) {
	va_list args;
	va_start(args, format);
	_log(LogLevel::Debug, format, args);
	va_end(args);
}
