#include "game/game.h"
#include <iostream>

int main() {
    std::cout << "Starting quick-test-premake-game Game Engine..." << std::endl;

    quick-test-premake-game::Game game;

    if (!game.Initialize()) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }

    game.Run();
    game.Shutdown();

    std::cout << "Game exited successfully." << std::endl;
    return 0;
}
