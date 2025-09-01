#pragma once

#include <memory>
#include <vector>
#include <string>

namespace quick-test-premake-game {

class Entity;
class Renderer;

class Scene {
public:
    Scene(const std::string& name);
    virtual ~Scene();

    const std::string& GetName() const { return name_; }

    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(Renderer* renderer) {}
    virtual void Shutdown() {}

    void AddEntity(std::shared_ptr<Entity> entity);
    void RemoveEntity(std::shared_ptr<Entity> entity);

private:
    std::string name_;
    std::vector<std::shared_ptr<Entity>> entities_;
};

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    void Render(Renderer* renderer);

    void LoadScene(std::shared_ptr<Scene> scene);
    void UnloadCurrentScene();
    Scene* GetCurrentScene() const;

private:
    struct SceneManagerImpl;
    std::unique_ptr<SceneManagerImpl> impl_;
};

} // namespace quick-test-premake-game
