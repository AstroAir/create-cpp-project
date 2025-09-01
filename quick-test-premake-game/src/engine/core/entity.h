#pragma once

#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_map>

namespace quick-test-premake-game {

class Component;

class Entity {
public:
    Entity();
    virtual ~Entity();

    // Component management
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent();

    template<typename T>
    const T* GetComponent() const;

    template<typename T>
    void RemoveComponent();

    template<typename T>
    bool HasComponent() const;

    // Entity lifecycle
    virtual void Update(float deltaTime) {}
    virtual void Render() {}

    // Properties
    bool IsActive() const { return active_; }
    void SetActive(bool active) { active_ = active; }

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
    bool active_ = true;
};

} // namespace quick-test-premake-game
