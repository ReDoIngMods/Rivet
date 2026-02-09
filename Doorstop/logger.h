#pragma once
#include "includes.h"

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

		void Log(LogLevel logLevel, const std::string_view& format, va_list arguments) const;
	public:
		Logger(const std::string_view& name);

		RIVET_DOORSTOP_API void Info(std::string_view format, ...) const;
		RIVET_DOORSTOP_API void Warn(std::string_view format, ...) const;
		RIVET_DOORSTOP_API void Error(std::string_view format, ...) const;
		RIVET_DOORSTOP_API void Debug(std::string_view format, ...) const;
	};
}