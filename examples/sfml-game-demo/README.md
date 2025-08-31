# SFML Game Demo - Complete Game Development Example

A comprehensive example of a modern C++ game using SFML (Simple and Fast Multimedia Library), demonstrating game development best practices, graphics programming, and cross-platform game deployment.

## 🎯 What This Example Demonstrates

- **Modern SFML 2.6+** game architecture
- **Entity Component System (ECS)** design pattern
- **Game state management** with multiple screens
- **2D graphics and animation** with sprites and textures
- **Audio system** with sound effects and music
- **Input handling** for keyboard, mouse, and gamepad
- **Collision detection** and physics simulation
- **Resource management** with efficient loading/unloading
- **Game loop optimization** with fixed timestep
- **Cross-platform deployment** for Windows, Linux, macOS

## 🚀 Generated Command

This project was created using:

```bash
cpp-scaffold sfml-game-demo \
  --type gui \
  --framework sfml \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode \
  --std cpp20 \
  --docs
```

## 📁 Project Structure

```
sfml-game-demo/
├── CMakeLists.txt              # Modern CMake with SFML integration
├── vcpkg.json                  # Package dependencies
├── .gitignore                  # Git ignore patterns
├── LICENSE                     # MIT license
├── README.md                   # Project documentation
├── .github/
│   └── workflows/
│       ├── ci.yml              # Multi-platform CI/CD
│       └── release.yml         # Release automation
├── .vscode/
│   ├── settings.json           # VSCode SFML settings
│   ├── tasks.json              # Build and debug tasks
│   └── launch.json             # Debug configuration
├── src/
│   ├── main.cpp                # Application entry point
│   ├── game/
│   │   ├── game.h              # Main game class
│   │   ├── game.cpp
│   │   ├── gamestate.h         # Game state interface
│   │   └── gamestate.cpp
│   ├── states/
│   │   ├── menustate.h         # Main menu state
│   │   ├── menustate.cpp
│   │   ├── gameplaystate.h     # Gameplay state
│   │   ├── gameplaystate.cpp
│   │   ├── pausestate.h        # Pause state
│   │   └── pausestate.cpp
│   ├── entities/
│   │   ├── entity.h            # Base entity class
│   │   ├── entity.cpp
│   │   ├── player.h            # Player entity
│   │   ├── player.cpp
│   │   ├── enemy.h             # Enemy entity
│   │   ├── enemy.cpp
│   │   ├── projectile.h        # Projectile entity
│   │   └── projectile.cpp
│   ├── components/
│   │   ├── component.h         # Base component
│   │   ├── transform.h         # Transform component
│   │   ├── sprite.h            # Sprite component
│   │   ├── physics.h           # Physics component
│   │   ├── input.h             # Input component
│   │   └── audio.h             # Audio component
│   ├── systems/
│   │   ├── rendersystem.h      # Rendering system
│   │   ├── rendersystem.cpp
│   │   ├── physicssystem.h     # Physics system
│   │   ├── physicssystem.cpp
│   │   ├── inputsystem.h       # Input system
│   │   ├── inputsystem.cpp
│   │   ├── audiosystem.h       # Audio system
│   │   └── audiosystem.cpp
│   ├── utils/
│   │   ├── resourcemanager.h   # Resource management
│   │   ├── resourcemanager.cpp
│   │   ├── mathutils.h         # Math utilities
│   │   ├── mathutils.cpp
│   │   ├── timer.h             # Timer utilities
│   │   └── timer.cpp
│   └── config/
│       ├── settings.h          # Game settings
│       └── settings.cpp
├── assets/
│   ├── textures/               # Game textures
│   │   ├── player.png
│   │   ├── enemy.png
│   │   ├── background.png
│   │   └── ui/
│   │       ├── button.png
│   │       └── menu_bg.png
│   ├── sounds/                 # Sound effects
│   │   ├── shoot.wav
│   │   ├── explosion.wav
│   │   └── pickup.wav
│   ├── music/                  # Background music
│   │   ├── menu.ogg
│   │   └── gameplay.ogg
│   ├── fonts/                  # Game fonts
│   │   ├── arial.ttf
│   │   └── game_font.ttf
│   └── shaders/                # GLSL shaders (optional)
│       ├── vertex.glsl
│       └── fragment.glsl
├── tests/
│   ├── CMakeLists.txt          # Test configuration
│   ├── test_main.cpp           # Test runner
│   ├── test_entities.cpp       # Entity tests
│   ├── test_systems.cpp        # System tests
│   └── test_utils.cpp          # Utility tests
├── docs/
│   ├── game-design.md          # Game design document
│   ├── architecture.md         # Technical architecture
│   └── deployment.md           # Deployment guide
└── deployment/
    ├── windows/                # Windows deployment
    │   └── package.bat         # Packaging script
    ├── macos/                  # macOS deployment
    │   └── package.sh          # Packaging script
    └── linux/                  # Linux deployment
        └── package.sh          # Packaging script
```

