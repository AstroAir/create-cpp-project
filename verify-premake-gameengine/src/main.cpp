#include "game/game.h"
#include <iostream>

int main() {
    std::cout << "Starting verify-premake-gameengine Game Engine..." << std::endl;

    verify-premake-gameengine::Game game;

    if (!game.Initialize()) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }

    game.Run();
    game.Shutdown();

    std::cout << "Game exited successfully." << std::endl;
    return 0;
}
