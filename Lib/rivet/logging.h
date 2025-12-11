#pragma once
#include "includes.h"
#include "logger.h"
#include "sharedmacros.h"
#include <mutex>

namespace Rivet {
	class LoggerManager {
		LoggerManager() = delete;
		LoggerManager(const LoggerManager&) = delete;
		LoggerManager& operator=(const LoggerManager&) = delete;
	public:
		RIVET_LIB_API static LoggerManager& GetInstance();
		RIVET_LIB_API Rivet::Logger* getLogger(const std::string_view& name);
	};
}