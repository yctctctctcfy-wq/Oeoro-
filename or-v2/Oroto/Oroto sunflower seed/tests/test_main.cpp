
#include "test_runner.h"
#include "../lib/thread_pool.h"
#include "../lib/resource_manager.h"
#include "../lib/logger.h"
#include <memory>
#include <thread>
#include <chrono>

using namespace Oroto::Testing;

void testThreadPoolBasic() {
    Oroto::ThreadPool pool(2);
    
    std::atomic<int> counter(0);
    
    // Submit multiple jobs
    for (int i = 0; i < 5; ++i) {
        pool.submitJob("test_job_" + std::to_string(i), [&counter]() {
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }
    
    // Wait a bit for jobs to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    ASSERT_EQ(5, counter.load());
}

void testThreadPoolJobTracking() {
    Oroto::ThreadPool pool(1);
    
    auto jobId = pool.submitJob("tracking_test", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });
    
    auto jobInfo = pool.getJobInfo(jobId);
    ASSERT_TRUE(jobInfo != nullptr);
    ASSERT_EQ("tracking_test", jobInfo->name);
    
    // Wait for job to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    jobInfo = pool.getJobInfo(jobId);
    ASSERT_TRUE(jobInfo->status == Oroto::JobStatus::COMPLETED);
}

void testResourceManagerBasic() {
    Oroto::ResourceManager<std::string> manager;
    
    auto resource = std::make_shared<std::string>("test_data");
    ASSERT_TRUE(manager.addResource("test1", resource));
    
    ASSERT_TRUE(manager.hasResource("test1"));
    ASSERT_FALSE(manager.hasResource("nonexistent"));
    
    auto retrieved = manager.getSharedResource("test1");
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ("test_data", *retrieved);
    
    ASSERT_EQ(1, manager.getResourceCount());
    
    ASSERT_TRUE(manager.removeResource("test1"));
    ASSERT_EQ(0, manager.getResourceCount());
}

void testResourceManagerCreateInPlace() {
    Oroto::ResourceManager<std::string> manager;
    
    auto resource = manager.createResource<std::string>("test2", "created_in_place");
    ASSERT_TRUE(resource != nullptr);
    ASSERT_EQ("created_in_place", *resource);
    
    ASSERT_TRUE(manager.hasResource("test2"));
}

void testResourceManagerCleanup() {
    Oroto::ResourceManager<std::string> manager;
    
    bool cleanup_called = false;
    auto resource = std::make_shared<std::string>("cleanup_test");
    
    manager.addResource("cleanup", resource, [&cleanup_called]() {
        cleanup_called = true;
    });
    
    manager.removeResource("cleanup");
    ASSERT_TRUE(cleanup_called);
}

void testLoggerInitialization() {
    ASSERT_TRUE(Oroto::Logger::initialize("test_log.log", Oroto::LogLevel::DEBUG));
    
    // Test basic logging
    Oroto::Logger::info("Test", "Logger initialization test");
    Oroto::Logger::debug("Test", "Debug message test");
    Oroto::Logger::warning("Test", "Warning message test");
    
    Oroto::Logger::shutdown();
}

int main() {
    TestRunner runner;
    
    // Add all tests
    runner.addTest("ThreadPool Basic Functionality", testThreadPoolBasic);
    runner.addTest("ThreadPool Job Tracking", testThreadPoolJobTracking);
    runner.addTest("ResourceManager Basic Operations", testResourceManagerBasic);
    runner.addTest("ResourceManager Create In Place", testResourceManagerCreateInPlace);
    runner.addTest("ResourceManager Cleanup", testResourceManagerCleanup);
    runner.addTest("Logger Initialization", testLoggerInitialization);
    
    // Run all tests
    runner.runAllTests();
    
    return 0;
}
