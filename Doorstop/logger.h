#pragma once
#include "includes.h"
#include <cstdarg>

#include "sharedmacros.h"

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
}