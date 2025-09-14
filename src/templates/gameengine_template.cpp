#include "gameengine_template.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <filesystem>

#include "../utils/core/file_utils.h"
#include "../utils/ui/terminal_utils.h"

using utils::FileUtils;

GameEngineTemplate::GameEngineTemplate(const CliOptions& options) : TemplateBase(options) {}

bool GameEngineTemplate::create() {
    spdlog::info("Creating game engine project: {}", options_.projectName);

    // Show progress
    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 0, "Initializing");

    if (!createProjectStructure()) {
        spdlog::error("Failed to create project structure");
        return false;
    }

    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 20,
                                               "Setting up graphics framework");

    if (!setupGraphicsFramework()) {
        spdlog::error("Failed to setup graphics framework");
        return false;
    }

    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 40,
                                               "Configuring build system");

    if (!createBuildSystem()) {
        spdlog::error("Failed to create build system");
        return false;
    }

    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 60,
                                               "Setting up package manager");

    if (!setupPackageManager()) {
        spdlog::error("Failed to setup package manager");
        return false;
    }

    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 80,
                                               "Configuring testing framework");

    if (options_.includeTests && !setupTestFramework()) {
        spdlog::error("Failed to setup test framework");
        return false;
    }

    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 90,
                                               "Setting up asset pipeline");

    if (!setupAssetPipeline()) {
        spdlog::error("Failed to setup asset pipeline");
        return false;
    }

    utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 100, "Finalizing");

    // Initialize Git if requested
    if (options_.initGit) {
        if (!initializeGit(options_.projectName)) {
            spdlog::error("Failed to initialize Git repository");
            return false;
        }
        spdlog::info("�?Git repository initialized");
    }

    utils::TerminalUtils::showNpmStyleSuccess("Game engine project created successfully!",
                                              options_.projectName);

    // Print usage guide
    printUsageGuide();

    return true;
}

bool GameEngineTemplate::createProjectStructure() {
    std::string projectPath = options_.projectName;

    // Create main directories
    std::vector<std::string> directories = {projectPath,
                                            projectPath + "/src",
                                            projectPath + "/src/engine",
                                            projectPath + "/src/engine/core",
                                            projectPath + "/src/engine/graphics",
                                            projectPath + "/src/engine/audio",
                                            projectPath + "/src/engine/input",
                                            projectPath + "/src/engine/physics",
                                            projectPath + "/src/engine/scripting",
                                            projectPath + "/src/engine/resources",
                                            projectPath + "/src/engine/scene",
                                            projectPath + "/src/game",
                                            projectPath + "/include",
                                            projectPath + "/include/engine",
                                            projectPath + "/include/game",
                                            projectPath + "/assets",
                                            projectPath + "/assets/textures",
                                            projectPath + "/assets/models",
                                            projectPath + "/assets/sounds",
                                            projectPath + "/assets/shaders",
                                            projectPath + "/assets/fonts",
                                            projectPath + "/assets/scenes",
                                            projectPath + "/config",
                                            projectPath + "/tools",
                                            projectPath + "/tools/asset_pipeline",
                                            projectPath + "/tools/shader_compiler",
                                            projectPath + "/examples",
                                            projectPath + "/docs",
                                            projectPath + "/tests",
                                            projectPath + "/tests/engine",
                                            projectPath + "/tests/game",
                                            projectPath + "/build",
                                            projectPath + "/bin",
                                            projectPath + "/lib"};

    for (const auto& dir : directories) {
        if (!FileUtils::createDirectory(dir)) {
            spdlog::error("Failed to create directory: {}", dir);
            return false;
        }
    }

    // Create main engine files
    if (!FileUtils::writeToFile(projectPath + "/src/engine/core/engine.h",
                                getEngineHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/core/engine.cpp",
                                getEngineSourceContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/game/game.h", getGameHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/game/game.cpp", getGameSourceContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/main.cpp", getExampleGameContent())) {
        return false;
    }

    // Create graphics system files
    if (!FileUtils::writeToFile(projectPath + "/src/engine/graphics/renderer.h",
                                getRendererHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/graphics/renderer.cpp",
                                getRendererSourceContent())) {
        return false;
    }

    // Create input system files
    if (!FileUtils::writeToFile(projectPath + "/src/engine/input/input_manager.h",
                                getInputManagerHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/input/input_manager.cpp",
                                getInputManagerSourceContent())) {
        return false;
    }

    // Create resource manager files
    if (!FileUtils::writeToFile(projectPath + "/src/engine/resources/resource_manager.h",
                                getResourceManagerHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/resources/resource_manager.cpp",
                                getResourceManagerSourceContent())) {
        return false;
    }

    // Create scene system files
    if (!FileUtils::writeToFile(projectPath + "/src/engine/scene/scene_manager.h",
                                getSceneManagerHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/scene/scene_manager.cpp",
                                getSceneManagerSourceContent())) {
        return false;
    }

    // Create ECS files
    if (!FileUtils::writeToFile(projectPath + "/src/engine/core/entity.h",
                                getEntityHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/core/entity.cpp",
                                getEntitySourceContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/core/component.h",
                                getComponentHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/src/engine/core/system.h",
                                getSystemHeaderContent())) {
        return false;
    }

    // Create basic shaders
    if (!FileUtils::writeToFile(projectPath + "/assets/shaders/vertex.glsl",
                                getShaderContent("vertex"))) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/assets/shaders/fragment.glsl",
                                getShaderContent("fragment"))) {
        return false;
    }

    // Create configuration files
    if (!FileUtils::writeToFile(projectPath + "/config/engine.json", getEngineConfigContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(projectPath + "/config/graphics.json",
                                getGraphicsConfigContent())) {
        return false;
    }

    // Create README
    if (!FileUtils::writeToFile(projectPath + "/README.md", getGameEngineReadmeContent())) {
        return false;
    }

    // Create assets README
    if (!FileUtils::writeToFile(projectPath + "/assets/README.md", getAssetsReadmeContent())) {
        return false;
    }

    return true;
}

