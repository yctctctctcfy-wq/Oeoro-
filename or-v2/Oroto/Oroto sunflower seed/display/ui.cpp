#include "../lib/oroto_shell.h"
#include "../lib/colors.h"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <atomic>   // <─ eklendi

namespace UI {

// ─────────────────────────────────────────────────────────────
// Global durum bayrakları artık atomik: yarış ve cache sorunu yok
// ─────────────────────────────────────────────────────────────
static std::atomic<bool> _initialized{false};
static std::atomic<bool> _headless{false};

// ─────────────────────────────────────────────────────────────
bool initScreen(bool headless = false)
// ─────────────────────────────────────────────────────────────
{
    std::cerr << "[DEBUG] UI::initScreen() entered with headless=" << headless << "\n";

    const bool forceHeadless =
          headless
       || getenv("CI")             != nullptr
       || getenv("OROTO_HEADLESS") != nullptr
       || !isatty(STDIN_FILENO);

    _headless.store(forceHeadless, std::memory_order_release);

    if (forceHeadless) {
        std::cout << YELLOW
                  << "[UI] Running in headless mode (TTY unavailable or forced)"
                  << RESET << "\n";
        _initialized.store(true, std::memory_order_release);
        std::cerr << "[DEBUG] UI initialized in headless mode\n";
        return false;
    }

    // ‼ Buraya TTY/NCURSES benzeri gerçek ekran kurulumu gelecek
    _initialized.store(true, std::memory_order_release);
    std::cerr << "[DEBUG] UI initialized in normal mode\n";
    return true;
}

// ─────────────────────────────────────────────────────────────
void start(bool headless = false)
// ─────────────────────────────────────────────────────────────
{
    std::cerr << "[DEBUG] UI::start() called with headless=" << headless << "\n";
    initScreen(headless);
    std::cerr << "[DEBUG] UI::start() completed\n";
}

bool ready()      { return _initialized.load(std::memory_order_acquire); }
bool isHeadless() { return _headless.load(std::memory_order_acquire);   }

} // namespace UI



// ─────────────────────────────────────────────────────────────
// Aşağıdan itibaren görsel/CLI yardımcı fonksiyonları (değişmedi)
// ─────────────────────────────────────────────────────────────
void displayWelcomeScreen() {
    std::cout << BOLD << BRIGHT_CYAN;
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════╗
    ║                                                                        ║
    ║    ██████╗ ██████╗  ██████╗ ████████╗ ██████╗                         ║
    ║   ██╔═══██╗██╔══██╗██╔═══██╗╚══██╔══╝██╔═══██╗                        ║
    ║   ██║   ██║██████╔╝██║   ██║   ██║   ██║   ██║                        ║
    ║   ██║   ██║██╔══██╗██║   ██║   ██║   ██║   ██║                        ║
    ║   ╚██████╔╝██║  ██║╚██████╔╝   ██║   ╚██████╔╝                        ║
    ║    ╚═════╝ ╚═╝  ╚═╝ ╚═════╝    ╚═╝    ╚═════╝                         ║
    ║                                                                        ║
    ║              ADVANCED KERNEL-BASED MOBILE OPERATING SYSTEM            ║
    ║                        Professional Security Edition                   ║
    ║                                                                        ║
    ║  ▶ Real-time kernel with hardware abstraction layer                   ║
    ║  ▶ Advanced security tools and penetration testing suite             ║
    ║  ▶ Complete device control and system management                      ║
    ║  ▶ Network analysis and monitoring capabilities                       ║
    ║                                                                        ║
    ╚════════════════════════════════════════════════════════════════════════╝
)" << RESET << "\n";
}

void displaySystemStatusBar() {
    std::cout << BOLD << BLACK << BG_WHITE << " OROTO v3.2.1 " << RESET;
    std::cout << BLACK << BG_GREEN << " KERNEL ACTIVE " << RESET;
    std::cout << BLACK << BG_YELLOW << " SECURE MODE " << RESET;
    std::cout << BLACK << BG_BLUE << " TOOLS LOADED " << RESET << "\n";
}

