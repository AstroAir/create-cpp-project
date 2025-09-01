#pragma once

#include <memory>
#include <string>

namespace verify-premake-gameengine {

class Renderer;
class InputManager;
class ResourceManager;
class SceneManager;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const std::string& configPath = "config/engine.json");
    void Run();
    void Shutdown();

    // Subsystem access
    Renderer* GetRenderer() const { return renderer_.get(); }
    InputManager* GetInputManager() const { return inputManager_.get(); }
    ResourceManager* GetResourceManager() const { return resourceManager_.get(); }
    SceneManager* GetSceneManager() const { return sceneManager_.get(); }

    // Engine state
    bool IsRunning() const { return isRunning_; }
    void Stop() { isRunning_ = false; }

    // Time management
    float GetDeltaTime() const { return deltaTime_; }
    double GetTotalTime() const { return totalTime_; }

private:
    void Update(float deltaTime);
    void Render();

    bool isRunning_;
    float deltaTime_;
    double totalTime_;

    // Subsystems
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<InputManager> inputManager_;
    std::unique_ptr<ResourceManager> resourceManager_;
    std::unique_ptr<SceneManager> sceneManager_;
};

} // namespace verify-premake-gameengine
