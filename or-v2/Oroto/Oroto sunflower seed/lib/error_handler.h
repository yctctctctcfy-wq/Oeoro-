
#ifndef OROTO_ERROR_HANDLER_H
#define OROTO_ERROR_HANDLER_H

#include <stdexcept>
#include <string>
#include <map>
#include <functional>
#include "logger.h"

namespace Oroto {

// Error codes for Oroto system
enum class ErrorCode {
    SUCCESS = 0,
    COMMAND_NOT_FOUND = 1,
    INVALID_ARGUMENTS = 2,
    PERMISSION_DENIED = 3,
    NETWORK_ERROR = 4,
    FILE_NOT_FOUND = 5,
    MEMORY_ERROR = 6,
    DEVICE_ERROR = 7,
    TIMEOUT = 8,
    INTERNAL_ERROR = 9,
    PARSE_ERROR = 10
};

// Custom exception class for Oroto
class OrotoException : public std::runtime_error {
private:
    ErrorCode code_;
    std::string module_;

public:
    OrotoException(ErrorCode code, const std::string& module, const std::string& message)
        : std::runtime_error(message), code_(code), module_(module) {
        LOG_ERROR(module_, what());
    }

    ErrorCode getCode() const { return code_; }
    const std::string& getModule() const { return module_; }
};

class ErrorHandler {
private:
    static std::map<ErrorCode, std::string> errorMessages;
    static std::map<ErrorCode, std::function<void()>> errorHandlers;

public:
    static void initialize() {
        // Initialize error messages
        errorMessages[ErrorCode::SUCCESS] = "Operation completed successfully";
        errorMessages[ErrorCode::COMMAND_NOT_FOUND] = "Command not found";
        errorMessages[ErrorCode::INVALID_ARGUMENTS] = "Invalid arguments provided";
        errorMessages[ErrorCode::PERMISSION_DENIED] = "Permission denied";
        errorMessages[ErrorCode::NETWORK_ERROR] = "Network operation failed";
        errorMessages[ErrorCode::FILE_NOT_FOUND] = "File or directory not found";
        errorMessages[ErrorCode::MEMORY_ERROR] = "Memory allocation failed";
        errorMessages[ErrorCode::DEVICE_ERROR] = "Device operation failed";
        errorMessages[ErrorCode::TIMEOUT] = "Operation timed out";
        errorMessages[ErrorCode::INTERNAL_ERROR] = "Internal system error";
        errorMessages[ErrorCode::PARSE_ERROR] = "Parse or syntax error";

        // Initialize default error handlers
        errorHandlers[ErrorCode::MEMORY_ERROR] = []() {
            LOG_CRITICAL("ErrorHandler", "Critical memory error detected");
        };
        
        errorHandlers[ErrorCode::INTERNAL_ERROR] = []() {
            LOG_CRITICAL("ErrorHandler", "Internal system error occurred");
        };
    }

    static std::string getErrorMessage(ErrorCode code) {
        auto it = errorMessages.find(code);
        return (it != errorMessages.end()) ? it->second : "Unknown error";
    }

    static void handleError(ErrorCode code, const std::string& module = "System") {
        std::string message = getErrorMessage(code);
        LOG_ERROR(module, "Error " + std::to_string(static_cast<int>(code)) + ": " + message);
        
        // Execute custom handler if available
        auto handler = errorHandlers.find(code);
        if (handler != errorHandlers.end()) {
            handler->second();
        }
    }

    static void registerErrorHandler(ErrorCode code, std::function<void()> handler) {
        errorHandlers[code] = handler;
    }

    // Safe wrapper for operations that might fail
    template<typename T>
    static bool safeExecute(std::function<T()> operation, T& result, 
                           const std::string& module = "System") {
        try {
            result = operation();
            return true;
        } catch (const OrotoException& e) {
            handleError(e.getCode(), e.getModule());
            return false;
        } catch (const std::exception& e) {
            LOG_ERROR(module, std::string("Standard exception: ") + e.what());
            handleError(ErrorCode::INTERNAL_ERROR, module);
            return false;
        } catch (...) {
            LOG_ERROR(module, "Unknown exception occurred");
            handleError(ErrorCode::INTERNAL_ERROR, module);
            return false;
        }
    }

    // Specialization for void functions
    static bool safeExecute(std::function<void()> operation, 
                           const std::string& module = "System") {
        try {
            operation();
            return true;
        } catch (const OrotoException& e) {
            handleError(e.getCode(), e.getModule());
            return false;
        } catch (const std::exception& e) {
            LOG_ERROR(module, std::string("Standard exception: ") + e.what());
            handleError(ErrorCode::INTERNAL_ERROR, module);
            return false;
        } catch (...) {
            LOG_ERROR(module, "Unknown exception occurred");
            handleError(ErrorCode::INTERNAL_ERROR, module);
            return false;
        }
    }
};

// Static member declarations (definitions are in error_handler.cpp)

// Utility macros
#define OROTO_THROW(code, module, msg) throw OrotoException(code, module, msg)
#define OROTO_HANDLE_ERROR(code, module) ErrorHandler::handleError(code, module)

} // namespace Oroto

#endif // OROTO_ERROR_HANDLER_H
