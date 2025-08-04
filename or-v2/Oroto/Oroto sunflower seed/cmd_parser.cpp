#include "lib/oroto_shell.h"
#include "lib/colors.h"
#include "lib/logger.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

void initCommandParser() {
    LOG_INFO("CommandParser", "Command parser module initialized");
}

#include "lib/thread_pool.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

extern void executeSystemCall(const std::string& syscall, const std::vector<std::string>& args);
extern void executeDeviceCommand(const std::string& device, const std::string& action, const std::vector<std::string>& args);
extern void executeNmapScan(const std::vector<std::string>& args);
extern void executeHashCrack(const std::vector<std::string>& args);
extern void executePing(const std::vector<std::string>& args);

void executeDirectCommand(const std::string& command, const std::vector<std::string>& args);
void showJobs();
void killJob(const std::string& jobIdStr);

std::vector<std::string> parseArguments(const std::string& command) {
    std::vector<std::string> args;
    std::stringstream ss(command);
    std::string arg;

    while (ss >> arg) {
        args.push_back(arg);
    }

    return args;
}

void showCommandHelp() {
    std::cout << "\n" << BOLD << YELLOW << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << YELLOW << "║                    OROTO COMPLETE COMMAND REFERENCE             ║" << RESET << "\n";
    std::cout << BOLD << YELLOW << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << WHITE << "📁 File and Directory Operations:" << RESET << "\n";
    std::cout << GREEN << "  ls" << WHITE << "                     - List files in current directory" << RESET << "\n";
    std::cout << GREEN << "  cd [folder]" << WHITE << "            - Change to specified directory" << RESET << "\n";
    std::cout << GREEN << "  pwd" << WHITE << "                    - Show current directory path" << RESET << "\n";
    std::cout << GREEN << "  mkdir [name]" << WHITE << "           - Create new folder" << RESET << "\n";
    std::cout << GREEN << "  rm [file]" << WHITE << "              - Delete file (safe mode)" << RESET << "\n";
    std::cout << GREEN << "  rmdir [folder]" << WHITE << "         - Delete empty folder" << RESET << "\n";
    std::cout << GREEN << "  touch [file]" << WHITE << "           - Create new empty file" << RESET << "\n";
    std::cout << GREEN << "  cat [file]" << WHITE << "             - Display file contents" << RESET << "\n";
    std::cout << GREEN << "  echo [text] > [file]" << WHITE << "   - Write text to file" << RESET << "\n\n";

    std::cout << WHITE << "🔧 System Commands:" << RESET << "\n";
    std::cout << GREEN << "  clear" << WHITE << "                  - Clear screen (keep OROTO header)" << RESET << "\n";
    std::cout << GREEN << "  reboot" << WHITE << "                 - Restart system" << RESET << "\n";
    std::cout << GREEN << "  shutdown" << WHITE << "               - Shutdown system" << RESET << "\n";
    std::cout << GREEN << "  help" << WHITE << "                   - Show this help" << RESET << "\n";
    std::cout << GREEN << "  status" << WHITE << "                 - System status and kernel usage" << RESET << "\n";
    std::cout << GREEN << "  memory" << WHITE << "                 - RAM usage information" << RESET << "\n";
    std::cout << GREEN << "  cpu" << WHITE << "                    - Processor information" << RESET << "\n";
    std::cout << GREEN << "  uptime" << WHITE << "                 - System uptime" << RESET << "\n";
    std::cout << GREEN << "  date" << WHITE << "                   - Current date and time" << RESET << "\n\n";

    std::cout << WHITE << "🌐 Network Commands:" << RESET << "\n";
    std::cout << GREEN << "  ping [host]" << WHITE << "            - Ping specified IP or domain" << RESET << "\n";
    std::cout << GREEN << "  ifconfig" << WHITE << "               - Network interface information" << RESET << "\n";
    std::cout << GREEN << "  netstat" << WHITE << "                - List active connections" << RESET << "\n";
    std::cout << GREEN << "  traceroute [host]" << WHITE << "      - Show route to target" << RESET << "\n";
    std::cout << GREEN << "  whois [domain]" << WHITE << "         - Get domain information" << RESET << "\n";
    std::cout << GREEN << "  dnslookup [host]" << WHITE << "       - Perform DNS resolution" << RESET << "\n\n";

    std::cout << WHITE << "🛡️ Security Tools:" << RESET << "\n";
    std::cout << GREEN << "  nmap [host]" << WHITE << "            - Scan ports (safe mode)" << RESET << "\n";
    std::cout << GREEN << "  hashid [hash]" << WHITE << "          - Identify hash type" << RESET << "\n";
    std::cout << GREEN << "  crack [md5_hash]" << WHITE << "       - Crack simple hashes" << RESET << "\n";
    std::cout << GREEN << "  ftpconnect [host]" << WHITE << "      - Establish FTP connection" << RESET << "\n";
    std::cout << GREEN << "  tcpdump" << WHITE << "                - Analyze network traffic" << RESET << "\n\n";

    std::cout << WHITE << "📷 Hardware Access:" << RESET << "\n";
    std::cout << GREEN << "  cam open" << WHITE << "               - Start camera" << RESET << "\n";
    std::cout << GREEN << "  cam snap" << WHITE << "               - Take photo" << RESET << "\n";
    std::cout << GREEN << "  cam close" << WHITE << "              - Close camera" << RESET << "\n";
    std::cout << GREEN << "  mic status" << WHITE << "             - Check microphone status" << RESET << "\n";
    std::cout << GREEN << "  storage list" << WHITE << "           - List storage contents" << RESET << "\n\n";

    std::cout << WHITE << "🔰 Kernel Commands:" << RESET << "\n";
    std::cout << GREEN << "  oroto status" << WHITE << "           - Detailed kernel status" << RESET << "\n";
    std::cout << GREEN << "  oroto root" << WHITE << "             - Enter root kernel mode" << RESET << "\n";
    std::cout << GREEN << "  oroto tool list" << WHITE << "        - List all installed tools" << RESET << "\n\n";
}