bool GameEngineTemplate::createBuildSystem() {
    std::string projectPath = options_.projectName;

    switch (options_.buildSystem) {
        case BuildSystem::CMake:
            return FileUtils::writeToFile(projectPath + "/CMakeLists.txt",
                                          getGameEngineCMakeContent());
        case BuildSystem::Meson:
            return FileUtils::writeToFile(projectPath + "/meson.build",
                                          getGameEngineMesonContent());
        case BuildSystem::Bazel:
            return FileUtils::writeToFile(projectPath + "/BUILD.bazel",
                                          getGameEngineBazelContent());
        case BuildSystem::XMake:
            return FileUtils::writeToFile(projectPath + "/xmake.lua", getGameEngineXMakeContent());
        case BuildSystem::Premake:
            return FileUtils::writeToFile(projectPath + "/premake5.lua",
                                          getGameEnginePremakeContent());
        default:
            spdlog::error("Unsupported build system for game engine template");
            return false;
    }
}

bool GameEngineTemplate::setupPackageManager() {
    std::string projectPath = options_.projectName;

    switch (options_.packageManager) {
        case PackageManager::Vcpkg:
            return FileUtils::writeToFile(projectPath + "/vcpkg.json", getGameEngineVcpkgContent());
        case PackageManager::Conan:
            return FileUtils::writeToFile(projectPath + "/conanfile.txt",
                                          getGameEngineConanContent());
        case PackageManager::None:
            return true;  // No package manager setup needed
        default:
            spdlog::error("Unsupported package manager for game engine template");
            return false;
    }
}

bool GameEngineTemplate::setupTestFramework() {
    // Use the base class implementation for now
    return true;
}

bool GameEngineTemplate::setupGraphicsFramework() {
    // Graphics framework setup is handled in the build system and package manager
    return true;
}

bool GameEngineTemplate::setupAudioSystem() {
    // Audio system setup
    return true;
}

bool GameEngineTemplate::setupInputSystem() {
    // Input system setup
    return true;
}

bool GameEngineTemplate::setupResourceManager() {
    // Resource manager setup
    return true;
}

bool GameEngineTemplate::setupSceneSystem() {
    // Scene system setup
    return true;
}

bool GameEngineTemplate::setupPhysicsEngine() {
    // Physics engine setup
    return true;
}

bool GameEngineTemplate::setupScriptingEngine() {
    // Scripting engine setup
    return true;
}

bool GameEngineTemplate::setupAssetPipeline() {
    std::string projectPath = options_.projectName;

    // Create asset pipeline script
    if (!FileUtils::writeToFile(projectPath + "/tools/asset_pipeline/process_assets.py",
                                getAssetPipelineScriptContent())) {
        return false;
    }

    // Create shader compiler script
    if (!FileUtils::writeToFile(projectPath + "/tools/shader_compiler/compile_shaders.py",
                                getShaderCompilerScriptContent())) {
        return false;
    }

    return true;
}

