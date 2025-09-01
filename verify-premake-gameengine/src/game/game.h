#pragma once

#include "engine/core/engine.h"

namespace verify-premake-gameengine {

class Game {
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
};

} // namespace verify-premake-gameengine
