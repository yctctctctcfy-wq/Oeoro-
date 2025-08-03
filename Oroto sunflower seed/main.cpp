#include "lib/oroto_shell.h"
#include "lib/colors.h"
#include "lib/logger.h"
#include "lib/error_handler.h"
#include "lib/resource_manager.h"
#include "lib/thread_pool.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <signal.h>

extern void initCommandParser();
extern void processCommand(const std::string& command);
extern void initSystemCalls();
extern void initDeviceInterface();
extern void shutdownSystem();
extern void rebootSystem();

class OrotoKernel {
private:
    bool systemRunning;
    std::string kernelVersion;
    std::map<std::string, std::string> systemInfo;
    std::unique_ptr<Oroto::ResourceManager<std::string>> resourceManager;

public:
    OrotoKernel() : systemRunning(false), kernelVersion("3.2.1") {
        initializeKernel();
    }

    ~OrotoKernel() {
        cleanup();
    }

    void initializeKernel() {
        try {
            // Initialize core systems with error checking
            if (!Oroto::Logger::initialize("oroto_kernel.log", Oroto::LogLevel::INFO)) {
                throw std::runtime_error("Failed to initialize logger");
            }
            
            Oroto::ErrorHandler::initialize();
            Oroto::initializeThreadPool(std::thread::hardware_concurrency());
            
            LOG_INFO("Kernel", "Oroto Kernel v" + kernelVersion + " initialization started");
            
            // Initialize system information
            systemInfo["kernel_name"] = "Oroto-Kernel";
            systemInfo["version"] = kernelVersion;
            systemInfo["architecture"] = "ARM64-Mobile";
            systemInfo["build"] = "2024.01.02";
            systemInfo["memory"] = "8GB";
            systemInfo["storage"] = "256GB";
            
            // Initialize resource manager with proper RAII
            resourceManager = std::make_unique<Oroto::ResourceManager<std::string>>();
            
            // Add some system resources
            auto sys_info = std::make_shared<std::string>("System initialized at " + 
                std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()));
            
            if (!resourceManager->addResource("system_info", sys_info)) {
                LOG_WARNING("Kernel", "Failed to add system info resource");
            }
            
            LOG_INFO("Kernel", "Core systems initialized successfully with " + 
                    std::to_string(std::thread::hardware_concurrency()) + " thread pool workers");
            
        } catch (const std::exception& e) {
            std::cerr << "CRITICAL: Failed to initialize kernel: " << e.what() << std::endl;
            std::exit(1);
        }
    }

    void cleanup() {
        if (systemRunning) {
            LOG_INFO("Kernel", "Starting kernel cleanup");
            
            try {
                if (resourceManager) {
                    resourceManager->clearResources();
                }
                
                Oroto::shutdownThreadPool();
                Oroto::Logger::shutdown();
                
            } catch (const std::exception& e) {
                std::cerr << "Error during cleanup: " << e.what() << std::endl;
            }
        }
    }

    void bootSequence() {
        try {
            LOG_INFO("Kernel", "Starting boot sequence");
            
            clearScreen();
            showKernelHeader();

            std::cout << YELLOW << "[KERNEL] Initializing Oroto Kernel v" << kernelVersion << RESET << "\n";
            simulateLoading(800);

            std::cout << GREEN << "[KERNEL] Loading command parser module..." << RESET << "\n";
            if (!Oroto::ErrorHandler::safeExecute([]() { initCommandParser(); }, "CommandParser")) {
                throw std::runtime_error("Failed to initialize command parser");
            }
            simulateLoading(500);

            std::cout << GREEN << "[KERNEL] Loading system calls..." << RESET << "\n";
            if (!Oroto::ErrorHandler::safeExecute([]() { initSystemCalls(); }, "SystemCalls")) {
                throw std::runtime_error("Failed to initialize system calls");
            }
            simulateLoading(600);

            std::cout << GREEN << "[KERNEL] Initializing device interface..." << RESET << "\n";
            if (!Oroto::ErrorHandler::safeExecute([]() { initDeviceInterface(); }, "DeviceInterface")) {
                throw std::runtime_error("Failed to initialize device interface");
            }
            simulateLoading(700);

            std::cout << GREEN << "[KERNEL] Loading security tools..." << RESET << "\n";
            simulateLoading(900);

            std::cout << BOLD << GREEN << "[KERNEL] Oroto kernel ready. All modules loaded." << RESET << "\n\n";
            systemRunning = true;
            
            LOG_INFO("Kernel", "Boot sequence completed successfully");
            
        } catch (const std::exception& e) {
            LOG_CRITICAL("Kernel", "Boot sequence failed: " + std::string(e.what()));
            std::cout << BOLD << RED << "[CRITICAL] Boot sequence failed: " << e.what() << RESET << "\n";
            std::exit(1);
        }
    }

    void showKernelHeader() {
        std::cout << BOLD << CYAN;
        std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║   ██████╗ ██████╗  ██████╗ ████████╗ ██████╗                    ║\n";
        std::cout << "║  ██╔═══██╗██╔══██╗██╔═══██╗╚══██╔══╝██╔═══██╗                   ║\n";
        std::cout << "║  ██║   ██║██████╔╝██║   ██║   ██║   ██║   ██║                   ║\n";
        std::cout << "║  ██║   ██║██╔══██╗██║   ██║   ██║   ██║   ██║                   ║\n";
        std::cout << "║  ╚██████╔╝██║  ██║╚██████╔╝   ██║   ╚██████╔╝                   ║\n";
        std::cout << "║   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝    ╚═╝    ╚═════╝                    ║\n";
        std::cout << "║                                                                  ║\n";
        std::cout << "║        Advanced Kernel-Based Mobile Operating System             ║\n";
        std::cout << "║                     Built for Security & Power                  ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
        std::cout << RESET << "\n";
    }

    void simulateLoading(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void clearScreen() {
        #ifdef _WIN32
            if (system("cls") != 0) {
                // Ignore clear screen failure
            }
        #else
            if (system("clear") != 0) {
                // Ignore clear screen failure
            }
        #endif
    }

    void run() {
        bootSequence();

        std::string command;
        while (systemRunning) {
            showPrompt();
            std::getline(std::cin, command);

            if (!command.empty()) {
                if (command == "oroto poweroff") {
                    shutdownSystem();
                    systemRunning = false;
                } else if (command == "oroto reboot") {
                    rebootSystem();
                    bootSequence();
                } else {
                    processCommand(command);
                }
            }
        }
    }

    void showPrompt() {
        std::cout << BLUE << "oroto-kernel@mobile:~# " << RESET;
    }
};

// Global kernel instance for signal handling
std::unique_ptr<OrotoKernel> g_kernel;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\n" << YELLOW << "[SIGNAL] Received signal " << signal << ", shutting down..." << RESET << "\n";
    
    if (g_kernel) {
        g_kernel.reset(); // This will call destructor and cleanup
    }
    
    std::exit(0);
}

int main() {
    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        srand(time(nullptr));

        std::cout << BOLD << GREEN << "[BOOT] Starting Oroto Mobile Operating System..." << RESET << "\n\n";

        g_kernel = std::make_unique<OrotoKernel>();
        g_kernel->run();

        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << BOLD << RED << "[FATAL] Unhandled exception: " << e.what() << RESET << "\n";
        return 1;
    } catch (...) {
        std::cerr << BOLD << RED << "[FATAL] Unknown exception occurred" << RESET << "\n";
        return 2;
    }
}