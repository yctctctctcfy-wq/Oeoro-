
#include "logger.h"

namespace Oroto {

// Static member definitions
std::mutex Logger::logMutex;
LogLevel Logger::currentLevel = LogLevel::INFO;
std::ofstream Logger::logFile;
bool Logger::initialized = false;

#ifndef USE_SPDLOG
bool Logger::initialize(const std::string& filename, LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (!initialized) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            return false;
        }
        currentLevel = level;
        initialized = true;
        log(LogLevel::INFO, "Logger", "Oroto Logger initialized");
    }
    return true;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
    }
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (initialized && logFile.is_open()) {
        log(LogLevel::INFO, "Logger", "Oroto Logger shutting down");
        logFile.close();
        initialized = false;
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::log(LogLevel level, const std::string& component, const std::string& message) {
    if (!initialized || level < currentLevel) return;

    std::lock_guard<std::mutex> lock(logMutex);

    std::string logEntry = "[" + getCurrentTime() + "] [" + 
                          levelToString(level) + "] [" + component + "] " + message;

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

void Logger::debug(const std::string& component, const std::string& message) {
    log(LogLevel::DEBUG, component, message);
}

void Logger::info(const std::string& component, const std::string& message) {
    log(LogLevel::INFO, component, message);
}

void Logger::warning(const std::string& component, const std::string& message) {
    log(LogLevel::WARNING, component, message);
}

void Logger::error(const std::string& component, const std::string& message) {
    log(LogLevel::ERROR, component, message);
}

void Logger::critical(const std::string& component, const std::string& message) {
    log(LogLevel::CRITICAL, component, message);
}
#endif

} // namespace Oroto
