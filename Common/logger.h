#pragma once
#include "includes.h"
#include <cstdarg>

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

		virtual void info(const std::string_view format, ...);
		virtual void warn(const std::string_view format, ...);
		virtual void error(const std::string_view format, ...);
		virtual void debug(const std::string_view format, ...);
	};
}