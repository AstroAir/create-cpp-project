# Module Partitions Example

This example demonstrates advanced C++20 module partitions for organizing large modules into logical components.

## What's Included

- **Interface Partitions**: Public module interfaces split into logical units
- **Implementation Partitions**: Private implementation details
- **Module Organization**: Best practices for large module hierarchies
- **Advanced Features**: Template partitions, concept exports, and more

## Generated Structure

```
partitions/
├── CMakeLists.txt
├── modules/
│   ├── graphics.cppm              # Primary module interface
│   ├── graphics-geometry.cppm     # Geometry interface partition
│   ├── graphics-rendering.cppm    # Rendering interface partition
│   ├── graphics-shaders.cppm      # Shaders interface partition
│   ├── graphics-impl-math.cppm    # Math implementation partition
│   └── graphics-impl-utils.cppm   # Utils implementation partition
├── src/
│   ├── graphics.cpp               # Primary module implementation
│   ├── geometry.cpp               # Geometry implementation
│   ├── rendering.cpp              # Rendering implementation
│   ├── shaders.cpp                # Shaders implementation
│   ├── math_impl.cpp              # Math implementation
│   ├── utils_impl.cpp             # Utils implementation
│   └── main.cpp                   # Demo application
├── tests/
│   ├── test_geometry.cpp          # Geometry tests
│   ├── test_rendering.cpp         # Rendering tests
│   └── test_shaders.cpp           # Shaders tests
└── shaders/                       # Shader files
    ├── vertex.glsl
    └── fragment.glsl
```

## Module Partition Types

### 1. Interface Partitions
Export public APIs for specific functionality:

```cpp
// graphics-geometry.cppm
export module graphics:geometry;

export namespace graphics::geometry {
    class Point3D { /* ... */ };
    class Vector3D { /* ... */ };
    class Matrix4x4 { /* ... */ };
}
```

### 2. Implementation Partitions
Internal implementation details:

```cpp
// graphics-impl-math.cppm
module graphics:impl_math;

namespace graphics::impl {
    // Internal math utilities
    constexpr double PI = 3.14159265359;
    double fast_sqrt(double x);
}
```

### 3. Primary Module Interface
Combines and re-exports partitions:

```cpp
// graphics.cppm
export module graphics;

export import :geometry;
export import :rendering;
export import :shaders;

import :impl_math;
import :impl_utils;
```

## Key Features Demonstrated

1. **Partition Syntax**: Using `:partition_name` syntax
2. **Selective Exports**: Controlling what gets exported from partitions
3. **Internal Partitions**: Implementation-only partitions
4. **Re-exports**: Primary module re-exporting partition interfaces
5. **Dependency Management**: How partitions depend on each other
6. **Template Partitions**: Organizing template code in partitions

## Advanced Patterns

### Template Partitions
```cpp
// graphics-templates.cppm
export module graphics:templates;

export template<typename T>
class Vector3 {
    T x, y, z;
public:
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    // ... methods
};
```

### Concept Exports
```cpp
// graphics-concepts.cppm
export module graphics:concepts;

export template<typename T>
concept Drawable = requires(T t) {
    t.draw();
    t.getBounds();
};
```

### Nested Partitions
```cpp
// graphics-rendering-opengl.cppm
export module graphics:rendering.opengl;

// Specialized OpenGL rendering functionality
```

## Building

```bash
# Create the project
cpp-scaffold partitions --template modules --build cmake --std cpp23 --tests --package vcpkg

# Build
cd partitions
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=23
cmake --build .

# Run
./partitions
```

## Usage Example

```cpp
import graphics;  // Imports all public partitions
import std;

int main() {
    using namespace graphics;
    
    // Use geometry partition
    geometry::Point3D point{1.0, 2.0, 3.0};
    geometry::Vector3D vector{0.0, 1.0, 0.0};
    
    // Use rendering partition
    rendering::Renderer renderer;
    renderer.initialize();
    
    // Use shaders partition
    auto shader = shaders::load_shader("vertex.glsl", "fragment.glsl");
    
    // Render scene
    renderer.render(point, vector, shader);
    
    return 0;
}
```

## Organization Benefits

### 1. Logical Separation
- Related functionality grouped together
- Clear module boundaries
- Easier to understand and maintain

### 2. Compilation Efficiency
- Only recompile affected partitions
- Better incremental builds
- Reduced compilation dependencies

### 3. Team Development
- Different teams can work on different partitions
- Clear ownership boundaries
- Reduced merge conflicts

### 4. Testing Isolation
- Test partitions independently
- Better test organization
- Easier to achieve full coverage

## Best Practices

### 1. Partition Design
- Keep partitions focused and cohesive
- Minimize dependencies between partitions
- Use implementation partitions for internal details

### 2. Naming Conventions
```cpp
module_name:public_partition     // Interface partition
module_name:impl_partition       // Implementation partition
module_name:detail_partition     // Internal details
```

### 3. Dependency Management
- Avoid circular dependencies between partitions
- Use forward declarations when possible
- Keep implementation partitions private

### 4. Export Strategy
- Export only what clients need
- Use selective exports from partitions
- Re-export through primary module interface

## Performance Considerations

- **Compilation Time**: Partitions can improve incremental builds
- **Binary Size**: Better dead code elimination
- **Runtime Performance**: No runtime overhead
- **Memory Usage**: More efficient symbol representation

## Compiler Requirements

- **C++23**: Full partition support
- **CMake 3.28+**: Stable modules support
- **Clang 16+**: Good partition support
- **MSVC 19.30+**: Full partition support
- **GCC 12+**: Basic partition support (improving)
