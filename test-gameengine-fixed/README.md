# test-gameengine-fixed Game Engine

A modern C++ game engine built with performance and flexibility in mind.

## Features

- **Modern C++ Architecture**: Built with C++17/20 features
- **Component-Entity System**: Flexible ECS architecture
- **Cross-Platform**: Windows, Linux, and macOS support
- **Graphics Rendering**: OpenGL/Vulkan support
- **Audio System**: 3D positional audio
- **Physics Integration**: Built-in physics engine
- **Asset Pipeline**: Automated asset processing
- **Scripting Support**: Lua/Python scripting integration

## Quick Start

### Prerequisites

- C++17 compatible compiler
- CMake 3.15 or higher
- Graphics drivers supporting OpenGL 4.5+

### Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running

```bash
./bin/test-gameengine-fixed
```

## Project Structure

```
src/
├── engine/           # Core engine systems
│   ├── core/        # Engine core (Engine, Entity, Component)
│   ├── graphics/    # Rendering system
│   ├── audio/       # Audio system
│   ├── input/       # Input management
│   ├── physics/     # Physics integration
│   ├── scripting/   # Scripting engine
│   ├── resources/   # Resource management
│   └── scene/       # Scene management
├── game/            # Game-specific code
└── main.cpp         # Application entry point

assets/
├── textures/        # Texture files
├── models/          # 3D models
├── sounds/          # Audio files
├── shaders/         # Shader programs
├── fonts/           # Font files
└── scenes/          # Scene definitions

tools/
├── asset_pipeline/  # Asset processing tools
└── shader_compiler/ # Shader compilation tools
```

## Development

### Adding New Components

1. Create header file in `src/engine/core/components/`
2. Implement component logic
3. Register component in the component system

### Creating Custom Systems

1. Inherit from `System` base class
2. Implement `Update()` and `Render()` methods
3. Register system in the engine

### Asset Pipeline

The engine includes an automated asset pipeline:

```bash
python tools/asset_pipeline/process_assets.py
```

This will:
- Optimize textures
- Compile shaders
- Process 3D models
- Generate asset manifests

## Documentation

- [Architecture Guide](docs/architecture.md)
- [API Reference](docs/api.md)
- [Getting Started](docs/getting-started.md)
- [Examples](examples/)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## Support

- [Documentation](docs/)
- [Issues](https://github.com/your-username/test-gameengine-fixed/issues)
- [Discussions](https://github.com/your-username/test-gameengine-fixed/discussions)