void showToolList() {
    std::cout << "\n" << BOLD << CYAN << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << CYAN << "║                     INSTALLED SECURITY TOOLS                    ║" << RESET << "\n";
    std::cout << BOLD << CYAN << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    std::cout << WHITE << "Network Tools:" << RESET << "\n";
    std::cout << GREEN << "  nmap" << WHITE << "              - Network port scanner" << RESET << "\n";
    std::cout << GREEN << "  whois" << WHITE << "             - Domain information" << RESET << "\n";
    std::cout << GREEN << "  traceroute" << WHITE << "        - Route tracer" << RESET << "\n";
    std::cout << GREEN << "  netstat" << WHITE << "           - Network statistics" << RESET << "\n";
    std::cout << GREEN << "  tcpdump" << WHITE << "           - Packet analyzer" << RESET << "\n\n";

    std::cout << WHITE << "Security Tools:" << RESET << "\n";
    std::cout << GREEN << "  hash-identifier" << WHITE << "   - Hash type detector" << RESET << "\n";
    std::cout << GREEN << "  sqlmap" << WHITE << "            - SQL injection tool" << RESET << "\n";
    std::cout << GREEN << "  hydra" << WHITE << "             - Brute force tool" << RESET << "\n";
    std::cout << GREEN << "  airmon-ng" << WHITE << "         - Wireless monitor" << RESET << "\n";
    std::cout << GREEN << "  aircrack" << WHITE << "          - WiFi cracker" << RESET << "\n\n";

    std::cout << WHITE << "Remote Access:" << RESET << "\n";
    std::cout << GREEN << "  ssh" << WHITE << "               - Secure shell" << RESET << "\n";
    std::cout << GREEN << "  ftp" << WHITE << "               - File transfer" << RESET << "\n\n";
}

