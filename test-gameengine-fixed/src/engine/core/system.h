#pragma once

#include <vector>
#include <memory>

namespace test-gameengine-fixed {

class Entity;

class System {
public:
    System() = default;
    virtual ~System() = default;

    virtual void Initialize() {}
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() {}

    void AddEntity(std::shared_ptr<Entity> entity);
    void RemoveEntity(std::shared_ptr<Entity> entity);

protected:
    std::vector<std::shared_ptr<Entity>> entities_;
};

// Example systems
class RenderSystem : public System {
public:
    void Update(float deltaTime) override;
};

class PhysicsSystem : public System {
public:
    void Update(float deltaTime) override;
};

} // namespace test-gameengine-fixed
