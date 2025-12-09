#pragma once
#include "includes.h"
#include "logger.h"
#include <mutex>

#if DOORSTOP_EXPORTS
#define LOGGERMANAGER_API __declspec(dllexport)
#else
#define LOGGERMANAGER_API __declspec(dllimport)
#endif

namespace Rivet {
	class LoggerManager {
		LoggerManager(const LoggerManager&) = delete;
		LoggerManager& operator=(const LoggerManager&) = delete;

		HANDLE hStdOut_ = INVALID_HANDLE_VALUE;
		std::mutex consoleMutex_;
	public:
		struct Buffer {
			char* buffer = nullptr;
			int size = 0;
		};
	private:
		Buffer formatBuffer_;
		Buffer logBuffer_;

		std::unordered_map<std::string_view, Logger*> loggers_;
	public:
		LoggerManager();
		~LoggerManager();

		LOGGERMANAGER_API static LoggerManager& getInstance() {
			static LoggerManager instance;
			return instance;
		}

		LOGGERMANAGER_API Rivet::Logger* getLogger(const std::string_view& name);

		void sendRawLog(LogLevel logLevel, const std::string_view& loggerName, const std::string_view& format, va_list arguments);
	};
}