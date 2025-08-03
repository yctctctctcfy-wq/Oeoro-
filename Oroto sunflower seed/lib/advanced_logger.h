
#ifndef OROTO_ADVANCED_LOGGER_H
#define OROTO_ADVANCED_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <memory>
#include <string>

namespace Oroto {

class AdvancedLogger {
private:
    static std::shared_ptr<spdlog::logger> console_logger_;
    static std::shared_ptr<spdlog::logger> file_logger_;
    static std::shared_ptr<spdlog::logger> async_logger_;
    static bool initialized_;

public:
    static bool initialize(const std::string& log_file = "oroto_advanced.log", 
                          size_t max_file_size = 1024 * 1024 * 10, // 10MB
                          size_t max_files = 3) {
        try {
            if (initialized_) {
                return true;
            }

            // Create console logger with colors
            console_logger_ = spdlog::stdout_color_mt("console");
            console_logger_->set_level(spdlog::level::info);
            console_logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");

            // Create rotating file logger
            file_logger_ = spdlog::rotating_logger_mt("file", log_file, max_file_size, max_files);
            file_logger_->set_level(spdlog::level::debug);
            file_logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] [%t] %v");

            // Create async logger for high-performance logging
            spdlog::init_thread_pool(8192, 1);
            auto async_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "async_" + log_file, max_file_size, max_files);
            async_logger_ = std::make_shared<spdlog::async_logger>(
                "async", async_sink, spdlog::thread_pool());
            async_logger_->set_level(spdlog::level::trace);
            
            spdlog::register_logger(async_logger_);

            initialized_ = true;
            info("AdvancedLogger", "Advanced logging system initialized");
            return true;

        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize advanced logger: " << e.what() << std::endl;
            return false;
        }
    }

    static void shutdown() {
        if (initialized_) {
            spdlog::shutdown();
            initialized_ = false;
        }
    }

    // Logging methods with automatic error checking
    template<typename... Args>
    static void trace(const std::string& module, const std::string& format, Args&&... args) {
        if (file_logger_) {
            file_logger_->trace("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        }
        if (async_logger_) {
            async_logger_->trace("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    static void debug(const std::string& module, const std::string& format, Args&&... args) {
        if (file_logger_) {
            file_logger_->debug("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        }
        if (async_logger_) {
            async_logger_->debug("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    static void info(const std::string& module, const std::string& format, Args&&... args) {
        auto msg = fmt::format("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        if (console_logger_) console_logger_->info(msg);
        if (file_logger_) file_logger_->info(msg);
        if (async_logger_) async_logger_->info(msg);
    }

    template<typename... Args>
    static void warn(const std::string& module, const std::string& format, Args&&... args) {
        auto msg = fmt::format("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        if (console_logger_) console_logger_->warn(msg);
        if (file_logger_) file_logger_->warn(msg);
        if (async_logger_) async_logger_->warn(msg);
    }

    template<typename... Args>
    static void error(const std::string& module, const std::string& format, Args&&... args) {
        auto msg = fmt::format("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        if (console_logger_) console_logger_->error(msg);
        if (file_logger_) file_logger_->error(msg);
        if (async_logger_) async_logger_->error(msg);
    }

    template<typename... Args>
    static void critical(const std::string& module, const std::string& format, Args&&... args) {
        auto msg = fmt::format("[{}] {}", module, fmt::format(format, std::forward<Args>(args)...));
        if (console_logger_) console_logger_->critical(msg);
        if (file_logger_) file_logger_->critical(msg);
        if (async_logger_) async_logger_->critical(msg);
    }

    // Helper for file operation error checking
    static bool checkFileOperation(bool success, const std::string& operation, 
                                 const std::string& filename) {
        if (!success) {
            error("FileOps", "Failed to {} file: {}", operation, filename);
            return false;
        }
        debug("FileOps", "Successfully {} file: {}", operation, filename);
        return true;
    }

    // Helper for network operation error checking
    static bool checkNetworkOperation(int result, const std::string& operation) {
        if (result < 0) {
            error("NetworkOps", "Failed to {}: error code {}", operation, result);
            return false;
        }
        debug("NetworkOps", "Successfully {}: result {}", operation, result);
        return true;
    }
};

// Static member definitions
std::shared_ptr<spdlog::logger> AdvancedLogger::console_logger_;
std::shared_ptr<spdlog::logger> AdvancedLogger::file_logger_;
std::shared_ptr<spdlog::logger> AdvancedLogger::async_logger_;
bool AdvancedLogger::initialized_ = false;

// Convenience macros for advanced logging
#define ALOG_TRACE(module, ...) Oroto::AdvancedLogger::trace(module, __VA_ARGS__)
#define ALOG_DEBUG(module, ...) Oroto::AdvancedLogger::debug(module, __VA_ARGS__)
#define ALOG_INFO(module, ...) Oroto::AdvancedLogger::info(module, __VA_ARGS__)
#define ALOG_WARN(module, ...) Oroto::AdvancedLogger::warn(module, __VA_ARGS__)
#define ALOG_ERROR(module, ...) Oroto::AdvancedLogger::error(module, __VA_ARGS__)
#define ALOG_CRITICAL(module, ...) Oroto::AdvancedLogger::critical(module, __VA_ARGS__)

} // namespace Oroto

#endif // OROTO_ADVANCED_LOGGER_H
