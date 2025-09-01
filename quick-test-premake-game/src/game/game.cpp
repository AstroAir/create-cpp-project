#include "game.h"
#include <iostream>

namespace quick-test-premake-game {

Game::Game() {
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    std::cout << "Initializing game..." << std::endl;

    if (!engine_.Initialize()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return false;
    }

    // Initialize game-specific systems here

    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}

void Game::Run() {
    engine_.Run();
}

void Game::Update(float deltaTime) {
    // Game-specific update logic here
}

void Game::Render() {
    // Game-specific rendering here
}

void Game::HandleInput() {
    // Game-specific input handling here
}

void Game::Shutdown() {
    std::cout << "Shutting down game..." << std::endl;
    engine_.Shutdown();
}

} // namespace quick-test-premake-game