void processCommand(const std::string& command) {
    std::vector<std::string> args = parseArguments(command);

    if (args.empty()) return;

    std::string mainCmd = args[0];
    std::transform(mainCmd.begin(), mainCmd.end(), mainCmd.begin(), ::tolower);

    // Handle direct commands without 'oroto' prefix
    if (mainCmd == "ls" || mainCmd == "cd" || mainCmd == "pwd" || mainCmd == "mkdir" || 
        mainCmd == "rm" || mainCmd == "rmdir" || mainCmd == "touch" || mainCmd == "cat" || 
        mainCmd == "echo" || mainCmd == "clear" || mainCmd == "reboot" || mainCmd == "shutdown" ||
        mainCmd == "help" || mainCmd == "status" || mainCmd == "memory" || mainCmd == "cpu" ||
        mainCmd == "uptime" || mainCmd == "date" || mainCmd == "ping" || mainCmd == "ifconfig" ||
        mainCmd == "netstat" || mainCmd == "traceroute" || mainCmd == "whois" || mainCmd == "dnslookup" ||
        mainCmd == "nmap" || mainCmd == "hashid" || mainCmd == "crack" || mainCmd == "ftpconnect" ||
        mainCmd == "tcpdump" || mainCmd == "cam" || mainCmd == "mic" || mainCmd == "storage") {
        executeDirectCommand(mainCmd, args);
        return;
    }

    if (mainCmd != "oroto") {
        std::cout << RED << "[ERROR] Unknown command: " << mainCmd << RESET << "\n";
        std::cout << YELLOW << "[HINT] Type 'help' for available commands or 'oroto help' for kernel commands" << RESET << "\n";
        return;
    }

    if (args.size() < 2) {
        std::cout << RED << "[ERROR] Incomplete command. Type 'oroto help' for usage" << RESET << "\n";
        return;
    }

    std::string subCmd = args[1];
    std::transform(subCmd.begin(), subCmd.end(), subCmd.begin(), ::tolower);

    if (subCmd == "help") {
        showCommandHelp();
    } else if (subCmd == "status") {
        executeSystemCall("status", args);
    } else if (subCmd == "root") {
        executeSystemCall("root", args);
    } else if (subCmd == "cam") {
        if (args.size() >= 3) {
            executeDeviceCommand("camera", args[2], args);
        } else {
            std::cout << RED << "[ERROR] Camera command requires action: open, snap, close" << RESET << "\n";
        }
    } else if (subCmd == "mic") {
        if (args.size() >= 3) {
            executeDeviceCommand("microphone", args[2], args);
        } else {
            std::cout << RED << "[ERROR] Microphone command requires action: status, start, stop" << RESET << "\n";
        }
    } else if (subCmd == "storage") {
        if (args.size() >= 3) {
            executeDeviceCommand("storage", args[2], args);
        } else {
            std::cout << RED << "[ERROR] Storage command requires action: info, write, read" << RESET << "\n";
        }
    } else if (subCmd == "scan") {
        if (args.size() >= 3 && args[2] == "net") {
            executeNmapScan(args);
        } else {
            std::cout << RED << "[ERROR] Unknown scan target. Use 'oroto scan net'" << RESET << "\n";
        }
    } else if (subCmd == "crack") {
        if (args.size() >= 3 && args[2] == "hash") {
            executeHashCrack(args);
        } else {
            std::cout << RED << "[ERROR] Use 'oroto crack hash [md5_hash]'" << RESET << "\n";
        }
    } else if (subCmd == "ping") {
        executePing(args);
    } else if (subCmd == "tool") {
        if (args.size() >= 3 && args[2] == "list") {
            showToolList();
        } else {
            std::cout << RED << "[ERROR] Use 'oroto tool list' to see available tools" << RESET << "\n";
        }
    } else if (subCmd == "exec") {
        if (args.size() >= 3) {
            executeSystemCall("exec", args);
        } else {
            std::cout << RED << "[ERROR] Specify tool to execute: oroto exec [tool_name]" << RESET << "\n";
        }
    } else {
        std::cout << RED << "[ERROR] Unknown command: " << subCmd << RESET << "\n";
        std::cout << YELLOW << "[HINT] Type 'oroto help' for available commands" << RESET << "\n";
    }
}

void executeDirectCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "ls") {
        std::cout << YELLOW << "[FS] Listing directory contents..." << RESET << "\n";
        std::cout << BLUE << "drwxr-xr-x" << WHITE << "  root  root   4096  Jan 02 14:30  " << GREEN << "bin/" << RESET << "\n";
        std::cout << BLUE << "drwxr-xr-x" << WHITE << "  root  root   4096  Jan 02 14:30  " << GREEN << "etc/" << RESET << "\n";
        std::cout << BLUE << "drwxr-xr-x" << WHITE << "  root  root   4096  Jan 02 14:30  " << GREEN << "home/" << RESET << "\n";
        std::cout << BLUE << "drwxr-xr-x" << WHITE << "  root  root   4096  Jan 02 14:30  " << GREEN << "storage/" << RESET << "\n";
        std::cout << WHITE << "-rw-r--r--" << WHITE << "  root  root   2048  Jan 02 14:30  " << WHITE << "system.log" << RESET << "\n";
        std::cout << WHITE << "-rw-r--r--" << WHITE << "  root  root   1024  Jan 02 14:30  " << WHITE << "kernel.conf" << RESET << "\n\n";
    } else if (command == "pwd") {
        std::cout << GREEN << "/home/oroto" << RESET << "\n\n";
    } else if (command == "date") {
        std::cout << GREEN << "Tue Jan  2 14:32:15 UTC 2024" << RESET << "\n\n";
    } else if (command == "uptime") {
        std::cout << GREEN << "System uptime: 0 days, 0 hours, 23 minutes, 42 seconds" << RESET << "\n";
        std::cout << WHITE << "Load average: 0.15, 0.08, 0.05" << RESET << "\n\n";
    } else if (command == "memory") {
        std::cout << "\n" << BOLD << CYAN << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
        std::cout << BOLD << CYAN << "║                        MEMORY USAGE REPORT                      ║" << RESET << "\n";
        std::cout << BOLD << CYAN << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";
        std::cout << WHITE << "Total RAM:     " << GREEN << "8192 MB" << RESET << "\n";
        std::cout << WHITE << "Used RAM:      " << GREEN << "2150 MB (26.3%)" << RESET << "\n";
        std::cout << WHITE << "Free RAM:      " << GREEN << "6042 MB (73.7%)" << RESET << "\n";
        std::cout << WHITE << "Cached:        " << GREEN << "485 MB" << RESET << "\n";
        std::cout << WHITE << "Buffers:       " << GREEN << "127 MB" << RESET << "\n";
        std::cout << WHITE << "Swap Total:    " << GREEN << "2048 MB" << RESET << "\n";
        std::cout << WHITE << "Swap Used:     " << GREEN << "0 MB (0%)" << RESET << "\n\n";
    } else if (command == "cpu") {
        std::cout << "\n" << BOLD << CYAN << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
        std::cout << BOLD << CYAN << "║                       CPU INFORMATION                           ║" << RESET << "\n";
        std::cout << BOLD << CYAN << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";
        std::cout << WHITE << "Processor:     " << GREEN << "ARM Cortex-A78 (4 cores)" << RESET << "\n";
        std::cout << WHITE << "Architecture:  " << GREEN << "ARM64" << RESET << "\n";
        std::cout << WHITE << "Clock Speed:   " << GREEN << "2.8 GHz" << RESET << "\n";
        std::cout << WHITE << "Cache L1:      " << GREEN << "64KB (per core)" << RESET << "\n";
        std::cout << WHITE << "Cache L2:      " << GREEN << "512KB (per core)" << RESET << "\n";
        std::cout << WHITE << "Cache L3:      " << GREEN << "4MB (shared)" << RESET << "\n";
        std::cout << WHITE << "Current Load:  " << GREEN << "12% average" << RESET << "\n\n";
    } else if (command == "ifconfig") {
        std::cout << GREEN << "wlan0: " << WHITE << "WiFi Interface" << RESET << "\n";
        std::cout << WHITE << "  inet addr: 192.168.1.105  Bcast: 192.168.1.255  Mask: 255.255.255.0" << RESET << "\n";
        std::cout << WHITE << "  UP BROADCAST RUNNING MULTICAST  MTU: 1500  Metric: 1" << RESET << "\n";
        std::cout << WHITE << "  RX packets: 15847  TX packets: 12043" << RESET << "\n\n";
        std::cout << GREEN << "lo: " << WHITE << "Loopback Interface" << RESET << "\n";
        std::cout << WHITE << "  inet addr: 127.0.0.1  Mask: 255.0.0.0" << RESET << "\n";
        std::cout << WHITE << "  UP LOOPBACK RUNNING  MTU: 65536  Metric: 1" << RESET << "\n\n";
    } else if (command == "netstat") {
        std::cout << YELLOW << "[NETWORK] Active connections:" << RESET << "\n";
        std::cout << WHITE << "Proto  Local Address         Foreign Address       State" << RESET << "\n";
        std::cout << GREEN << "tcp    127.0.0.1:5000       *:*                   LISTEN" << RESET << "\n";
        std::cout << GREEN << "tcp    192.168.1.105:53     8.8.8.8:53            ESTABLISHED" << RESET << "\n";
        std::cout << GREEN << "udp    0.0.0.0:68           *:*                   " << RESET << "\n\n";
    } else if (command == "clear") {
        #ifdef _WIN32
            if (system("cls") != 0) {
                // Ignore clear screen failure
            }
        #else
            if (system("clear") != 0) {
                // Ignore clear screen failure
            }
        #endif
        std::cout << BOLD << CYAN;
        std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║   ██████╗ ██████╗  ██████╗ ████████╗ ██████╗                    ║\n";
        std::cout << "║  ██╔═══██╗██╔══██╗██╔═══██╗╚══██╔══╝██╔═══██╗                   ║\n";
        std::cout << "║  ██║   ██║██████╔╝██║   ██║   ██║   ██║   ██║                   ║\n";
        std::cout << "║  ██║   ██║██╔══██╗██║   ██║   ██║   ██║   ██║                   ║\n";
        std::cout << "║  ╚██████╔╝██║  ██║╚██████╔╝   ██║   ╚██████╔╝                   ║\n";
        std::cout << "║   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝    ╚═╝    ╚═════╝                    ║\n";
        std::cout << "║                 Oroto Kernel v3.2.1 - Ready                    ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
        std::cout << RESET << "\n";
    } else if (command == "mkdir" && args.size() >= 2) {
        std::cout << GREEN << "[FS] Directory created: " << args[1] << RESET << "\n\n";
    } else if (command == "touch" && args.size() >= 2) {
        std::cout << GREEN << "[FS] File created: " << args[1] << RESET << "\n\n";
    } else if (command == "rm" && args.size() >= 2) {
        std::cout << YELLOW << "[FS] File deleted (safe mode): " << args[1] << RESET << "\n\n";
    } else if (command == "cat" && args.size() >= 2) {
        std::cout << WHITE << "File contents of " << args[1] << ":" << RESET << "\n";
        std::cout << GREEN << "# Oroto system configuration" << RESET << "\n";
        std::cout << GREEN << "kernel_version=3.2.1" << RESET << "\n";
        std::cout << GREEN << "security_level=high" << RESET << "\n\n";
    } else if (command == "ping" && args.size() >= 2) {
        executePing(args);
    } else if (command == "nmap" && args.size() >= 2) {
        executeNmapScan(args);
    } else if (command == "hashid" && args.size() >= 2) {
        std::cout << YELLOW << "[HASHID] Analyzing hash: " << args[1] << RESET << "\n";
        std::cout << GREEN << "[HASHID] Most likely hash type: MD5" << RESET << "\n";
        std::cout << WHITE << "Other possibilities: NTLM, MD4" << RESET << "\n\n";
    } else if (command == "crack" && args.size() >= 2) {
        std::vector<std::string> crackArgs = {"crack", "hash", args[1]};
        executeHashCrack(crackArgs);
    } else if (command == "tcpdump") {
        std::cout << YELLOW << "[TCPDUMP] Starting packet capture..." << RESET << "\n";
        std::cout << GREEN << "14:32:15.123456 IP 192.168.1.105.5000 > 8.8.8.8.53: UDP, length 32" << RESET << "\n";
        std::cout << GREEN << "14:32:15.167834 IP 8.8.8.8.53 > 192.168.1.105.5000: UDP, length 48" << RESET << "\n";
        std::cout << GREEN << "14:32:16.234567 ARP, Request who-has 192.168.1.1 tell 192.168.1.105" << RESET << "\n";
        std::cout << WHITE << "3 packets captured" << RESET << "\n\n";
    } else if (command == "whois" && args.size() >= 2) {
        std::cout << YELLOW << "[WHOIS] Looking up domain: " << args[1] << RESET << "\n";
        std::cout << WHITE << "Domain Name: " << GREEN << args[1] << RESET << "\n";
        std::cout << WHITE << "Registry Domain ID: D123456789-LROR" << RESET << "\n";
        std::cout << WHITE << "Registrar: Example Registrar Inc." << RESET << "\n";
        std::cout << WHITE << "Creation Date: 2020-01-01T00:00:00Z" << RESET << "\n";
        std::cout << WHITE << "Registry Expiry Date: 2025-01-01T00:00:00Z" << RESET << "\n\n";
    } else if (command == "dnslookup" && args.size() >= 2) {
        std::cout << YELLOW << "[DNS] Resolving: " << args[1] << RESET << "\n";
        std::cout << GREEN << args[1] << " has address 8.8.8.8" << RESET << "\n";
        std::cout << GREEN << args[1] << " has IPv6 address 2001:4860:4860::8888" << RESET << "\n\n";
    } else if (command == "traceroute" && args.size() >= 2) {
        std::cout << YELLOW << "[TRACEROUTE] Tracing route to " << args[1] << RESET << "\n";
        std::cout << WHITE << " 1  192.168.1.1         2.456 ms  2.123 ms  1.987 ms" << RESET << "\n";
        std::cout << WHITE << " 2  10.0.0.1           15.234 ms 14.567 ms 15.123 ms" << RESET << "\n";
        std::cout << WHITE << " 3  203.0.113.1        28.456 ms 29.123 ms 27.987 ms" << RESET << "\n";
        std::cout << WHITE << " 4  " << GREEN << args[1] << WHITE << "  45.234 ms 44.567 ms 45.123 ms" << RESET << "\n\n";
    } else if (command == "ftpconnect" && args.size() >= 2) {
        std::cout << YELLOW << "[FTP] Connecting to " << args[1] << "..." << RESET << "\n";
        std::cout << GREEN << "[FTP] Connected to " << args[1] << RESET << "\n";
        std::cout << WHITE << "220 FTP Server ready." << RESET << "\n";
        std::cout << WHITE << "User (" << args[1] << ":(none)): " << RESET;
    } else if (command == "cam") {
        if (args.size() >= 2) {
            if (args[1] == "open") executeDeviceCommand("camera", "open", args);
            else if (args[1] == "snap") executeDeviceCommand("camera", "snap", args);
            else if (args[1] == "close") executeDeviceCommand("camera", "close", args);
            else std::cout << RED << "[ERROR] Camera usage: cam [open|snap|close]" << RESET << "\n";
        } else {
            std::cout << RED << "[ERROR] Camera usage: cam [open|snap|close]" << RESET << "\n";
        }
    } else if (command == "mic") {
        if (args.size() >= 2) {
            executeDeviceCommand("microphone", args[1], args);
        } else {
            std::cout << RED << "[ERROR] Microphone usage: mic [status|start|stop]" << RESET << "\n";
        }
    } else if (command == "storage") {
        if (args.size() >= 2) {
            if (args[1] == "list") executeDeviceCommand("storage", "info", args);
            else executeDeviceCommand("storage", args[1], args);
        } else {
            std::cout << RED << "[ERROR] Storage usage: storage [list|info|write|read]" << RESET << "\n";
        }
    } else if (command == "jobs") {
        showJobs();
    } else if (command == "kill" && args.size() >= 2) {
        killJob(args[1]);
    } else if (command == "help") {
        showCommandHelp();
    } else if (command == "status") {
        executeSystemCall("status", args);
    } else if (command == "reboot") {
        executeSystemCall("reboot", args);
    } else if (command == "shutdown") {
        executeSystemCall("shutdown", args);
    } else {
        std::cout << RED << "[ERROR] Command requires additional parameters or is not implemented" << RESET << "\n";
        std::cout << YELLOW << "[HINT] Type 'help' for usage information" << RESET << "\n";
    }
}

