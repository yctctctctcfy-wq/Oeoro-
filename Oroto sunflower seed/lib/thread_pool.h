
#ifndef OROTO_THREAD_POOL_H
#define OROTO_THREAD_POOL_H

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include "logger.h"
#include "error_handler.h"

namespace Oroto {

// Job status enumeration
enum class JobStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED,
    CANCELLED
};

// Job information structure
struct JobInfo {
    size_t id;
    std::string name;
    JobStatus status;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    std::string result;
    std::string error;

    JobInfo(size_t jobId, const std::string& jobName) 
        : id(jobId), name(jobName), status(JobStatus::PENDING) {}
};

class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    std::atomic<size_t> nextJobId_;
    
    // Job tracking
    std::unordered_map<size_t, std::shared_ptr<JobInfo>> jobs_;
    std::mutex jobsMutex_;

public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) 
        : stop_(false), nextJobId_(1) {
        
        LOG_INFO("ThreadPool", "Initializing thread pool with " + 
                std::to_string(numThreads) + " threads");
        
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] { return stop_.load() || !tasks_.empty(); });
                        
                        if (stop_.load() && tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();
                }
            });
        }
        
        LOG_INFO("ThreadPool", "Thread pool initialized successfully");
    }

    ~ThreadPool() {
        shutdown();
    }

    // Submit a job and return its ID
    template<typename F, typename... Args>
    size_t submitJob(const std::string& jobName, F&& f, Args&&... args) {
        using ReturnType = typename std::result_of<F(Args...)>::type;
        
        size_t jobId = nextJobId_++;
        auto jobInfo = std::make_shared<JobInfo>(jobId, jobName);
        
        {
            std::lock_guard<std::mutex> lock(jobsMutex_);
            jobs_[jobId] = jobInfo;
        }
        
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        auto future = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            if (stop_.load()) {
                OROTO_THROW(ErrorCode::INTERNAL_ERROR, "ThreadPool", 
                           "Cannot submit job to stopped thread pool");
            }
            
            tasks_.emplace([task, jobInfo, this]() {
                jobInfo->status = JobStatus::RUNNING;
                jobInfo->startTime = std::chrono::steady_clock::now();
                
                try {
                    (*task)();
                    jobInfo->status = JobStatus::COMPLETED;
                    jobInfo->result = "Job completed successfully";
                    LOG_INFO("ThreadPool", "Job " + std::to_string(jobInfo->id) + 
                            " (" + jobInfo->name + ") completed");
                } catch (const std::exception& e) {
                    jobInfo->status = JobStatus::FAILED;
                    jobInfo->error = e.what();
                    LOG_ERROR("ThreadPool", "Job " + std::to_string(jobInfo->id) + 
                             " (" + jobInfo->name + ") failed: " + e.what());
                } catch (...) {
                    jobInfo->status = JobStatus::FAILED;
                    jobInfo->error = "Unknown error";
                    LOG_ERROR("ThreadPool", "Job " + std::to_string(jobInfo->id) + 
                             " (" + jobInfo->name + ") failed with unknown error");
                }
                
                jobInfo->endTime = std::chrono::steady_clock::now();
            });
        }
        
        condition_.notify_one();
        LOG_INFO("ThreadPool", "Submitted job " + std::to_string(jobId) + 
                " (" + jobName + ") to thread pool");
        
        return jobId;
    }

    // Get job information
    std::shared_ptr<JobInfo> getJobInfo(size_t jobId) {
        std::lock_guard<std::mutex> lock(jobsMutex_);
        auto it = jobs_.find(jobId);
        return (it != jobs_.end()) ? it->second : nullptr;
    }

    // List all jobs
    std::vector<std::shared_ptr<JobInfo>> listJobs() {
        std::lock_guard<std::mutex> lock(jobsMutex_);
        std::vector<std::shared_ptr<JobInfo>> result;
        for (const auto& pair : jobs_) {
            result.push_back(pair.second);
        }
        return result;
    }

    // Cancel a job (if not started yet)
    bool cancelJob(size_t jobId) {
        std::lock_guard<std::mutex> lock(jobsMutex_);
        auto it = jobs_.find(jobId);
        if (it != jobs_.end() && it->second->status == JobStatus::PENDING) {
            it->second->status = JobStatus::CANCELLED;
            LOG_INFO("ThreadPool", "Cancelled job " + std::to_string(jobId));
            return true;
        }
        return false;
    }

    // Remove completed jobs from tracking
    void cleanupJobs() {
        std::lock_guard<std::mutex> lock(jobsMutex_);
        auto it = jobs_.begin();
        while (it != jobs_.end()) {
            if (it->second->status == JobStatus::COMPLETED || 
                it->second->status == JobStatus::FAILED ||
                it->second->status == JobStatus::CANCELLED) {
                it = jobs_.erase(it);
            } else {
                ++it;
            }
        }
        LOG_INFO("ThreadPool", "Cleaned up completed jobs");
    }

    // Get pool statistics
    struct PoolStats {
        size_t numThreads;
        size_t queueSize;
        size_t totalJobs;
        size_t pendingJobs;
        size_t runningJobs;
        size_t completedJobs;
        size_t failedJobs;
    };

    PoolStats getStats() {
        PoolStats stats{};
        stats.numThreads = workers_.size();
        
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            stats.queueSize = tasks_.size();
        }
        
        {
            std::lock_guard<std::mutex> lock(jobsMutex_);
            stats.totalJobs = jobs_.size();
            
            for (const auto& pair : jobs_) {
                switch (pair.second->status) {
                    case JobStatus::PENDING: stats.pendingJobs++; break;
                    case JobStatus::RUNNING: stats.runningJobs++; break;
                    case JobStatus::COMPLETED: stats.completedJobs++; break;
                    case JobStatus::FAILED: stats.failedJobs++; break;
                    default: break;
                }
            }
        }
        
        return stats;
    }

    void shutdown() {
        if (!stop_.load()) {
            LOG_INFO("ThreadPool", "Shutting down thread pool...");
            
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                stop_ = true;
            }
            
            condition_.notify_all();
            
            for (std::thread& worker : workers_) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
            
            LOG_INFO("ThreadPool", "Thread pool shutdown complete");
        }
    }
};

// Global thread pool instance
extern std::unique_ptr<ThreadPool> g_threadPool;

// Initialize global thread pool
void initializeThreadPool(size_t numThreads = std::thread::hardware_concurrency());

// Get global thread pool
ThreadPool& getThreadPool();

// Shutdown global thread pool
void shutdownThreadPool();

} // namespace Oroto

#endif // OROTO_THREAD_POOL_H
