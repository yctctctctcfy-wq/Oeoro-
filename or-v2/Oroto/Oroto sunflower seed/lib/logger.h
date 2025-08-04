#ifndef OROTO_LOGGER_H
#define OROTO_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace Oroto {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

class Logger {
private:
    static std::mutex logMutex;
    static LogLevel currentLevel;
    static std::ofstream logFile;
    static bool initialized;

#ifndef USE_SPDLOG
    static std::string getCurrentTime();
    static std::string levelToString(LogLevel level);
    static void log(LogLevel level, const std::string& module, const std::string& message);
#endif

public:
    static bool initialize(const std::string& logFileName = "oroto_kernel.log", 
                          LogLevel level = LogLevel::INFO);

    static void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(logMutex);
        currentLevel = level;
    }

    static void debug(const std::string& module, const std::string& message);
    static void info(const std::string& module, const std::string& message);
    static void warning(const std::string& module, const std::string& message);
    static void error(const std::string& module, const std::string& message);
    static void critical(const std::string& module, const std::string& message);

    static void shutdown();
};

// Static member declarations (definitions are in logger.cpp)

// Convenience macros
#define LOG_DEBUG(module, msg) Oroto::Logger::debug(module, msg)
#define LOG_INFO(module, msg) Oroto::Logger::info(module, msg)
#define LOG_WARNING(module, msg) Oroto::Logger::warning(module, msg)
#define LOG_ERROR(module, msg) Oroto::Logger::error(module, msg)
#define LOG_CRITICAL(module, msg) Oroto::Logger::critical(module, msg)

} // namespace Oroto

#endif // OROTO_LOGGER_H