
#include "../lib/oroto_shell.h"
#include "../lib/colors.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <iomanip>

void simulateCracking(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void showProgressBar(int percentage) {
    std::cout << "\r" << YELLOW << "[CRACK] Progress: [";
    int pos = percentage / 5;
    for (int i = 0; i < 20; ++i) {
        if (i < pos) std::cout << GREEN << "█";
        else std::cout << WHITE << "░";
    }
    std::cout << YELLOW << "] " << percentage << "%" << RESET;
    std::cout.flush();
}

void performDictionaryAttack(const std::string& hash) {
    std::cout << GREEN << "[CRACK] Starting dictionary attack on MD5 hash" << RESET << "\n";
    std::cout << WHITE << "Hash: " << hash << RESET << "\n";
    std::cout << WHITE << "Method: Dictionary + Rule-based" << RESET << "\n\n";
    
    std::vector<std::string> commonPasswords = {
        "password", "123456", "password123", "admin", "root", "test", 
        "hello", "world", "login", "pass", "secret", "qwerty", "abc123"
    };
    
    for (int i = 0; i <= 100; i += 5) {
        showProgressBar(i);
        simulateCracking(200 + (rand() % 300));
    }
    
    std::cout << "\n\n";
    
    if (hash.length() == 32 && rand() % 3 == 0) {
        std::string cracked = commonPasswords[rand() % commonPasswords.size()];
        std::cout << BOLD << GREEN << "🎉 HASH CRACKED SUCCESSFULLY! 🎉" << RESET << "\n";
        std::cout << WHITE << "Original text: " << BOLD << GREEN << cracked << RESET << "\n";
        std::cout << WHITE << "Time taken: " << GREEN << (rand() % 60 + 10) << " seconds" << RESET << "\n";
        std::cout << WHITE << "Attempts: " << GREEN << (rand() % 10000 + 1000) << RESET << "\n\n";
    } else if (hash.length() != 32) {
        std::cout << RED << "[ERROR] Invalid MD5 hash format" << RESET << "\n";
        std::cout << YELLOW << "[HINT] MD5 hashes are exactly 32 hexadecimal characters" << RESET << "\n\n";
    } else {
        std::cout << RED << "❌ Hash could not be cracked" << RESET << "\n";
        std::cout << YELLOW << "[INFO] Hash may use a complex password not in dictionary" << RESET << "\n";
        std::cout << YELLOW << "[HINT] Try brute force or larger wordlist" << RESET << "\n\n";
    }
}

void performBruteForce(const std::string& hash __attribute__((unused))) {
    std::cout << GREEN << "[CRACK] Starting brute force attack" << RESET << "\n";
    std::cout << WHITE << "Character set: a-z, A-Z, 0-9" << RESET << "\n";
    std::cout << WHITE << "Max length: 6 characters" << RESET << "\n\n";
    
    for (int i = 0; i <= 100; i += 3) {
        showProgressBar(i);
        simulateCracking(400 + (rand() % 200));
        
        if (i > 30 && rand() % 20 == 0) {
            std::cout << "\n\n";
            std::cout << BOLD << GREEN << "🔓 BRUTE FORCE SUCCESS!" << RESET << "\n";
            std::cout << WHITE << "Password found: " << BOLD << GREEN << "pass1" << RESET << "\n";
            std::cout << WHITE << "Combinations tried: " << GREEN << (rand() % 1000000 + 100000) << RESET << "\n\n";
            return;
        }
    }
    
    std::cout << "\n\n";
    std::cout << RED << "⏰ Brute force timeout after 100% completion" << RESET << "\n";
    std::cout << YELLOW << "[INFO] Password may be longer than 6 characters" << RESET << "\n\n";
}

void showHashInfo(const std::string& hash) {
    std::cout << "\n" << BOLD << CYAN << "╔══════════════════════════════════════════════════════════════════╗" << RESET << "\n";
    std::cout << BOLD << CYAN << "║                        HASH ANALYSIS                            ║" << RESET << "\n";
    std::cout << BOLD << CYAN << "╚══════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";
    
    std::cout << WHITE << "Hash Value: " << GREEN << hash << RESET << "\n";
    std::cout << WHITE << "Length: " << GREEN << hash.length() << " characters" << RESET << "\n";
    
    if (hash.length() == 32) {
        std::cout << WHITE << "Type: " << GREEN << "MD5 (Message Digest 5)" << RESET << "\n";
        std::cout << WHITE << "Security: " << YELLOW << "Weak (Cryptographically broken)" << RESET << "\n";
        std::cout << WHITE << "Crack Difficulty: " << GREEN << "Easy to Medium" << RESET << "\n";
    } else if (hash.length() == 40) {
        std::cout << WHITE << "Type: " << GREEN << "SHA-1" << RESET << "\n";
        std::cout << WHITE << "Security: " << YELLOW << "Deprecated" << RESET << "\n";
        std::cout << WHITE << "Crack Difficulty: " << YELLOW << "Medium" << RESET << "\n";
    } else if (hash.length() == 64) {
        std::cout << WHITE << "Type: " << GREEN << "SHA-256" << RESET << "\n";
        std::cout << WHITE << "Security: " << GREEN << "Strong" << RESET << "\n";
        std::cout << WHITE << "Crack Difficulty: " << RED << "Very Hard" << RESET << "\n";
    } else {
        std::cout << WHITE << "Type: " << RED << "Unknown or Invalid" << RESET << "\n";
    }
    std::cout << "\n";
}

void executeHashCrack(const std::vector<std::string>& args) {
    std::cout << YELLOW << "[CRACK] Loading hash cracking engine..." << RESET << "\n";
    simulateCracking(1200);
    
    if (args.size() < 4) {
        std::cout << RED << "[ERROR] No hash provided" << RESET << "\n";
        std::cout << YELLOW << "[USAGE] oroto crack hash [md5_hash]" << RESET << "\n";
        std::cout << YELLOW << "[EXAMPLE] oroto crack hash 5d41402abc4b2a76b9719d911017c592" << RESET << "\n\n";
        return;
    }
    
    std::string hash = args[3];
    showHashInfo(hash);
    
    if (hash.length() == 32) {
        char choice;
        std::cout << WHITE << "Select attack method:" << RESET << "\n";
        std::cout << GREEN << "  [1] Dictionary Attack (Fast)" << RESET << "\n";
        std::cout << GREEN << "  [2] Brute Force (Slow but thorough)" << RESET << "\n";
        std::cout << BLUE << "Choice [1-2]: " << RESET;
        
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == '1') {
            performDictionaryAttack(hash);
        } else if (choice == '2') {
            performBruteForce(hash);
        } else {
            std::cout << YELLOW << "[INFO] Defaulting to dictionary attack" << RESET << "\n\n";
            performDictionaryAttack(hash);
        }
    } else {
        std::cout << YELLOW << "[INFO] Only MD5 hashes supported in this version" << RESET << "\n\n";
    }
}
