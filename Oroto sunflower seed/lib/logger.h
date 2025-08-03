
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

    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRIT";
            default: return "UNKNOWN";
        }
    }

public:
    static bool initialize(const std::string& logFileName = "oroto_kernel.log", 
                          LogLevel level = LogLevel::INFO) {
        std::lock_guard<std::mutex> lock(logMutex);
        if (!initialized) {
            logFile.open(logFileName, std::ios::app);
            if (!logFile.is_open()) {
                return false;
            }
            currentLevel = level;
            initialized = true;
            log(LogLevel::INFO, "Logger", "Oroto Logger initialized");
        }
        return true;
    }

    static void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(logMutex);
        currentLevel = level;
    }

    static void log(LogLevel level, const std::string& module, const std::string& message) {
        if (!initialized || level < currentLevel) return;

        std::lock_guard<std::mutex> lock(logMutex);
        
        std::string logEntry = "[" + getCurrentTime() + "] [" + 
                              levelToString(level) + "] [" + module + "] " + message;
        
        // Write to file
        if (logFile.is_open()) {
            logFile << logEntry << std::endl;
            logFile.flush();
        }
        
        // Write to console for errors and critical
        if (level >= LogLevel::ERROR) {
            std::cerr << logEntry << std::endl;
        }
    }

    static void debug(const std::string& module, const std::string& message) {
        log(LogLevel::DEBUG, module, message);
    }

    static void info(const std::string& module, const std::string& message) {
        log(LogLevel::INFO, module, message);
    }

    static void warning(const std::string& module, const std::string& message) {
        log(LogLevel::WARNING, module, message);
    }

    static void error(const std::string& module, const std::string& message) {
        log(LogLevel::ERROR, module, message);
    }

    static void critical(const std::string& module, const std::string& message) {
        log(LogLevel::CRITICAL, module, message);
    }

    static void shutdown() {
        std::lock_guard<std::mutex> lock(logMutex);
        if (initialized && logFile.is_open()) {
            log(LogLevel::INFO, "Logger", "Oroto Logger shutting down");
            logFile.close();
            initialized = false;
        }
    }
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
