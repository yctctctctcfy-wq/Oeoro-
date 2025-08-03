
#ifndef OROTO_PLUGIN_MANAGER_H
#define OROTO_PLUGIN_MANAGER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <dlfcn.h>
#include "logger.h"
#include "error_handler.h"

namespace Oroto {

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
};

// Plugin metadata
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string filename;
    void* handle;
    std::shared_ptr<IPlugin> instance;
    bool loaded;
    bool initialized;

    PluginInfo() : handle(nullptr), loaded(false), initialized(false) {}
};

// Plugin factory function type
typedef std::unique_ptr<IPlugin> (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(IPlugin*);

class PluginManager {
private:
    std::unordered_map<std::string, std::unique_ptr<PluginInfo>> plugins_;
    std::vector<std::string> search_paths_;
    mutable std::mutex plugin_mutex_;

public:
    PluginManager() {
        // Default search paths
        search_paths_.push_back("./plugins/");
        search_paths_.push_back("/usr/local/lib/oroto/plugins/");
        search_paths_.push_back("./tools/");
    }

    ~PluginManager() {
        unloadAllPlugins();
    }

    // Add search path for plugins
    void addSearchPath(const std::string& path) {
        std::lock_guard<std::mutex> lock(plugin_mutex_);
        search_paths_.push_back(path);
        LOG_INFO("PluginManager", "Added search path: " + path);
    }

    // Load plugin from file
    bool loadPlugin(const std::string& filename) {
        std::lock_guard<std::mutex> lock(plugin_mutex_);
        
        std::string full_path = findPluginFile(filename);
        if (full_path.empty()) {
            LOG_ERROR("PluginManager", "Plugin file not found: " + filename);
            return false;
        }

        // Check if already loaded
        if (plugins_.find(filename) != plugins_.end()) {
            LOG_WARNING("PluginManager", "Plugin already loaded: " + filename);
            return true;
        }

        auto plugin_info = std::make_unique<PluginInfo>();
        plugin_info->filename = filename;

        // Load shared library
        plugin_info->handle = dlopen(full_path.c_str(), RTLD_LAZY);
        if (!plugin_info->handle) {
            LOG_ERROR("PluginManager", "Failed to load plugin " + filename + ": " + dlerror());
            return false;
        }

        // Get factory function
        CreatePluginFunc create_func = (CreatePluginFunc)dlsym(plugin_info->handle, "createPlugin");
        if (!create_func) {
            LOG_ERROR("PluginManager", "Plugin " + filename + " missing createPlugin function");
            dlclose(plugin_info->handle);
            return false;
        }

        // Create plugin instance
        try {
            auto plugin_instance = create_func();
            if (!plugin_instance) {
                LOG_ERROR("PluginManager", "Failed to create plugin instance: " + filename);
                dlclose(plugin_info->handle);
                return false;
            }

            plugin_info->instance = std::move(plugin_instance);
            plugin_info->name = plugin_info->instance->getName();
            plugin_info->version = plugin_info->instance->getVersion();
            plugin_info->description = plugin_info->instance->getDescription();
            plugin_info->loaded = true;

            // Initialize plugin
            if (plugin_info->instance->initialize()) {
                plugin_info->initialized = true;
                LOG_INFO("PluginManager", "Successfully loaded and initialized plugin: " + 
                        plugin_info->name + " v" + plugin_info->version);
            } else {
                LOG_WARNING("PluginManager", "Plugin loaded but failed to initialize: " + filename);
            }

            plugins_[filename] = std::move(plugin_info);
            return true;

        } catch (const std::exception& e) {
            LOG_ERROR("PluginManager", "Exception while loading plugin " + filename + ": " + e.what());
            dlclose(plugin_info->handle);
            return false;
        }
    }

    // Unload plugin
    bool unloadPlugin(const std::string& filename) {
        std::lock_guard<std::mutex> lock(plugin_mutex_);
        
        auto it = plugins_.find(filename);
        if (it == plugins_.end()) {
            LOG_WARNING("PluginManager", "Plugin not loaded: " + filename);
            return false;
        }

        auto& plugin_info = it->second;
        
        try {
            if (plugin_info->initialized && plugin_info->instance) {
                plugin_info->instance->shutdown();
                plugin_info->initialized = false;
            }

            if (plugin_info->handle) {
                dlclose(plugin_info->handle);
                plugin_info->handle = nullptr;
            }

            LOG_INFO("PluginManager", "Unloaded plugin: " + filename);
            plugins_.erase(it);
            return true;

        } catch (const std::exception& e) {
            LOG_ERROR("PluginManager", "Exception while unloading plugin " + filename + ": " + e.what());
            return false;
        }
    }

    // Get plugin by name
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) const {
        std::lock_guard<std::mutex> lock(plugin_mutex_);
        
        for (const auto& [filename, plugin_info] : plugins_) {
            if (plugin_info->name == name && plugin_info->initialized) {
                return plugin_info->instance;
            }
        }
        
        return nullptr;
    }

    // List all loaded plugins
    std::vector<PluginInfo> listPlugins() const {
        std::lock_guard<std::mutex> lock(plugin_mutex_);
        
        std::vector<PluginInfo> result;
        for (const auto& [filename, plugin_info] : plugins_) {
            PluginInfo info;
            info.name = plugin_info->name;
            info.version = plugin_info->version;
            info.description = plugin_info->description;
            info.filename = plugin_info->filename;
            info.loaded = plugin_info->loaded;
            info.initialized = plugin_info->initialized;
            result.push_back(info);
        }
        
        return result;
    }

    // Unload all plugins
    void unloadAllPlugins() {
        std::lock_guard<std::mutex> lock(plugin_mutex_);
        
        LOG_INFO("PluginManager", "Unloading all plugins");
        
        for (auto& [filename, plugin_info] : plugins_) {
            try {
                if (plugin_info->initialized && plugin_info->instance) {
                    plugin_info->instance->shutdown();
                }
                if (plugin_info->handle) {
                    dlclose(plugin_info->handle);
                }
            } catch (const std::exception& e) {
                LOG_ERROR("PluginManager", "Error unloading plugin " + filename + ": " + e.what());
            }
        }
        
        plugins_.clear();
    }

private:
    std::string findPluginFile(const std::string& filename) const {
        // Try absolute path first
        if (filename[0] == '/') {
            return filename;
        }
        
        // Search in all search paths
        for (const auto& path : search_paths_) {
            std::string full_path = path + "/" + filename;
            if (access(full_path.c_str(), F_OK) == 0) {
                return full_path;
            }
        }
        
        return "";
    }
};

} // namespace Oroto

// Macros for plugin creation
#define OROTO_PLUGIN_EXPORT extern "C"

#define DECLARE_PLUGIN(ClassName) \
    OROTO_PLUGIN_EXPORT std::unique_ptr<Oroto::IPlugin> createPlugin() { \
        return std::make_unique<ClassName>(); \
    } \
    OROTO_PLUGIN_EXPORT void destroyPlugin(Oroto::IPlugin* plugin) { \
        delete plugin; \
    }

#endif // OROTO_PLUGIN_MANAGER_H
