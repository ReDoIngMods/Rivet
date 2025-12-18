#pragma once
#include "includes.h"
#include "logger.h"
#include <mutex>

#include "sharedmacros.h"

namespace Rivet {
	class LoggerManager {
	public:
		LoggerManager();
		~LoggerManager();

		RIVET_DOORSTOP_API static Rivet::LoggerManager& getInstance() {
			static LoggerManager instance;
			return instance;
		}

		RIVET_DOORSTOP_API Rivet::Logger* getLogger(std::string_view name);

		void sendRawLog(LogLevel logLevel, std::string_view loggerName, std::string_view format, va_list arguments);

		struct Buffer {
			char* buffer = nullptr;
			int size = 0;
		};

	private:
		LoggerManager(const LoggerManager&) = delete;
		LoggerManager& operator=(const LoggerManager&) = delete;

		HANDLE hStdOut_ = INVALID_HANDLE_VALUE;
		HANDLE hFileOut_ = INVALID_HANDLE_VALUE;

		std::mutex consoleMutex_;

		Buffer formatBuffer_;
		Buffer logBuffer_;

		std::unordered_map<std::string_view, Logger*> loggers_;

	public:
	};
}