## 🎮 Game Features

### Core Gameplay

- **Space Shooter**: Classic arcade-style space shooter
- **Player Movement**: Smooth WASD/arrow key movement
- **Shooting Mechanics**: Projectile-based combat system
- **Enemy AI**: Multiple enemy types with different behaviors
- **Power-ups**: Collectible items that enhance gameplay
- **Scoring System**: Points-based progression
- **Multiple Levels**: Progressive difficulty scaling

### Technical Features

- **60 FPS Gameplay**: Smooth, consistent frame rate
- **Entity Component System**: Modular, extensible architecture
- **State Management**: Clean separation of game states
- **Resource Caching**: Efficient texture and sound loading
- **Collision Detection**: AABB and circle collision systems
- **Particle Effects**: Visual effects for explosions and trails
- **Audio Management**: Spatial audio with volume controls

### User Interface

- **Main Menu**: Start game, settings, quit options
- **In-Game HUD**: Health, score, ammunition display
- **Pause Menu**: Pause/resume, settings, quit to menu
- **Settings Screen**: Graphics, audio, and control options
- **Game Over Screen**: Final score and restart options

## 🏗️ Building the Project

### Prerequisites

- **SFML 2.6+**: Graphics, Window, Audio, Network, System modules
- **C++20** compatible compiler
- **CMake 3.16** or later
- **vcpkg** package manager
- **OpenGL** support (usually included with graphics drivers)

### Dependencies (managed by vcpkg)

```json
{
  "name": "sfml-game-demo",
  "version": "1.0.0",
  "dependencies": [
    "sfml",
    "nlohmann-json",
    "gtest"
  ]
}
```

### Build Steps

1. **Install SFML with vcpkg:**
   ```bash
   # Install vcpkg if not already installed
   git clone https://github.com/Microsoft/vcpkg.git
   ./vcpkg/bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows
   
   # Install SFML and dependencies
   ./vcpkg/vcpkg install sfml nlohmann-json gtest
   ```

2. **Configure and build:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

3. **Copy assets:**
   ```bash
   # Assets are automatically copied during build
   # Or manually copy if needed:
   cp -r ../assets ./assets
   ```

4. **Run the game:**
   ```bash
   # On Windows
   ./Release/sfml-game-demo.exe
   
   # On Linux/macOS
   ./sfml-game-demo
   ```

## 🎮 Gameplay Instructions

### Controls

| Key | Action |
|-----|--------|
| `W/↑` | Move Up |
| `S/↓` | Move Down |
| `A/←` | Move Left |
| `D/→` | Move Right |
| `Space` | Shoot |
| `P` | Pause Game |
| `Esc` | Pause/Menu |
| `Enter` | Confirm/Start |
| `Mouse` | Menu Navigation |

### Game Mechanics

1. **Movement**: Use WASD or arrow keys to move your ship
2. **Shooting**: Press Space to fire projectiles at enemies
3. **Enemies**: Destroy enemies to earn points
4. **Power-ups**: Collect power-ups for enhanced abilities
5. **Health**: Avoid enemy projectiles and collisions
6. **Scoring**: Higher scores unlock achievements

### Game Modes

- **Classic Mode**: Traditional endless gameplay
- **Campaign Mode**: Structured levels with progression
- **Survival Mode**: Increasingly difficult waves
- **Time Attack**: Score as much as possible in limited time

## 🧪 Testing

### Test Categories

- **Unit Tests**: Individual component and system testing
- **Integration Tests**: Game state transitions and interactions
- **Performance Tests**: Frame rate and memory usage validation
- **Gameplay Tests**: Game mechanics and balance verification

### Running Tests

```bash
# Run all tests
ctest

# Run specific test suite
ctest -R EntityTests

# Run with SFML headless mode
export DISPLAY=:99  # For Linux CI
./test_sfml_game_demo
```

### Performance Benchmarks

