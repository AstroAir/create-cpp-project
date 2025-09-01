#pragma once

namespace verify-premake-gameengine {

class Entity;

class Component {
public:
    Component() = default;
    virtual ~Component() = default;

    // Component lifecycle
    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    virtual void Shutdown() {}

    // Entity reference
    void SetOwner(Entity* owner) { owner_ = owner; }
    Entity* GetOwner() const { return owner_; }

protected:
    Entity* owner_ = nullptr;
};

// Example components
class TransformComponent : public Component {
public:
    TransformComponent(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : position{x, y, z}, rotation{0.0f, 0.0f, 0.0f}, scale{1.0f, 1.0f, 1.0f} {}

    float position[3];
    float rotation[3];
    float scale[3];
};

class RenderComponent : public Component {
public:
    RenderComponent(const std::string& meshPath = "", const std::string& texturePath = "")
        : meshPath_(meshPath), texturePath_(texturePath) {}

    void Render() override;

private:
    std::string meshPath_;
    std::string texturePath_;
};

} // namespace verify-premake-gameengine
