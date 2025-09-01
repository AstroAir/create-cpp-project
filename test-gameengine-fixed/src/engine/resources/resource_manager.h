#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace test-gameengine-fixed {

template<typename T>
class Resource {
public:
    Resource(const std::string& path) : path_(path) {}
    virtual ~Resource() = default;

    const std::string& GetPath() const { return path_; }
    virtual bool Load() = 0;
    virtual void Unload() = 0;

private:
    std::string path_;
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    bool Initialize();
    void Shutdown();

    // Resource loading
    template<typename T>
    std::shared_ptr<T> LoadResource(const std::string& path);

    template<typename T>
    void UnloadResource(const std::string& path);

    void UnloadAllResources();

private:
    struct ResourceManagerImpl;
    std::unique_ptr<ResourceManagerImpl> impl_;
};

} // namespace test-gameengine-fixed
