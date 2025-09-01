#include "input_manager.h"
#include <iostream>

namespace test-gameengine-fixed {

struct InputManager::InputManagerImpl {
    std::unordered_map<std::string, bool> keyStates;
    std::unordered_map<std::string, bool> prevKeyStates;
    std::unordered_map<int, bool> mouseButtonStates;
    std::unordered_map<int, bool> prevMouseButtonStates;
    int mouseX = 0, mouseY = 0;
    bool initialized = false;
};

InputManager::InputManager() : impl_(std::make_unique<InputManagerImpl>()) {
}

InputManager::~InputManager() {
    Shutdown();
}

bool InputManager::Initialize() {
    std::cout << "Initializing input manager..." << std::endl;

    // TODO: Initialize input system
    impl_->initialized = true;

    std::cout << "Input manager initialized successfully" << std::endl;
    return true;
}

void InputManager::Shutdown() {
    if (impl_->initialized) {
        std::cout << "Shutting down input manager..." << std::endl;
        impl_->initialized = false;
    }
}

void InputManager::Update() {
    if (!impl_->initialized) return;

    // Store previous frame states
    impl_->prevKeyStates = impl_->keyStates;
    impl_->prevMouseButtonStates = impl_->mouseButtonStates;

    // TODO: Poll input events and update current states
}

bool InputManager::IsKeyPressed(const std::string& key) const {
    auto current = impl_->keyStates.find(key);
    auto previous = impl_->prevKeyStates.find(key);

    bool currentState = (current != impl_->keyStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevKeyStates.end()) ? previous->second : false;

    return currentState && !previousState;
}

bool InputManager::IsKeyDown(const std::string& key) const {
    auto it = impl_->keyStates.find(key);
    return (it != impl_->keyStates.end()) ? it->second : false;
}

bool InputManager::IsKeyUp(const std::string& key) const {
    auto current = impl_->keyStates.find(key);
    auto previous = impl_->prevKeyStates.find(key);

    bool currentState = (current != impl_->keyStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevKeyStates.end()) ? previous->second : false;

    return !currentState && previousState;
}

bool InputManager::IsMouseButtonPressed(int button) const {
    auto current = impl_->mouseButtonStates.find(button);
    auto previous = impl_->prevMouseButtonStates.find(button);

    bool currentState = (current != impl_->mouseButtonStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevMouseButtonStates.end()) ? previous->second : false;

    return currentState && !previousState;
}

bool InputManager::IsMouseButtonDown(int button) const {
    auto it = impl_->mouseButtonStates.find(button);
    return (it != impl_->mouseButtonStates.end()) ? it->second : false;
}

bool InputManager::IsMouseButtonUp(int button) const {
    auto current = impl_->mouseButtonStates.find(button);
    auto previous = impl_->prevMouseButtonStates.find(button);

    bool currentState = (current != impl_->mouseButtonStates.end()) ? current->second : false;
    bool previousState = (previous != impl_->prevMouseButtonStates.end()) ? previous->second : false;

    return !currentState && previousState;
}

void InputManager::GetMousePosition(int& x, int& y) const {
    x = impl_->mouseX;
    y = impl_->mouseY;
}

} // namespace test-gameengine-fixed
