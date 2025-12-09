#pragma once
#include "includes.h"
#include "logger.h"
#include <mutex>

namespace Rivet {
	class LoggerManager {
		LoggerManager() = delete;
		LoggerManager(const LoggerManager&) = delete;
		LoggerManager& operator=(const LoggerManager&) = delete;
	public:
		__declspec(dllimport) static LoggerManager& getInstance();
		__declspec(dllimport) Rivet::Logger* getLogger(const std::string_view& name);
	};
}