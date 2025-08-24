#include "gameengine_template.h"
#include "../utils/file_utils.h"
#include "../utils/terminal_utils.h"
#include <filesystem>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

GameEngineTemplate::GameEngineTemplate(const CliOptions &options) : TemplateBase(options) {}

bool GameEngineTemplate::create() {
  spdlog::info("Creating game engine project: {}", options_.projectName);

  // Show progress
  utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 0, "Initializing");

  if (!createProjectStructure()) {
    spdlog::error("Failed to create project structure");
    return false;
  }

  utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 20, "Setting up graphics framework");

  if (!setupGraphicsFramework()) {
    spdlog::error("Failed to setup graphics framework");
    return false;
  }

  utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 40, "Configuring build system");

  if (!createBuildSystem()) {
    spdlog::error("Failed to create build system");
    return false;
  }

  utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 60, "Setting up package manager");

  if (!setupPackageManager()) {
    spdlog::error("Failed to setup package manager");
    return false;
  }

  utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 80, "Configuring testing framework");

  if (options_.includeTests && !setupTestFramework()) {
    spdlog::error("Failed to setup test framework");
    return false;
  }

  utils::TerminalUtils::showNpmStyleProgress("Creating game engine project", 90, "Setting up asset pipeline");

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
    spdlog::info("✅ Git repository initialized");
  }

  utils::TerminalUtils::showNpmStyleSuccess("Game engine project created successfully!", options_.projectName);

  // Print usage guide
  printUsageGuide();

  return true;
}

bool GameEngineTemplate::createProjectStructure() {
  std::string projectPath = options_.projectName;

  // Create main directories
  std::vector<std::string> directories = {
    projectPath,
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
    projectPath + "/lib"
  };

  for (const auto &dir : directories) {
    if (!FileUtils::createDirectory(dir)) {
      spdlog::error("Failed to create directory: {}", dir);
      return false;
    }
  }

  // Create main engine files
  if (!FileUtils::writeToFile(projectPath + "/src/engine/core/engine.h", getEngineHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/core/engine.cpp", getEngineSourceContent())) {
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
  if (!FileUtils::writeToFile(projectPath + "/src/engine/graphics/renderer.h", getRendererHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/graphics/renderer.cpp", getRendererSourceContent())) {
    return false;
  }

  // Create input system files
  if (!FileUtils::writeToFile(projectPath + "/src/engine/input/input_manager.h", getInputManagerHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/input/input_manager.cpp", getInputManagerSourceContent())) {
    return false;
  }

  // Create resource manager files
  if (!FileUtils::writeToFile(projectPath + "/src/engine/resources/resource_manager.h", getResourceManagerHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/resources/resource_manager.cpp", getResourceManagerSourceContent())) {
    return false;
  }

  // Create scene system files
  if (!FileUtils::writeToFile(projectPath + "/src/engine/scene/scene_manager.h", getSceneManagerHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/scene/scene_manager.cpp", getSceneManagerSourceContent())) {
    return false;
  }

  // Create ECS files
  if (!FileUtils::writeToFile(projectPath + "/src/engine/core/entity.h", getEntityHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/core/entity.cpp", getEntitySourceContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/core/component.h", getComponentHeaderContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/src/engine/core/system.h", getSystemHeaderContent())) {
    return false;
  }

  // Create basic shaders
  if (!FileUtils::writeToFile(projectPath + "/assets/shaders/vertex.glsl", getShaderContent("vertex"))) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/assets/shaders/fragment.glsl", getShaderContent("fragment"))) {
    return false;
  }

  // Create configuration files
  if (!FileUtils::writeToFile(projectPath + "/config/engine.json", getEngineConfigContent())) {
    return false;
  }

  if (!FileUtils::writeToFile(projectPath + "/config/graphics.json", getGraphicsConfigContent())) {
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
      return FileUtils::writeToFile(projectPath + "/CMakeLists.txt", getGameEngineCMakeContent());
    case BuildSystem::Meson:
      return FileUtils::writeToFile(projectPath + "/meson.build", getGameEngineMesonContent());
    case BuildSystem::Bazel:
      return FileUtils::writeToFile(projectPath + "/BUILD.bazel", getGameEngineBazelContent());
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
      return FileUtils::writeToFile(projectPath + "/conanfile.txt", getGameEngineConanContent());
    case PackageManager::None:
      return true; // No package manager setup needed
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
  if (!FileUtils::writeToFile(projectPath + "/tools/asset_pipeline/process_assets.py", getAssetPipelineScriptContent())) {
    return false;
  }

  // Create shader compiler script
  if (!FileUtils::writeToFile(projectPath + "/tools/shader_compiler/compile_shaders.py", getShaderCompilerScriptContent())) {
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
)", options_.projectName, options_.projectName);
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
)", options_.projectName, options_.projectName, options_.projectName);
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
)", options_.projectName, options_.projectName);
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
)", options_.projectName, options_.projectName);
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
)", options_.projectName, options_.projectName);
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
│   ├── core/        # Engine core (Engine, Entity, Component)
│   ├── graphics/    # Rendering system
│   ├── audio/       # Audio system
│   ├── input/       # Input management
│   ├── physics/     # Physics integration
│   ├── scripting/   # Scripting engine
│   ├── resources/   # Resource management
│   └── scene/       # Scene management
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
)", options_.projectName, options_.projectName, options_.projectName, options_.projectName);
}
