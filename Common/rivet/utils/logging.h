#pragma once

#include "includes.h"

#include <cstdio>
#include <cstdarg>
#include <string_view>
#include <mutex>
#include <windows.h>

namespace Rivet {
enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

class Logger {
public:
	Logger(std::string_view logPath, bool useConsole = true) {
		useConsole_ = useConsole;

        // Open file
        logFile_ = fopen(logPath.data(), "a");

        // Allocate console
        if (useConsole_) {
            AllocConsole();
            freopen_s(&consoleOut_, "CONOUT$", "w", stdout);
            freopen_s(&consoleErr_, "CONOUT$", "w", stderr);
        }

        hConsole_ = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    ~Logger() {
        if (logFile_) {
            fclose(logFile_);
            logFile_ = nullptr;
        }

        if (consoleOut_) {
            fclose(consoleOut_);
            consoleOut_ = nullptr;
        }

        if (consoleErr_) {
            fclose(consoleErr_);
            consoleErr_ = nullptr;
        }
    }

    static void LogVarArgs(LogLevel level, const char* format, va_list args) {
        std::lock_guard<std::mutex> lock(mutex_);
        const char* levelStr = nullptr;
		WORD color = 7; // Green
        switch (level) {
        case LogLevel::Info:
            levelStr = "INFO";
			color = 10; // Green
            break;
        case LogLevel::Warning:
            levelStr = "WARN";
            color = 14; // Yellow
            break;
        case LogLevel::Error:
            levelStr = "ERROR";
            color = 12; // Red
            break;
        case LogLevel::Debug:
            levelStr = "DEBUG";
            color = 9; // Blue
            break;
        }
        // Format message
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        // Write to file
        if (logFile_) {
            fprintf(logFile_, "[%s] %s\n", levelStr, buffer);
            fflush(logFile_);
        }
        // Write to console
		if (hConsole_) {
            SetConsoleTextAttribute(hConsole_, color);
            fprintf(stdout, "[%s] %s\n", levelStr, buffer);
            SetConsoleTextAttribute(hConsole_, 7); // Reset
        }
	}

    static void Info(const char* format, ...) {
        va_list args;
        va_start(args, format);
        LogVarArgs(LogLevel::Info, format, args);
        va_end(args);
	}

    static void Warning(const char* format, ...) {
        va_list args;
        va_start(args, format);
        LogVarArgs(LogLevel::Warning, format, args);
        va_end(args);
    }

    static void Error(const char* format, ...) {
        va_list args;
        va_start(args, format);
        LogVarArgs(LogLevel::Error, format, args);
        va_end(args);
	}

    static void Debug(const char* format, ...) {
        va_list args;
        va_start(args, format);
        LogVarArgs(LogLevel::Debug, format, args);
        va_end(args);
	}

    static void Log(LogLevel level, const char* format, ...) {
        va_list args;
        va_start(args, format);
        LogVarArgs(level, format, args);
        va_end(args);
	}

private:
    static inline FILE* logFile_ = nullptr;
    static inline FILE* consoleOut_ = nullptr;
    static inline FILE* consoleErr_ = nullptr;
    static inline HANDLE hConsole_ = nullptr;
    static inline std::mutex mutex_;
	static inline bool useConsole_ = true;
};

} // namespace Rivet