void showJobs() {
    std::cout << "\n" << BOLD << CYAN << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << CYAN << "║                        ACTIVE JOBS MANAGER                      ║" << RESET << "\n";
    std::cout << BOLD << CYAN << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    auto& threadPool = Oroto::getThreadPool();
    auto jobs = threadPool.listJobs();
    auto stats = threadPool.getStats();

    std::cout << WHITE << "Thread Pool Statistics:" << RESET << "\n";
    std::cout << GREEN << "  Active Threads: " << stats.numThreads << RESET << "\n";
    std::cout << GREEN << "  Queue Size: " << stats.queueSize << RESET << "\n";
    std::cout << GREEN << "  Total Jobs: " << stats.totalJobs << RESET << "\n";
    std::cout << GREEN << "  Pending: " << stats.pendingJobs << RESET << "\n";
    std::cout << GREEN << "  Running: " << stats.runningJobs << RESET << "\n";
    std::cout << GREEN << "  Completed: " << stats.completedJobs << RESET << "\n";
    std::cout << GREEN << "  Failed: " << stats.failedJobs << RESET << "\n\n";

    if (jobs.empty()) {
        std::cout << YELLOW << "No active jobs" << RESET << "\n\n";
        return;
    }

    std::cout << WHITE << "Job ID  Status      Name                Duration" << RESET << "\n";
    std::cout << WHITE << "------  ----------  ------------------  --------" << RESET << "\n";

    for (const auto& job : jobs) {
        std::string statusStr;
        std::string color;

        switch (job->status) {
            case Oroto::JobStatus::PENDING:
                statusStr = "PENDING";
                color = YELLOW;
                break;
            case Oroto::JobStatus::RUNNING:
                statusStr = "RUNNING";
                color = GREEN;
                break;
            case Oroto::JobStatus::COMPLETED:
                statusStr = "COMPLETED";
                color = BLUE;
                break;
            case Oroto::JobStatus::FAILED:
                statusStr = "FAILED";
                color = RED;
                break;
            case Oroto::JobStatus::CANCELLED:
                statusStr = "CANCELLED";
                color = MAGENTA;
                break;
        }

        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - job->startTime
        ).count();

        std::cout << WHITE << std::setw(6) << job->id << "  "
                  << color << std::setw(10) << statusStr << WHITE << "  "
                  << std::setw(18) << job->name.substr(0, 18) << "  "
                  << std::setw(8) << duration << "s" << RESET << "\n";
    }
    std::cout << "\n";
}

void killJob(const std::string& jobIdStr) {
    try {
        size_t jobId = std::stoull(jobIdStr);
        auto& threadPool = Oroto::getThreadPool();

        if (threadPool.cancelJob(jobId)) {
            std::cout << GREEN << "[JOBS] Job " << jobId << " cancelled successfully" << RESET << "\n\n";
        } else {
            std::cout << RED << "[ERROR] Cannot cancel job " << jobId 
                      << " (not found or already running)" << RESET << "\n\n";
        }
    } catch (const std::exception& e) {
        std::cout << RED << "[ERROR] Invalid job ID: " << jobIdStr << RESET << "\n\n";
    }
}