void displayLoadingAnimation(const std::string& message, int duration_ms) {
    const std::string spinner = "|/-\\";
    auto start = std::chrono::steady_clock::now();
    auto end   = start + std::chrono::milliseconds(duration_ms);

    size_t i = 0;
    while (std::chrono::steady_clock::now() < end) {
        std::cout << "\r" << YELLOW << "[" << spinner[i % 4] << "] " << message << RESET;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++i;
    }
    std::cout << "\r" << GREEN << "[✓] " << message << " - Complete" << RESET << "\n";
}

void displayProgressBar(const std::string& label, int percentage, int width = 40) {
    std::cout << "\r" << WHITE << label << " [";
    int pos = width * percentage / 100;
    for (int i = 0; i < width; ++i) {
        if (i < pos)          std::cout << GREEN  << "█";
        else if (i == pos)    std::cout << YELLOW << "█";
        else                  std::cout << WHITE  << "░";
    }
    std::cout << WHITE << "] " << std::setw(3) << percentage << "%" << RESET;
    std::cout.flush();
}

/* ------------------------------------------------------------------------- */
/*  Aşağıdaki büyük “display...” ve yardımcı fonksiyonlar orijinal haliyle    */
/*  bırakıldı. Sadece UI::initScreen kilitlenmesi çözüldü.                   */
/* ------------------------------------------------------------------------- */

void displayModuleStatus() {
    std::cout << "\n" << BOLD << CYAN
              << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << CYAN << "║                        MODULE STATUS                            ║" << RESET << "\n";
    std::cout << BOLD << CYAN
              << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::vector<std::pair<std::string, std::string>> modules = {
        {"Kernel Core", "ACTIVE"},      {"Command Parser", "LOADED"},
        {"System Calls", "READY"},      {"Device Interface", "INITIALIZED"},
        {"Network Scanner", "STANDBY"}, {"Hash Cracker", "READY"},
        {"Ping Engine", "ACTIVE"},      {"Security Suite", "LOADED"},
        {"Hardware HAL", "CONNECTED"},  {"Memory Manager", "OPTIMIZED"}
    };

    for (const auto& module : modules) {
        std::cout << WHITE << "  " << std::setw(20) << std::left << module.first << " : ";
        if (module.second == "ACTIVE")          std::cout << BOLD << GREEN  << module.second;
        else if (module.second == "READY"
              || module.second == "LOADED")     std::cout << GREEN << module.second;
        else                                    std::cout << YELLOW << module.second;
        std::cout << RESET << "\n";
    }
    std::cout << "\n";
}

void displayNetworkInfo() {
    std::cout << BOLD << BLUE
              << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << BLUE << "║                       NETWORK STATUS                            ║" << RESET << "\n";
    std::cout << BOLD << BLUE
              << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << WHITE << "Interface:     " << GREEN << "wlan0 (Wireless)"      << RESET << "\n";
    std::cout << WHITE << "IP Address:    " << GREEN << "192.168.1.100"         << RESET << "\n";
    std::cout << WHITE << "Subnet Mask:   " << GREEN << "255.255.255.0"         << RESET << "\n";
    std::cout << WHITE << "Gateway:       " << GREEN << "192.168.1.1"           << RESET << "\n";
    std::cout << WHITE << "DNS Server:    " << GREEN << "8.8.8.8, 8.8.4.4"      << RESET << "\n";
    std::cout << WHITE << "Signal:        " << GREEN << "██████████ (100%)"     << RESET << "\n";
    std::cout << WHITE << "Encryption:    " << GREEN << "WPA3-SAE"              << RESET << "\n\n";
}

