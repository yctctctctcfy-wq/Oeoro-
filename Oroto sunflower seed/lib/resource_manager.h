#ifndef OROTO_RESOURCE_MANAGER_H
#define OROTO_RESOURCE_MANAGER_H

#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>
#include <functional>
#include <atomic>
#include "logger.h"
#include "error_handler.h"

namespace Oroto {

template<typename T>
class ResourceManager {
private:
    std::unordered_map<std::string, std::shared_ptr<T>> resources_;
    std::unordered_map<std::string, std::function<void()>> cleanupCallbacks_;
    mutable std::mutex resourceMutex_;
    std::atomic<size_t> resourceCount_{0};

public:
    ResourceManager() = default;
    ~ResourceManager() {
        clearResources();
    }

    // Delete copy constructor and assignment operator
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Allow move operations
    ResourceManager(ResourceManager&&) = default;
    ResourceManager& operator=(ResourceManager&&) = default;

    // Add resource with smart pointer
    bool addResource(const std::string& id, std::shared_ptr<T> resource, 
                    std::function<void()> cleanup = nullptr) {
        std::lock_guard<std::mutex> lock(resourceMutex_);

        if (resources_.find(id) != resources_.end()) {
            LOG_WARNING("ResourceManager", "Resource " + id + " already exists");
            return false;
        }

        resources_[id] = std::move(resource);
        if (cleanup) {
            cleanupCallbacks_[id] = std::move(cleanup);
        }

        ++resourceCount_;
        LOG_INFO("ResourceManager", "Added resource: " + id);
        return true;
    }

    // Create and add resource in place
    template<typename U = T, typename... Args>
    std::shared_ptr<U> createResource(const std::string& id, Args&&... args) {
        auto resource = std::make_shared<U>(std::forward<Args>(args)...);
        if (addResource(id, std::static_pointer_cast<T>(resource))) {
            return resource;
        }
        return nullptr;
    }

    // Get resource (returns weak_ptr to prevent circular dependencies)
    std::weak_ptr<T> getResource(const std::string& id) const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        auto it = resources_.find(id);
        if (it != resources_.end()) {
            return it->second;
        }
        return {};
    }

    // Get resource as shared_ptr (use carefully)
    std::shared_ptr<T> getSharedResource(const std::string& id) const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        auto it = resources_.find(id);
        if (it != resources_.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Remove specific resource
    bool removeResource(const std::string& id) {
        std::lock_guard<std::mutex> lock(resourceMutex_);

        auto it = resources_.find(id);
        if (it == resources_.end()) {
            LOG_WARNING("ResourceManager", "Resource " + id + " not found");
            return false;
        }

        // Run cleanup callback if exists
        auto cleanupIt = cleanupCallbacks_.find(id);
        if (cleanupIt != cleanupCallbacks_.end()) {
            try {
                cleanupIt->second();
                cleanupCallbacks_.erase(cleanupIt);
            } catch (const std::exception& e) {
                LOG_ERROR("ResourceManager", "Cleanup failed for " + id + ": " + e.what());
            }
        }

        resources_.erase(it);
        --resourceCount_;
        LOG_INFO("ResourceManager", "Removed resource: " + id);
        return true;
    }

    // Clear all resources
    void clearResources() {
        std::lock_guard<std::mutex> lock(resourceMutex_);

        LOG_INFO("ResourceManager", "Clearing " + std::to_string(resources_.size()) + " resources");

        // Run all cleanup callbacks
        for (auto& [id, cleanup] : cleanupCallbacks_) {
            try {
                cleanup();
            } catch (const std::exception& e) {
                LOG_ERROR("ResourceManager", "Cleanup failed for " + id + ": " + e.what());
            }
        }

        cleanupCallbacks_.clear();
        resources_.clear();
        resourceCount_ = 0;

        LOG_INFO("ResourceManager", "All resources cleared");
    }

    // Get resource count
    size_t getResourceCount() const {
        return resourceCount_.load();
    }

    // Check if resource exists
    bool hasResource(const std::string& id) const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        return resources_.find(id) != resources_.end();
    }

    // Get all resource IDs
    std::vector<std::string> getResourceIds() const {
        std::lock_guard<std::mutex> lock(resourceMutex_);
        std::vector<std::string> ids;
        ids.reserve(resources_.size());

        for (const auto& [id, resource] : resources_) {
            ids.push_back(id);
        }

        return ids;
    }
};

} // namespace Oroto

#endif // OROTO_RESOURCE_MANAGER_H