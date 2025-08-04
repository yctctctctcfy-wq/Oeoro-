
#include "thread_pool.h"

namespace Oroto {

// Global thread pool instance
std::unique_ptr<ThreadPool> g_threadPool;

void initializeThreadPool(size_t numThreads) {
    if (!g_threadPool) {
        g_threadPool = std::make_unique<ThreadPool>(numThreads);
        LOG_INFO("Global", "Global thread pool initialized with " + 
                std::to_string(numThreads) + " threads");
    }
}

ThreadPool& getThreadPool() {
    if (!g_threadPool) {
        initializeThreadPool();
    }
    return *g_threadPool;
}

void shutdownThreadPool() {
    if (g_threadPool) {
        g_threadPool->shutdown();
        g_threadPool.reset();
        LOG_INFO("Global", "Global thread pool shutdown complete");
    }
}

} // namespace Oroto