void displaySecurityDashboard() {
    std::cout << BOLD << RED
              << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << RED << "║                     SECURITY DASHBOARD                          ║" << RESET << "\n";
    std::cout << BOLD << RED
              << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << WHITE << "Firewall Status:     " << GREEN  << "ACTIVE"       << RESET << "\n";
    std::cout << WHITE << "Intrusion Detection: " << GREEN  << "MONITORING"   << RESET << "\n";
    std::cout << WHITE << "VPN Connection:      " << YELLOW << "DISCONNECTED" << RESET << "\n";
    std::cout << WHITE << "Encryption Level:    " << GREEN  << "AES-256"      << RESET << "\n";
    std::cout << WHITE << "Root Protection:     " << GREEN  << "ENABLED"      << RESET << "\n";
    std::cout << WHITE << "Security Tools:      " << GREEN  << "15 LOADED"    << RESET << "\n";
    std::cout << WHITE << "Threat Level:        " << GREEN  << "LOW"          << RESET << "\n\n";
}

void displaySystemStats() {
    std::cout << BOLD << MAGENTA
              << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << MAGENTA << "║                      SYSTEM STATISTICS                          ║" << RESET << "\n";
    std::cout << BOLD << MAGENTA
              << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << WHITE << "Boot Time:         " << GREEN << "12.3 seconds"   << RESET << "\n";
    std::cout << WHITE << "Uptime:            " << GREEN << "2h 15m 43s"     << RESET << "\n";
    std::cout << WHITE << "Commands Executed: " << GREEN << "247"            << RESET << "\n";
    std::cout << WHITE << "Network Scans:     " << GREEN << "15"             << RESET << "\n";
    std::cout << WHITE << "Successful Pings:  " << GREEN << "98.7%"          << RESET << "\n";
    std::cout << WHITE << "Hash Cracks:       " << GREEN << "7/12"           << RESET << "\n";
    std::cout << WHITE << "System Load:       " << GREEN << "0.24, 0.18, 0.12"<< RESET << "\n\n";
}

void displayAboutSystem() {
    std::cout << BOLD << CYAN
              << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << CYAN << "║                        ABOUT OROTO                              ║" << RESET << "\n";
    std::cout << BOLD << CYAN
              << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << WHITE << "Oroto is an advanced kernel-based mobile operating system designed" << RESET << "\n";
    std::cout << WHITE << "for security professionals and penetration testers. Built from the" << RESET << "\n";
    std::cout << WHITE << "ground up with a modular architecture, Oroto provides comprehensive" << RESET << "\n";
    std::cout << WHITE << "tools for network analysis, system control, and security testing."   << RESET << "\n\n";

    std::cout << GREEN  << "Features:"                                                   << RESET << "\n";
    std::cout << WHITE  << "  • Real-time kernel with hardware abstraction"             << RESET << "\n";
    std::cout << WHITE  << "  • Advanced security and penetration testing tools"        << RESET << "\n";
    std::cout << WHITE  << "  • Complete device control (camera, microphone, storage)"  << RESET << "\n";
    std::cout << WHITE  << "  • Network scanning and monitoring capabilities"           << RESET << "\n";
    std::cout << WHITE  << "  • Modular architecture for extensibility"                 << RESET << "\n\n";

    std::cout << YELLOW << "Version: 3.2.1 Professional Security Edition" << RESET << "\n";
    std::cout << YELLOW << "Build: 2024.01.02"                            << RESET << "\n";
    std::cout << YELLOW << "Architecture: ARM64-Mobile"                   << RESET << "\n\n";
}

void clearScreenAdvanced() {
#ifdef _WIN32
    system("cls");
#else
    if (system("clear") != 0) { /* ignore */ }
#endif
    displaySystemStatusBar();
    std::cout << "\n";
}

void displayErrorMessage(const std::string& error, const std::string& hint = "") {
    std::cout << BOLD << RED
              << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << RED << "║                            ERROR                                 ║" << RESET << "\n";
    std::cout << BOLD << RED
              << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << RED << "Error: " << error << RESET << "\n";
    if (!hint.empty()) std::cout << YELLOW << "Hint: " << hint << RESET << "\n";
    std::cout << "\n";
}

void displaySuccessMessage(const std::string& message) {
    std::cout << GREEN << "✓ " << message << RESET << "\n";
}