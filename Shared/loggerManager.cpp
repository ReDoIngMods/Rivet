#include "loggerManager.h"
#include "flags.h"

// Helper class
enum class ConsoleColor : WORD {
    Black = 0,
    Blue = FOREGROUND_BLUE,
    Green = FOREGROUND_GREEN,
    Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
    Red = FOREGROUND_RED,
    Magenta = FOREGROUND_RED | FOREGROUND_BLUE,
    Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
    White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    Gray = FOREGROUND_INTENSITY,
    BrightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BrightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    BrightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BrightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
    BrightMagenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BrightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    BrightWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};

inline static bool VFormatBuffer(Rivet::LoggerManager::Buffer& buffer, const std::string_view& format, va_list args) {
    va_list args2;
    va_copy(args2, args);
    int size = vsnprintf(nullptr, 0, format.data(), args2);
    va_end(args2);

    if (size < 0)
        return false;

    if (size >= buffer.size) {
        delete[] buffer.buffer;
        buffer.size = size + 1;
        buffer.buffer = new char[buffer.size];
    }

    vsnprintf(buffer.buffer, buffer.size, format.data(), args);
    return true;
}

inline static bool FormatBuffer(Rivet::LoggerManager::Buffer& buffer, const std::string_view format, ...) {
    va_list args;
    va_start(args, format);
    
    bool result = VFormatBuffer(buffer, format, args);

    va_end(args);
    return result;
}

Rivet::LoggerManager::LoggerManager() {
    this->formatBuffer_.size = 0xFF + 1;
    this->formatBuffer_.buffer = new char[this->formatBuffer_.size];

    this->logBuffer_.size = 0xFF + 1;
    this->logBuffer_.buffer = new char[this->logBuffer_.size];

    Rivet::Flags genericFlags = Rivet::Flags::GetFlags();
    Rivet::DoorstopFlags& flags = genericFlags.doorstop;

	if (!flags.hideConsole) {
		AllocConsole();
		SetConsoleTitle(L"Debug Console");
		SetConsoleCP(CP_UTF8);

		this->hStdOut_ = GetStdHandle(STD_OUTPUT_HANDLE);
	}
    
    this->sendRawLog(Rivet::LogLevel::Info, "LoggerManager", "Initialized Console!", nullptr);
}

Rivet::LoggerManager::~LoggerManager() { 
    if (logBuffer_.buffer) {
        delete[] logBuffer_.buffer;
        logBuffer_.buffer = nullptr;
    }

    if (formatBuffer_.buffer) {
        delete[] formatBuffer_.buffer;
        formatBuffer_.buffer = nullptr;
    }
}

Rivet::Logger* Rivet::LoggerManager::getLogger(std::string_view name) {
	const auto& iterator = loggers_.find(name);
	if (iterator == loggers_.end()) {
		Logger* logger = new Logger(name);
		loggers_[name] = logger;

		return logger;
	}

	return iterator->second;
}

void Rivet::LoggerManager::sendRawLog(LogLevel logLevel, std::string_view loggerName, std::string_view format, va_list arguments) {
	const char* levelStr = nullptr;
    ConsoleColor color = ConsoleColor::White;

    switch (logLevel) {
    case LogLevel::Info:
        levelStr = "[ INFO ]: ";
        color = ConsoleColor::BrightGreen;
        break;
    case LogLevel::Warning:
        levelStr = "[ WARN ]: ";
        color = ConsoleColor::BrightYellow;
        break;
    case LogLevel::Error:
        levelStr = "[ ERROR ]: ";
        color = ConsoleColor::BrightRed;
        break;
    case LogLevel::Debug:
        levelStr = "[ DEBUG ]: ";
        color = ConsoleColor::BrightMagenta;
        break;
    }

    time_t epochTime;
    time(&epochTime);
    struct tm curTime;
    localtime_s(&curTime, &epochTime);

    constexpr const char* logFormat = "%02d:%02d:%02d %s %s%s\n";

	std::lock_guard<std::mutex> lock(this->consoleMutex_);

    VFormatBuffer(formatBuffer_, format, arguments);
    FormatBuffer(logBuffer_, logFormat, curTime.tm_hour, curTime.tm_min, curTime.tm_sec, loggerName.data(), levelStr, formatBuffer_.buffer);
    
    SetConsoleTextAttribute(this->hStdOut_, static_cast<WORD>(color));

    DWORD written = 0;
    WriteConsoleA(hStdOut_, logBuffer_.buffer, static_cast<DWORD>(strlen(logBuffer_.buffer)), &written, nullptr);
}