// Content generation methods
std::string GameEngineTemplate::getEngineHeaderContent() {
    return fmt::format(R"(#pragma once

#include <memory>
#include <string>

namespace {} {{

class Renderer;
class InputManager;
class ResourceManager;
class SceneManager;

class Engine {{
public:
    Engine();
    ~Engine();

    bool Initialize(const std::string& configPath = "config/engine.json");
    void Run();
    void Shutdown();

    // Subsystem access
    Renderer* GetRenderer() const {{ return renderer_.get(); }}
    InputManager* GetInputManager() const {{ return inputManager_.get(); }}
    ResourceManager* GetResourceManager() const {{ return resourceManager_.get(); }}
    SceneManager* GetSceneManager() const {{ return sceneManager_.get(); }}

    // Engine state
    bool IsRunning() const {{ return isRunning_; }}
    void Stop() {{ isRunning_ = false; }}

    // Time management
    float GetDeltaTime() const {{ return deltaTime_; }}
    double GetTotalTime() const {{ return totalTime_; }}

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
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getEngineSourceContent() {
    return fmt::format(R"(#include "engine.h"
#include "../graphics/renderer.h"
#include "../input/input_manager.h"
#include "../resources/resource_manager.h"
#include "../scene/scene_manager.h"
#include <chrono>
#include <iostream>

namespace {} {{

Engine::Engine()
    : isRunning_(false), deltaTime_(0.0f), totalTime_(0.0) {{
}}

Engine::~Engine() {{
    Shutdown();
}}

bool Engine::Initialize(const std::string& configPath) {{
    std::cout << "Initializing {} Engine..." << std::endl;

    // Initialize subsystems
    renderer_ = std::make_unique<Renderer>();
    if (!renderer_->Initialize()) {{
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }}

    inputManager_ = std::make_unique<InputManager>();
    if (!inputManager_->Initialize()) {{
        std::cerr << "Failed to initialize input manager" << std::endl;
        return false;
    }}

    resourceManager_ = std::make_unique<ResourceManager>();
    if (!resourceManager_->Initialize()) {{
        std::cerr << "Failed to initialize resource manager" << std::endl;
        return false;
    }}

    sceneManager_ = std::make_unique<SceneManager>();
    if (!sceneManager_->Initialize()) {{
        std::cerr << "Failed to initialize scene manager" << std::endl;
        return false;
    }}

    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}}

void Engine::Run() {{
    isRunning_ = true;

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (isRunning_) {{
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime);
        deltaTime_ = duration.count() / 1000000.0f;
        totalTime_ += deltaTime_;
        lastTime = currentTime;

        // Update input
        inputManager_->Update();

        // Check for exit condition
        if (inputManager_->IsKeyPressed("Escape")) {{
            Stop();
        }}

        // Update game logic
        Update(deltaTime_);

        // Render frame
        Render();
    }}
}}

void Engine::Update(float deltaTime) {{
    sceneManager_->Update(deltaTime);
}}

void Engine::Render() {{
    renderer_->BeginFrame();
    sceneManager_->Render(renderer_.get());
    renderer_->EndFrame();
}}

void Engine::Shutdown() {{
    std::cout << "Shutting down engine..." << std::endl;

    sceneManager_.reset();
    resourceManager_.reset();
    inputManager_.reset();
    renderer_.reset();

    std::cout << "Engine shutdown complete." << std::endl;
}}

}} // namespace {}
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameHeaderContent() {
    return fmt::format(R"(#pragma once

#include "engine/core/engine.h"

namespace {} {{

class Game {{
public:
    Game();
    ~Game();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void Update(float deltaTime);
    void Render();
    void HandleInput();

    Engine engine_;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameSourceContent() {
    return fmt::format(R"(#include "game.h"
#include <iostream>

namespace {} {{

Game::Game() {{
}}

Game::~Game() {{
    Shutdown();
}}

bool Game::Initialize() {{
    std::cout << "Initializing game..." << std::endl;

    if (!engine_.Initialize()) {{
        std::cerr << "Failed to initialize engine" << std::endl;
        return false;
    }}

    // Initialize game-specific systems here

    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}}

void Game::Run() {{
    engine_.Run();
}}

void Game::Update(float deltaTime) {{
    // Game-specific update logic here
}}

void Game::Render() {{
    // Game-specific rendering here
}}

void Game::HandleInput() {{
    // Game-specific input handling here
}}

void Game::Shutdown() {{
    std::cout << "Shutting down game..." << std::endl;
    engine_.Shutdown();
}}

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getExampleGameContent() {
    return fmt::format(R"(#include "game/game.h"
#include <iostream>

int main() {{
    std::cout << "Starting {} Game Engine..." << std::endl;

    {}::Game game;

    if (!game.Initialize()) {{
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }}

    game.Run();
    game.Shutdown();

    std::cout << "Game exited successfully." << std::endl;
    return 0;
}}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getRendererHeaderContent() {
    return fmt::format(R"(#pragma once

#include <memory>
#include <string>

namespace {} {{

class Renderer {{
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void Clear();
    void Present();

    // Basic rendering functions
    void SetViewport(int x, int y, int width, int height);
    void SetClearColor(float r, float g, float b, float a = 1.0f);

private:
    struct RendererImpl;
    std::unique_ptr<RendererImpl> impl_;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getRendererSourceContent() {
    return fmt::format(R"(#include "renderer.h"
#include <iostream>

namespace {} {{

struct Renderer::RendererImpl {{
    bool initialized = false;
    float clearColor[4] = {{0.0f, 0.0f, 0.0f, 1.0f}};
}};

Renderer::Renderer() : impl_(std::make_unique<RendererImpl>()) {{
}}

Renderer::~Renderer() {{
    Shutdown();
}}

bool Renderer::Initialize() {{
    std::cout << "Initializing renderer..." << std::endl;

    // TODO: Initialize graphics API (OpenGL, Vulkan, etc.)
    impl_->initialized = true;

    std::cout << "Renderer initialized successfully" << std::endl;
    return true;
}}

void Renderer::Shutdown() {{
    if (impl_->initialized) {{
        std::cout << "Shutting down renderer..." << std::endl;
        impl_->initialized = false;
    }}
}}

void Renderer::BeginFrame() {{
    if (!impl_->initialized) return;

    Clear();
}}

void Renderer::EndFrame() {{
    if (!impl_->initialized) return;

    Present();
}}

void Renderer::Clear() {{
    // TODO: Clear the framebuffer
}}

void Renderer::Present() {{
    // TODO: Present the rendered frame
}}

void Renderer::SetViewport(int x, int y, int width, int height) {{
    // TODO: Set viewport
    (void)x; (void)y; (void)width; (void)height;
}}

void Renderer::SetClearColor(float r, float g, float b, float a) {{
    impl_->clearColor[0] = r;
    impl_->clearColor[1] = g;
    impl_->clearColor[2] = b;
    impl_->clearColor[3] = a;
}}

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getInputManagerHeaderContent() {
    return fmt::format(R"(#pragma once

#include <string>
#include <unordered_map>

namespace {} {{

class InputManager {{
public:
    InputManager();
    ~InputManager();

    bool Initialize();
    void Shutdown();
    void Update();

    // Keyboard input
    bool IsKeyPressed(const std::string& key) const;
    bool IsKeyDown(const std::string& key) const;
    bool IsKeyUp(const std::string& key) const;

    // Mouse input
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonDown(int button) const;
    bool IsMouseButtonUp(int button) const;
    void GetMousePosition(int& x, int& y) const;

private:
    struct InputManagerImpl;
    std::unique_ptr<InputManagerImpl> impl_;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getInputManagerSourceContent() {
    return fmt::format(R"(#include "input_manager.h"
#include <iostream>

namespace {} {{

struct InputManager::InputManagerImpl {{
    std::unordered_map<std::string, bool> keyStates;
    std::unordered_map<std::string, bool> prevKeyStates;
    std::unordered_map<int, bool> mouseButtonStates;
    std::unordered_map<int, bool> prevMouseButtonStates;
    int mouseX = 0, mouseY = 0;
    bool initialized = false;
}};

InputManager::InputManager() : impl_(std::make_unique<InputManagerImpl>()) {{
}}

InputManager::~InputManager() {{
    Shutdown();
}}

bool InputManager::Initialize() {{
    std::cout << "Initializing input manager..." << std::endl;

    // TODO: Initialize input system
    impl_->initialized = true;

    std::cout << "Input manager initialized successfully" << std::endl;
    return true;
}}

void InputManager::Shutdown() {{
    if (impl_->initialized) {{
        std::cout << "Shutting down input manager..." << std::endl;
        impl_->initialized = false;
    }}
}}

void InputManager::Update() {{
    if (!impl_->initialized) return;

    // Store previous frame states
    impl_->prevKeyStates = impl_->keyStates;
    impl_->prevMouseButtonStates = impl_->mouseButtonStates;

    // TODO: Poll input events and update current states
}}

bool InputManager::IsKeyPressed(const std::string& key) const {{
    auto current = impl_->keyStates.find(key);
    auto previous = impl_->prevKeyStates.find(key);

    bool currentState = (current != impl_->keyStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevKeyStates.end()) ? previous->second : false;

    return currentState && !previousState;
}}

bool InputManager::IsKeyDown(const std::string& key) const {{
    auto it = impl_->keyStates.find(key);
    return (it != impl_->keyStates.end()) ? it->second : false;
}}

bool InputManager::IsKeyUp(const std::string& key) const {{
    auto current = impl_->keyStates.find(key);
    auto previous = impl_->prevKeyStates.find(key);

    bool currentState = (current != impl_->keyStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevKeyStates.end()) ? previous->second : false;

    return !currentState && previousState;
}}

bool InputManager::IsMouseButtonPressed(int button) const {{
    auto current = impl_->mouseButtonStates.find(button);
    auto previous = impl_->prevMouseButtonStates.find(button);

    bool currentState = (current != impl_->mouseButtonStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevMouseButtonStates.end()) ? previous->second : false;

    return currentState && !previousState;
}}

bool InputManager::IsMouseButtonDown(int button) const {{
    auto it = impl_->mouseButtonStates.find(button);
    return (it != impl_->mouseButtonStates.end()) ? it->second : false;
}}

bool InputManager::IsMouseButtonUp(int button) const {{
    auto current = impl_->mouseButtonStates.find(button);
    auto previous = impl_->prevMouseButtonStates.find(button);

    bool currentState = (current != impl_->mouseButtonStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevMouseButtonStates.end()) ? previous->second : false;

    return !currentState && previousState;
}}

void InputManager::GetMousePosition(int& x, int& y) const {{
    x = impl_->mouseX;
    y = impl_->mouseY;
}}

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getResourceManagerHeaderContent() {
    return fmt::format(R"(#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace {} {{

template<typename T>
class Resource {{
public:
    Resource(const std::string& path) : path_(path) {{}}
    virtual ~Resource() = default;

    const std::string& GetPath() const {{ return path_; }}
    virtual bool Load() = 0;
    virtual void Unload() = 0;

private:
    std::string path_;
}};

class ResourceManager {{
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
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getResourceManagerSourceContent() {
    return fmt::format(R"(#include "resource_manager.h"
#include <iostream>

namespace {} {{

struct ResourceManager::ResourceManagerImpl {{
    std::unordered_map<std::string, std::shared_ptr<void>> resources;
    bool initialized = false;
}};

ResourceManager::ResourceManager() : impl_(std::make_unique<ResourceManagerImpl>()) {{
}}

ResourceManager::~ResourceManager() {{
    Shutdown();
}}

bool ResourceManager::Initialize() {{
    std::cout << "Initializing resource manager..." << std::endl;

    impl_->initialized = true;

    std::cout << "Resource manager initialized successfully" << std::endl;
    return true;
}}

void ResourceManager::Shutdown() {{
    if (impl_->initialized) {{
        std::cout << "Shutting down resource manager..." << std::endl;
        UnloadAllResources();
        impl_->initialized = false;
    }}
}}

void ResourceManager::UnloadAllResources() {{
    impl_->resources.clear();
    std::cout << "All resources unloaded" << std::endl;
}}

// Template specializations would go here in a real implementation

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getSceneManagerHeaderContent() {
    return fmt::format(R"(#pragma once

#include <memory>
#include <vector>
#include <string>

namespace {} {{

class Entity;
class Renderer;

class Scene {{
public:
    Scene(const std::string& name);
    virtual ~Scene();

    const std::string& GetName() const {{ return name_; }}

    virtual void Initialize() {{}}
    virtual void Update(float deltaTime) {{}}
    virtual void Render(Renderer* renderer) {{}}
    virtual void Shutdown() {{}}

    void AddEntity(std::shared_ptr<Entity> entity);
    void RemoveEntity(std::shared_ptr<Entity> entity);

private:
    std::string name_;
    std::vector<std::shared_ptr<Entity>> entities_;
}};

class SceneManager {{
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
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getSceneManagerSourceContent() {
    return fmt::format(R"(#include "scene_manager.h"
#include "entity.h"
#include "../graphics/renderer.h"
#include <iostream>
#include <algorithm>

namespace {} {{

Scene::Scene(const std::string& name) : name_(name) {{
}}

Scene::~Scene() {{
    Shutdown();
}}

void Scene::AddEntity(std::shared_ptr<Entity> entity) {{
    entities_.push_back(entity);
}}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity) {{
    auto it = std::find(entities_.begin(), entities_.end(), entity);
    if (it != entities_.end()) {{
        entities_.erase(it);
    }}
}}

struct SceneManager::SceneManagerImpl {{
    std::shared_ptr<Scene> currentScene;
    bool initialized = false;
}};

SceneManager::SceneManager() : impl_(std::make_unique<SceneManagerImpl>()) {{
}}

SceneManager::~SceneManager() {{
    Shutdown();
}}

bool SceneManager::Initialize() {{
    std::cout << "Initializing scene manager..." << std::endl;

    impl_->initialized = true;

    std::cout << "Scene manager initialized successfully" << std::endl;
    return true;
}}

void SceneManager::Shutdown() {{
    if (impl_->initialized) {{
        std::cout << "Shutting down scene manager..." << std::endl;
        UnloadCurrentScene();
        impl_->initialized = false;
    }}
}}

void SceneManager::Update(float deltaTime) {{
    if (impl_->currentScene) {{
        impl_->currentScene->Update(deltaTime);
    }}
}}

void SceneManager::Render(Renderer* renderer) {{
    if (impl_->currentScene) {{
        impl_->currentScene->Render(renderer);
    }}
}}

void SceneManager::LoadScene(std::shared_ptr<Scene> scene) {{
    if (impl_->currentScene) {{
        impl_->currentScene->Shutdown();
    }}

    impl_->currentScene = scene;

    if (impl_->currentScene) {{
        impl_->currentScene->Initialize();
        std::cout << "Loaded scene: " << impl_->currentScene->GetName() << std::endl;
    }}
}}

void SceneManager::UnloadCurrentScene() {{
    if (impl_->currentScene) {{
        impl_->currentScene->Shutdown();
        impl_->currentScene.reset();
        std::cout << "Unloaded current scene" << std::endl;
    }}
}}

Scene* SceneManager::GetCurrentScene() const {{
    return impl_->currentScene.get();
}}

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getEntityHeaderContent() {
    return fmt::format(R"(#pragma once

#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_map>

namespace {} {{

class Component;

class Entity {{
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
    virtual void Update(float deltaTime) {{}}
    virtual void Render() {{}}

    // Properties
    bool IsActive() const {{ return active_; }}
    void SetActive(bool active) {{ active_ = active; }}

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
    bool active_ = true;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getEntitySourceContent() {
    return fmt::format(R"(#include "entity.h"
#include "component.h"

namespace {} {{

Entity::Entity() {{
}}

Entity::~Entity() {{
    components_.clear();
}}

// Template method implementations would typically go in the header file
// for template instantiation, but here's the basic structure

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getComponentHeaderContent() {
    return fmt::format(R"(#pragma once

namespace {} {{

class Entity;

class Component {{
public:
    Component() = default;
    virtual ~Component() = default;

    // Component lifecycle
    virtual void Initialize() {{}}
    virtual void Update(float deltaTime) {{}}
    virtual void Render() {{}}
    virtual void Shutdown() {{}}

    // Entity reference
    void SetOwner(Entity* owner) {{ owner_ = owner; }}
    Entity* GetOwner() const {{ return owner_; }}

protected:
    Entity* owner_ = nullptr;
}};

// Example components
class TransformComponent : public Component {{
public:
    TransformComponent(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : position{{x, y, z}}, rotation{{0.0f, 0.0f, 0.0f}}, scale{{1.0f, 1.0f, 1.0f}} {{}}

    float position[3];
    float rotation[3];
    float scale[3];
}};

class RenderComponent : public Component {{
public:
    RenderComponent(const std::string& meshPath = "", const std::string& texturePath = "")
        : meshPath_(meshPath), texturePath_(texturePath) {{}}

    void Render() override;

private:
    std::string meshPath_;
    std::string texturePath_;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getSystemHeaderContent() {
    return fmt::format(R"(#pragma once

#include <vector>
#include <memory>

namespace {} {{

class Entity;

class System {{
public:
    System() = default;
    virtual ~System() = default;

    virtual void Initialize() {{}}
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() {{}}

    void AddEntity(std::shared_ptr<Entity> entity);
    void RemoveEntity(std::shared_ptr<Entity> entity);

protected:
    std::vector<std::shared_ptr<Entity>> entities_;
}};

// Example systems
class RenderSystem : public System {{
public:
    void Update(float deltaTime) override;
}};

class PhysicsSystem : public System {{
public:
    void Update(float deltaTime) override;
}};

}} // namespace {}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getShaderContent(const std::string& shaderType) {
    if (shaderType == "vertex") {
        return R"(#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";
    } else if (shaderType == "fragment") {
        return R"(#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 color;

void main()
{
    FragColor = texture(ourTexture, TexCoord) * vec4(color, 1.0);
}
)";
    }
    return "// Unknown shader type";
}

std::string GameEngineTemplate::getEngineConfigContent() {
    return fmt::format(R"({{
  "engine": {{
    "name": "{}",
    "version": "1.0.0",
    "window": {{
      "title": "{} Game Engine",
      "width": 1280,
      "height": 720,
      "fullscreen": false,
      "vsync": true
    }},
    "renderer": {{
      "api": "OpenGL",
      "version": "3.3",
      "debug": true
    }},
    "audio": {{
      "enabled": true,
      "masterVolume": 1.0,
      "channels": 32
    }},
    "physics": {{
      "enabled": true,
      "gravity": [0.0, -9.81, 0.0],
      "timestep": 0.016667
    }}
  }}
}}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGraphicsConfigContent() {
    return R"({
  "graphics": {
    "clearColor": [0.2, 0.3, 0.3, 1.0],
    "wireframe": false,
    "culling": {
      "enabled": true,
      "face": "back"
    },
    "depth": {
      "test": true,
      "write": true
    },
    "blending": {
      "enabled": false,
      "srcFactor": "SRC_ALPHA",
      "dstFactor": "ONE_MINUS_SRC_ALPHA"
    },
    "multisampling": {
      "enabled": true,
      "samples": 4
    }
  }
}
)";
}

std::string GameEngineTemplate::getAssetsReadmeContent() {
    return fmt::format(R"(# {} Assets

This directory contains all the assets for the {} game engine project.

## Directory Structure

```
assets/
├── textures/       # Texture files (.png, .jpg, .tga, etc.)
├── models/         # 3D model files (.obj, .fbx, .gltf, etc.)
├── sounds/         # Audio files (.wav, .ogg, .mp3, etc.)
├── music/          # Background music files
├── shaders/        # Shader programs (.glsl, .hlsl, etc.)
├── fonts/          # Font files (.ttf, .otf, etc.)
├── scenes/         # Scene definition files (.json, .xml, etc.)
├── scripts/        # Game scripts (.lua, .py, etc.)
└── config/         # Configuration files

```

## Asset Guidelines

### Textures
- Use power-of-2 dimensions when possible (256x256, 512x512, 1024x1024, etc.)
- Prefer PNG for images with transparency
- Use JPG for photographs and images without transparency
- Keep texture sizes reasonable for target platforms

### Models
- Use consistent units (meters recommended)
- Optimize polygon count for target performance
- Include proper UV mapping
- Export with materials when possible

### Audio
- Use uncompressed formats (WAV) for short sound effects
- Use compressed formats (OGG) for longer audio files
- Normalize audio levels
- Use appropriate sample rates (44.1kHz for most cases)

### Shaders
- Follow consistent naming conventions
- Include comments explaining complex operations
- Test on target graphics APIs
- Provide fallback shaders for older hardware

## Asset Pipeline

The project includes an automated asset pipeline that:
- Validates asset formats
- Optimizes assets for target platforms
- Generates asset manifests
- Handles asset dependencies

Run the asset pipeline with:
```bash
python tools/asset_pipeline/process_assets.py
```

## Adding New Assets

1. Place assets in the appropriate subdirectory
2. Update any relevant configuration files
3. Run the asset pipeline to process new assets
4. Test assets in the engine

## Asset Naming Conventions

- Use lowercase with underscores: `player_texture.png`
- Include descriptive names: `explosion_sound_01.wav`
- Use consistent prefixes for related assets: `ui_button_normal.png`, `ui_button_hover.png`
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameEngineCMakeContent() {
    return fmt::format(R"(cmake_minimum_required(VERSION 3.16)
project({} VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(ENABLE_GRAPHICS_DEBUG "Enable graphics debugging" ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Engine library
set(ENGINE_SOURCES
    src/engine/core/engine.cpp
    src/engine/graphics/renderer.cpp
    src/engine/input/input_manager.cpp
    src/engine/resources/resource_manager.cpp
    src/engine/scene/scene_manager.cpp
    src/engine/core/entity.cpp
    src/game/game.cpp
)

set(ENGINE_HEADERS
    src/engine/core/engine.h
    src/engine/graphics/renderer.h
    src/engine/input/input_manager.h
    src/engine/resources/resource_manager.h
    src/engine/scene/scene_manager.h
    src/engine/core/entity.h
    src/engine/core/component.h
    src/engine/core/system.h
    src/game/game.h
)

add_library({}_engine STATIC ${{ENGINE_SOURCES}} ${{ENGINE_HEADERS}})

target_include_directories({}_engine PUBLIC
    $<BUILD_INTERFACE:${{CMAKE_CURRENT_SOURCE_DIR}}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries({}_engine PUBLIC
    OpenGL::GL
    glfw
    glm::glm
)

if(ENABLE_GRAPHICS_DEBUG)
    target_compile_definitions({}_engine PUBLIC GRAPHICS_DEBUG)
endif()

# Main executable
add_executable({} src/main.cpp)
target_link_libraries({} PRIVATE {}_engine)

# Set output directories
set_target_properties({} {}_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${{CMAKE_BINARY_DIR}}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${{CMAKE_BINARY_DIR}}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${{CMAKE_BINARY_DIR}}/lib"
)

# Copy assets to build directory
file(COPY assets DESTINATION ${{CMAKE_BINARY_DIR}})
file(COPY config DESTINATION ${{CMAKE_BINARY_DIR}})

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options({}_engine PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options({}_engine PRIVATE /W4)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameEngineMesonContent() {
    return fmt::format(R"(project('{}', 'cpp',
  version : '1.0.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

# Dependencies
opengl_dep = dependency('opengl')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

# Engine library sources
engine_sources = [
  'src/engine/core/engine.cpp',
  'src/engine/graphics/renderer.cpp',
  'src/engine/input/input_manager.cpp',
  'src/engine/resources/resource_manager.cpp',
  'src/engine/scene/scene_manager.cpp',
  'src/engine/core/entity.cpp',
  'src/game/game.cpp'
]

# Engine library
engine_lib = static_library('{}_engine',
  engine_sources,
  include_directories : include_directories('src'),
  dependencies : [opengl_dep, glfw_dep, glm_dep]
)

# Main executable
executable('{}',
  'src/main.cpp',
  link_with : engine_lib,
  dependencies : [opengl_dep, glfw_dep, glm_dep]
)
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameEngineBazelContent() {
    return fmt::format(R"(load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "{}_engine",
    srcs = [
        "src/engine/core/engine.cpp",
        "src/engine/graphics/renderer.cpp",
        "src/engine/input/input_manager.cpp",
        "src/engine/resources/resource_manager.cpp",
        "src/engine/scene/scene_manager.cpp",
        "src/engine/core/entity.cpp",
        "src/game/game.cpp",
    ],
    hdrs = [
        "src/engine/core/engine.h",
        "src/engine/graphics/renderer.h",
        "src/engine/input/input_manager.h",
        "src/engine/resources/resource_manager.h",
        "src/engine/scene/scene_manager.h",
        "src/engine/core/entity.h",
        "src/engine/core/component.h",
        "src/engine/core/system.h",
        "src/game/game.h",
    ],
    includes = ["src"],
    deps = [
        "@opengl//:opengl",
        "@glfw//:glfw",
        "@glm//:glm",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "{}",
    srcs = ["src/main.cpp"],
    deps = [":{}_engine"],
)
)",
                       options_.projectName, options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameEngineXMakeContent() {
    return fmt::format(R"(set_project("{0}")
set_version("1.0.0")

-- Set C++ standard
set_languages("c++17")

-- Add build modes
add_rules("mode.debug", "mode.release")

-- Game engine dependencies
add_requires("opengl", "glfw", "glm", "stb")

-- Engine core library
target("{1}_engine")
    set_kind("static")
    add_files("src/engine/**.cpp")
    add_headerfiles("include/engine/**.h")
    add_includedirs("include", {{public = true}})

    -- Add packages
    add_packages("opengl", "glfw", "glm", "stb")

    -- Set output directory
    set_targetdir("lib")

    -- Enable C++ features
    set_languages("c++17")

    -- Add compile flags
    if is_mode("debug") then
        add_defines("DEBUG", "ENGINE_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG", "ENGINE_RELEASE")
        set_symbols("hidden")
        set_optimize("fastest")
    end

-- Game executable
target("{2}")
    set_kind("binary")
    add_files("src/game/**.cpp", "src/main.cpp")
    add_headerfiles("include/game/**.h")
    add_includedirs("include")
    add_deps("{3}_engine")

    -- Add packages
    add_packages("opengl", "glfw", "glm", "stb")

    -- Set output directory
    set_targetdir("bin")

    -- Enable C++ features
    set_languages("c++17")
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName);
}

std::string GameEngineTemplate::getGameEnginePremakeContent() {
    return fmt::format(R"(workspace "{0}"
    configurations {{ "Debug", "Release" }}
    platforms {{ "x64" }}

project "{1}_engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    targetdir "lib/%{{cfg.buildcfg}}"

    files {{
        "src/engine/**.cpp",
        "include/engine/**.h"
    }}

    includedirs {{
        "include",
        "vendor/glfw/include",
        "vendor/glm",
        "vendor/stb"
    }}

    links {{
        "opengl32",
        "glfw3",
        "gdi32",
        "user32",
        "kernel32"
    }}

    filter "configurations:Debug"
        defines {{ "DEBUG", "ENGINE_DEBUG" }}
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines {{ "NDEBUG", "ENGINE_RELEASE" }}
        symbols "Off"
        optimize "Speed"

project "{2}"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{{cfg.buildcfg}}"

    files {{
        "src/game/**.cpp",
        "src/main.cpp",
        "include/game/**.h"
    }}

    includedirs {{
        "include",
        "vendor/glfw/include",
        "vendor/glm",
        "vendor/stb"
    }}

    links {{
        "{3}_engine",
        "opengl32",
        "glfw3",
        "gdi32",
        "user32",
        "kernel32"
    }}

    filter "configurations:Debug"
        defines {{ "DEBUG" }}
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines {{ "NDEBUG" }}
        symbols "Off"
        optimize "Speed"
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName);
}

std::string GameEngineTemplate::getGameEngineVcpkgContent() {
    return fmt::format(R"({{
  "name": "{}",
  "version": "1.0.0",
  "description": "A modern C++ game engine",
  "homepage": "https://github.com/yourname/{}",
  "dependencies": [
    "opengl",
    "glfw3",
    "glm",
    "stb",
    "assimp",
    "openal-soft"
  ],
  "features": {{
    "audio": {{
      "description": "Audio support",
      "dependencies": [
        "openal-soft"
      ]
    }},
    "physics": {{
      "description": "Physics engine support",
      "dependencies": [
        "bullet3"
      ]
    }},
    "scripting": {{
      "description": "Scripting support",
      "dependencies": [
        "lua"
      ]
    }}
  }}
}}
)",
                       options_.projectName, options_.projectName);
}

std::string GameEngineTemplate::getGameEngineConanContent() {
    return fmt::format(R"([requires]
opengl/system
glfw/3.3.8
glm/0.9.9.8
stb/cci.20220909
assimp/5.2.5

[generators]
CMakeDeps
CMakeToolchain

[options]
shared=False
fPIC=True

[settings]
os
compiler
build_type
arch

[build_requires]
cmake/[>=3.16]
)",
                       options_.projectName);
}

std::string GameEngineTemplate::getAssetPipelineScriptContent() {
    return R"(#!/usr/bin/env python3
"""
Asset Pipeline for Game Engine
Processes and optimizes game assets
"""

import os
import sys
import json
import shutil
from pathlib import Path

class AssetProcessor:
    def __init__(self, assets_dir="assets", output_dir="processed_assets"):
        self.assets_dir = Path(assets_dir)
        self.output_dir = Path(output_dir)
        self.manifest = {"assets": []}

    def process_all_assets(self):
        """Process all assets in the assets directory"""
        print("Starting asset processing...")

        if not self.assets_dir.exists():
            print(f"Assets directory '{self.assets_dir}' not found!")
            return False

        # Create output directory
        self.output_dir.mkdir(exist_ok=True)

        # Process different asset types
        self.process_textures()
        self.process_models()
        self.process_audio()
        self.process_shaders()

        # Generate manifest
        self.generate_manifest()

        print("Asset processing complete!")
        return True

    def process_textures(self):
        """Process texture files"""
        texture_dir = self.assets_dir / "textures"
        if not texture_dir.exists():
            return

        output_texture_dir = self.output_dir / "textures"
        output_texture_dir.mkdir(exist_ok=True)

        for texture_file in texture_dir.glob("*"):
            if texture_file.suffix.lower() in ['.png', '.jpg', '.jpeg', '.tga', '.bmp']:
                print(f"Processing texture: {texture_file.name}")
                # Copy for now - in real implementation, would optimize
                shutil.copy2(texture_file, output_texture_dir / texture_file.name)
                self.manifest["assets"].append({
                    "type": "texture",
                    "source": str(texture_file),
                    "output": str(output_texture_dir / texture_file.name)
                })

    def process_models(self):
        """Process 3D model files"""
        model_dir = self.assets_dir / "models"
        if not model_dir.exists():
            return

        output_model_dir = self.output_dir / "models"
        output_model_dir.mkdir(exist_ok=True)

        for model_file in model_dir.glob("*"):
            if model_file.suffix.lower() in ['.obj', '.fbx', '.gltf', '.dae']:
                print(f"Processing model: {model_file.name}")
                shutil.copy2(model_file, output_model_dir / model_file.name)
                self.manifest["assets"].append({
                    "type": "model",
                    "source": str(model_file),
                    "output": str(output_model_dir / model_file.name)
                })

    def process_audio(self):
        """Process audio files"""
        audio_dir = self.assets_dir / "sounds"
        if not audio_dir.exists():
            return

        output_audio_dir = self.output_dir / "sounds"
        output_audio_dir.mkdir(exist_ok=True)

        for audio_file in audio_dir.glob("*"):
            if audio_file.suffix.lower() in ['.wav', '.ogg', '.mp3']:
                print(f"Processing audio: {audio_file.name}")
                shutil.copy2(audio_file, output_audio_dir / audio_file.name)
                self.manifest["assets"].append({
                    "type": "audio",
                    "source": str(audio_file),
                    "output": str(output_audio_dir / audio_file.name)
                })

    def process_shaders(self):
        """Process shader files"""
        shader_dir = self.assets_dir / "shaders"
        if not shader_dir.exists():
            return

        output_shader_dir = self.output_dir / "shaders"
        output_shader_dir.mkdir(exist_ok=True)

        for shader_file in shader_dir.glob("*"):
            if shader_file.suffix.lower() in ['.glsl', '.vert', '.frag', '.hlsl']:
                print(f"Processing shader: {shader_file.name}")
                shutil.copy2(shader_file, output_shader_dir / shader_file.name)
                self.manifest["assets"].append({
                    "type": "shader",
                    "source": str(shader_file),
                    "output": str(output_shader_dir / shader_file.name)
                })

    def generate_manifest(self):
        """Generate asset manifest file"""
        manifest_file = self.output_dir / "asset_manifest.json"
        with open(manifest_file, 'w') as f:
            json.dump(self.manifest, f, indent=2)
        print(f"Generated manifest: {manifest_file}")

def main():
    processor = AssetProcessor()
    success = processor.process_all_assets()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
)";
}

std::string GameEngineTemplate::getGameEngineReadmeContent() {
    return fmt::format(R"(# {} Game Engine

A modern C++ game engine built with performance and flexibility in mind.

## Features

- **Modern C++ Architecture**: Built with C++17/20 features
- **Component-Entity System**: Flexible ECS architecture
- **Cross-Platform**: Windows, Linux, and macOS support
- **Graphics Rendering**: OpenGL/Vulkan support
- **Audio System**: 3D positional audio
- **Physics Integration**: Built-in physics engine
- **Asset Pipeline**: Automated asset processing
- **Scripting Support**: Lua/Python scripting integration

## Quick Start

### Prerequisites

- C++17 compatible compiler
- CMake 3.15 or higher
- Graphics drivers supporting OpenGL 4.5+

### Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running

```bash
./bin/{}
```

## Project Structure

```
src/
├── engine/           # Core engine systems
�?  ├── core/        # Engine core (Engine, Entity, Component)
�?  ├── graphics/    # Rendering system
�?  ├── audio/       # Audio system
�?  ├── input/       # Input management
�?  ├── physics/     # Physics integration
�?  ├── scripting/   # Scripting engine
�?  ├── resources/   # Resource management
�?  └── scene/       # Scene management
├── game/            # Game-specific code
└── main.cpp         # Application entry point

assets/
├── textures/        # Texture files
├── models/          # 3D models
├── sounds/          # Audio files
├── shaders/         # Shader programs
├── fonts/           # Font files
└── scenes/          # Scene definitions

tools/
├── asset_pipeline/  # Asset processing tools
└── shader_compiler/ # Shader compilation tools
```

## Development

### Adding New Components

1. Create header file in `src/engine/core/components/`
2. Implement component logic
3. Register component in the component system

### Creating Custom Systems

1. Inherit from `System` base class
2. Implement `Update()` and `Render()` methods
3. Register system in the engine

### Asset Pipeline

The engine includes an automated asset pipeline:

```bash
python tools/asset_pipeline/process_assets.py
```

This will:
- Optimize textures
- Compile shaders
- Process 3D models
- Generate asset manifests

## Documentation

- [Architecture Guide](docs/architecture.md)
- [API Reference](docs/api.md)
- [Getting Started](docs/getting-started.md)
- [Examples](examples/)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## Support

- [Documentation](docs/)
- [Issues](https://github.com/your-username/{}/issues)
- [Discussions](https://github.com/your-username/{}/discussions)
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName);
}

std::string GameEngineTemplate::getShaderCompilerScriptContent() {
    return R"(#!/usr/bin/env python3
"""
Shader Compiler Script for Game Engine
Compiles HLSL/GLSL shaders to various formats
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def compile_shader(input_file, output_file, shader_type):
    """Compile a single shader file"""
    print(f"Compiling {input_file} -> {output_file}")

    # Add your shader compilation logic here
    # This is a placeholder implementation

    return True

def main():
    parser = argparse.ArgumentParser(description='Compile shaders')
    parser.add_argument('--input', '-i', required=True, help='Input shader directory')
    parser.add_argument('--output', '-o', required=True, help='Output directory')

    args = parser.parse_args()

    input_dir = Path(args.input)
    output_dir = Path(args.output)

    if not input_dir.exists():
        print(f"Error: Input directory {input_dir} does not exist")
        return 1

    output_dir.mkdir(parents=True, exist_ok=True)

    # Find and compile shader files
    shader_files = list(input_dir.glob('**/*.hlsl')) + list(input_dir.glob('**/*.glsl'))

    for shader_file in shader_files:
        relative_path = shader_file.relative_to(input_dir)
        output_file = output_dir / relative_path.with_suffix('.spv')

        if not compile_shader(shader_file, output_file, 'vertex'):
            return 1

    print(f"Successfully compiled {len(shader_files)} shaders")
    return 0

if __name__ == '__main__':
    sys.exit(main())
)";
}
