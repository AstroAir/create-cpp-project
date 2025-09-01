#include "engine.h"
#include "../graphics/renderer.h"
#include "../input/input_manager.h"
#include "../resources/resource_manager.h"
#include "../scene/scene_manager.h"
#include <chrono>
#include <iostream>

namespace test-gameengine-fixed {

Engine::Engine()
    : isRunning_(false), deltaTime_(0.0f), totalTime_(0.0) {
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const std::string& configPath) {
    std::cout << "Initializing test-gameengine-fixed Engine..." << std::endl;

    // Initialize subsystems
    renderer_ = std::make_unique<Renderer>();
    if (!renderer_->Initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    inputManager_ = std::make_unique<InputManager>();
    if (!inputManager_->Initialize()) {
        std::cerr << "Failed to initialize input manager" << std::endl;
        return false;
    }

    resourceManager_ = std::make_unique<ResourceManager>();
    if (!resourceManager_->Initialize()) {
        std::cerr << "Failed to initialize resource manager" << std::endl;
        return false;
    }

    sceneManager_ = std::make_unique<SceneManager>();
    if (!sceneManager_->Initialize()) {
        std::cerr << "Failed to initialize scene manager" << std::endl;
        return false;
    }

    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}

void Engine::Run() {
    isRunning_ = true;

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (isRunning_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime);
        deltaTime_ = duration.count() / 1000000.0f;
        totalTime_ += deltaTime_;
        lastTime = currentTime;

        // Update input
        inputManager_->Update();

        // Check for exit condition
        if (inputManager_->IsKeyPressed("Escape")) {
            Stop();
        }

        // Update game logic
        Update(deltaTime_);

        // Render frame
        Render();
    }
}

void Engine::Update(float deltaTime) {
    sceneManager_->Update(deltaTime);
}

void Engine::Render() {
    renderer_->BeginFrame();
    sceneManager_->Render(renderer_.get());
    renderer_->EndFrame();
}

void Engine::Shutdown() {
    std::cout << "Shutting down engine..." << std::endl;

    sceneManager_.reset();
    resourceManager_.reset();
    inputManager_.reset();
    renderer_.reset();

    std::cout << "Engine shutdown complete." << std::endl;
}

} // namespace test-gameengine-fixed
