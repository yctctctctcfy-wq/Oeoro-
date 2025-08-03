
#ifndef OROTO_SHELL_H
#define OROTO_SHELL_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>

// Shell interface declarations
void showPrompt();
void clearScreen();
void displayBanner();
void simulateLoading(int milliseconds);

// Command processing
std::vector<std::string> parseArguments(const std::string& command);
void processCommand(const std::string& command);
void initCommandParser();

// System calls interface
void executeSystemCall(const std::string& syscall, const std::vector<std::string>& args);
void initSystemCalls();
void shutdownSystem();
void rebootSystem();

// Device interface
void executeDeviceCommand(const std::string& device, const std::string& action, const std::vector<std::string>& args);
void initDeviceInterface();

// Security tools interface
void executeNmapScan(const std::vector<std::string>& args);
void executeHashCrack(const std::vector<std::string>& args);
void executePing(const std::vector<std::string>& args);

// Utility functions
inline std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

inline void printSeparator() {
    std::cout << "═══════════════════════════════════════════════════════════════════" << std::endl;
}

inline void printHeader(const std::string& title) {
    std::cout << "╔══════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║" << std::setw(33 + title.length()/2) << title 
              << std::setw(33 - title.length()/2) << "║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════╝" << std::endl;
}

#endif // OROTO_SHELL_H
