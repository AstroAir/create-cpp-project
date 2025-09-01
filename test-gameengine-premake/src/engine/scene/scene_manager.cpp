#include "scene_manager.h"
#include "entity.h"
#include "../graphics/renderer.h"
#include <iostream>
#include <algorithm>

namespace test-gameengine-premake {

Scene::Scene(const std::string& name) : name_(name) {
}

Scene::~Scene() {
    Shutdown();
}

void Scene::AddEntity(std::shared_ptr<Entity> entity) {
    entities_.push_back(entity);
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity) {
    auto it = std::find(entities_.begin(), entities_.end(), entity);
    if (it != entities_.end()) {
        entities_.erase(it);
    }
}

struct SceneManager::SceneManagerImpl {
    std::shared_ptr<Scene> currentScene;
    bool initialized = false;
};

SceneManager::SceneManager() : impl_(std::make_unique<SceneManagerImpl>()) {
}

SceneManager::~SceneManager() {
    Shutdown();
}

bool SceneManager::Initialize() {
    std::cout << "Initializing scene manager..." << std::endl;

    impl_->initialized = true;

    std::cout << "Scene manager initialized successfully" << std::endl;
    return true;
}

void SceneManager::Shutdown() {
    if (impl_->initialized) {
        std::cout << "Shutting down scene manager..." << std::endl;
        UnloadCurrentScene();
        impl_->initialized = false;
    }
}

void SceneManager::Update(float deltaTime) {
    if (impl_->currentScene) {
        impl_->currentScene->Update(deltaTime);
    }
}

void SceneManager::Render(Renderer* renderer) {
    if (impl_->currentScene) {
        impl_->currentScene->Render(renderer);
    }
}

void SceneManager::LoadScene(std::shared_ptr<Scene> scene) {
    if (impl_->currentScene) {
        impl_->currentScene->Shutdown();
    }

    impl_->currentScene = scene;

    if (impl_->currentScene) {
        impl_->currentScene->Initialize();
        std::cout << "Loaded scene: " << impl_->currentScene->GetName() << std::endl;
    }
}

void SceneManager::UnloadCurrentScene() {
    if (impl_->currentScene) {
        impl_->currentScene->Shutdown();
        impl_->currentScene.reset();
        std::cout << "Unloaded current scene" << std::endl;
    }
}

Scene* SceneManager::GetCurrentScene() const {
    return impl_->currentScene.get();
}

} // namespace test-gameengine-premake