- **Target FPS**: 60 FPS consistent
- **Memory Usage**: < 100MB during gameplay
- **Load Times**: < 2 seconds for level transitions
- **Input Latency**: < 16ms response time

## 🎨 Asset Pipeline

### Texture Management

```cpp
// Efficient texture loading and caching
class ResourceManager {
public:
    sf::Texture& getTexture(const std::string& filename) {
        auto it = textures_.find(filename);
        if (it != textures_.end()) {
            return it->second;
        }
        
        sf::Texture texture;
        if (!texture.loadFromFile("assets/textures/" + filename)) {
            throw std::runtime_error("Failed to load texture: " + filename);
        }
        
        textures_[filename] = std::move(texture);
        return textures_[filename];
    }
    
private:
    std::unordered_map<std::string, sf::Texture> textures_;
};
```

### Audio System

```cpp
// 3D positional audio with distance attenuation
class AudioSystem {
public:
    void playSound(const std::string& soundName, sf::Vector2f position) {
        sf::Sound sound;
        sound.setBuffer(getBuffer(soundName));
        sound.setPosition(position.x, position.y, 0.0f);
        sound.setAttenuation(0.1f);
        sound.setMinDistance(50.0f);
        
        sounds_.push_back(std::move(sound));
        sounds_.back().play();
    }
    
    void update() {
        // Remove finished sounds
        sounds_.erase(
            std::remove_if(sounds_.begin(), sounds_.end(),
                [](const sf::Sound& sound) {
                    return sound.getStatus() == sf::Sound::Stopped;
                }),
            sounds_.end()
        );
    }
    
private:
    std::vector<sf::Sound> sounds_;
    std::unordered_map<std::string, sf::SoundBuffer> buffers_;
};
```

## 🚀 Advanced Features

### Entity Component System

```cpp
// Modern ECS implementation
class Entity {
public:
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        components_[typeid(T)] = std::move(component);
        return *ptr;
    }
    
    template<typename T>
    T& getComponent() {
        auto it = components_.find(typeid(T));
        if (it == components_.end()) {
            throw std::runtime_error("Component not found");
        }
        return static_cast<T&>(*it->second);
    }
    
private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
};
```

### Shader Effects

```glsl
// Fragment shader for particle effects
#version 330 core

in vec2 texCoord;
in vec4 color;

out vec4 fragColor;

uniform sampler2D texture;
uniform float time;

void main() {
    vec4 texColor = texture2D(texture, texCoord);
    
    // Add pulsing effect
    float pulse = sin(time * 5.0) * 0.1 + 0.9;
    
    // Apply color and alpha
    fragColor = texColor * color * pulse;
}
```

## 📦 Deployment

### Windows Deployment

```batch
@echo off
REM Build release version
cmake --build build --config Release

REM Copy SFML DLLs
copy "vcpkg\installed\x64-windows\bin\sfml-*.dll" "build\Release\"

REM Copy assets
xcopy "assets" "build\Release\assets" /E /I

REM Create distribution package
7z a sfml-game-demo-windows.zip "build\Release\*"
```

### Linux Deployment

```bash
#!/bin/bash
# Build release version
cmake --build build --config Release

# Copy assets
cp -r assets build/

# Create AppImage (requires linuxdeploy)
linuxdeploy --appdir build/AppDir --executable build/sfml-game-demo --create-desktop-file --output appimage

# Or create tarball
tar -czf sfml-game-demo-linux.tar.gz -C build sfml-game-demo assets
```

## 🎓 Learning Outcomes

By studying this example, you'll learn:

1. **Game Architecture**: Modern game development patterns and ECS design
2. **SFML Framework**: Comprehensive multimedia library usage
3. **Real-time Programming**: Game loops, timing, and performance optimization
4. **Graphics Programming**: 2D rendering, sprites, and visual effects
5. **Audio Programming**: Sound effects, music, and spatial audio
6. **Input Handling**: Responsive controls and user interaction
7. **Resource Management**: Efficient asset loading and memory management
8. **Game State Management**: Clean separation of game screens and logic

## 🔗 Related Examples

- [Qt Image Viewer](../qt-image-viewer/) - GUI application with Qt
- [Console File Processor](../console-file-processor/) - Command-line application
- [REST API Server](../rest-api-server/) - Network service example

## 📚 Additional Resources

- [SFML Documentation](https://www.sfml-dev.org/documentation/)
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)
- [Real-Time Rendering](https://www.realtimerendering.com/)
- [Game Engine Architecture](https://www.gameenginebook.com/)
