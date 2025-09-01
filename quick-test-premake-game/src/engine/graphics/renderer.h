#pragma once

#include <memory>
#include <string>

namespace quick-test-premake-game {

class Renderer {
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
};

} // namespace quick-test-premake-game
