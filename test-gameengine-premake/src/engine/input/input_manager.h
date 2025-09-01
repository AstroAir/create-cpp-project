#pragma once

#include <string>
#include <unordered_map>

namespace test-gameengine-premake {

class InputManager {
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
};

} // namespace test-gameengine-premake
