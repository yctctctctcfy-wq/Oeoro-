
#ifndef OROTO_TEST_RUNNER_H
#define OROTO_TEST_RUNNER_H

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <chrono>
#include <exception>

namespace Oroto {
namespace Testing {

class TestResult {
public:
    bool passed;
    std::string name;
    std::string error_message;
    std::chrono::milliseconds duration;

    TestResult(const std::string& test_name, bool success, 
               const std::string& error = "", 
               std::chrono::milliseconds time = std::chrono::milliseconds(0))
        : passed(success), name(test_name), error_message(error), duration(time) {}
};

class TestRunner {
private:
    std::vector<std::pair<std::string, std::function<void()>>> tests_;
    std::vector<TestResult> results_;

public:
    void addTest(const std::string& name, std::function<void()> test) {
        tests_.emplace_back(name, std::move(test));
    }

    void runAllTests() {
        results_.clear();
        std::cout << "Running " << tests_.size() << " tests...\n\n";

        for (const auto& [name, test] : tests_) {
            runSingleTest(name, test);
        }

        printSummary();
    }

private:
    void runSingleTest(const std::string& name, const std::function<void()>& test) {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            test();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            results_.emplace_back(name, true, "", duration);
            std::cout << "✓ " << name << " (" << duration.count() << "ms)\n";
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            results_.emplace_back(name, false, e.what(), duration);
            std::cout << "✗ " << name << " - " << e.what() << " (" << duration.count() << "ms)\n";
        }
    }

    void printSummary() {
        size_t passed = 0;
        size_t failed = 0;
        std::chrono::milliseconds total_time(0);

        for (const auto& result : results_) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            total_time += result.duration;
        }

        std::cout << "\n" << "=" << std::string(50, '=') << "\n";
        std::cout << "Test Summary:\n";
        std::cout << "Total: " << results_.size() << "\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        std::cout << "Total time: " << total_time.count() << "ms\n";
        
        if (failed > 0) {
            std::cout << "\nFailed tests:\n";
            for (const auto& result : results_) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << ": " << result.error_message << "\n";
                }
            }
        }
    }
};

// Test assertion macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Assertion failed: " #condition " should be false"); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: expected " + std::to_string(expected) + \
                               " but got " + std::to_string(actual)); \
    }

#define ASSERT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        throw std::runtime_error("Assertion failed: values should not be equal"); \
    }

} // namespace Testing
} // namespace Oroto

#endif // OROTO_TEST_RUNNER_H
