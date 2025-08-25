# Advanced Features Tutorial

Explore advanced CPP-Scaffold features including custom templates, configuration profiles, and CI/CD integration.

## What You'll Learn

This tutorial covers:

- Creating custom project templates
- Managing configuration profiles
- Setting up automated CI/CD pipelines
- Integrating with package managers
- Advanced build system configurations
- Custom IDE integrations

## Prerequisites

- Experience with basic CPP-Scaffold usage
- Understanding of C++ project structure
- Familiarity with Git and CI/CD concepts
- Knowledge of CMake or other build systems

## Part 1: Custom Templates

### Creating a Custom Template

Create a custom template for microservice projects:

```bash
# Create template directory
mkdir -p ~/.local/share/cpp-scaffold/templates/microservice

# Create template configuration
cat > ~/.local/share/cpp-scaffold/templates/microservice/template.json << 'EOF'
{
    "name": "microservice",
    "description": "Microservice with REST API and database integration",
    "version": "1.0.0",
    "author": "Your Name",
    "variables": {
        "service_name": {
            "type": "string",
            "description": "Name of the microservice",
            "default": "{{project_name}}"
        },
        "api_version": {
            "type": "string",
            "description": "API version",
            "default": "v1"
        },
        "database_type": {
            "type": "choice",
            "description": "Database type",
            "choices": ["postgresql", "mysql", "sqlite"],
            "default": "postgresql"
        }
    },
    "dependencies": {
        "vcpkg": [
            "cpprestsdk",
            "nlohmann-json",
            "spdlog"
        ],
        "conan": [
            "cpprestsdk/2.10.18",
            "nlohmann_json/3.11.2",
            "spdlog/1.12.0"
        ]
    }
}
EOF
```

### Template Structure

```
microservice/
├── template.json
├── CMakeLists.txt.template
├── src/
│   ├── main.cpp.template
│   ├── api/
│   │   ├── router.h.template
│   │   └── router.cpp.template
│   └── database/
│       ├── connection.h.template
│       └── connection.cpp.template
├── tests/
│   └── test_api.cpp.template
└── docker/
    ├── Dockerfile.template
    └── docker-compose.yml.template
```

### Template Implementation

```cpp title="src/main.cpp.template"
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <spdlog/spdlog.h>
#include "api/router.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

int main() {
    spdlog::info("Starting {{service_name}} microservice");
    
    try {
        // Create HTTP listener
        http_listener listener("http://0.0.0.0:8080");
        
        // Setup routes
        api::Router router;
        router.setup_routes(listener);
        
        // Start listening
        listener.open().wait();
        spdlog::info("{{service_name}} listening on port 8080");
        
        // Keep running
        std::string line;
        std::getline(std::cin, line);
        
        listener.close().wait();
    }
    catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
    
    return 0;
}
```

## Part 2: Configuration Profiles

### Creating Profiles

```bash
# Create a web development profile
cpp-scaffold --save-profile webdev \
  --type microservice \
  --build cmake \
  --package vcpkg \
  --test gtest \
  --ci github \
  --editor vscode

# Create an embedded development profile
cpp-scaffold --save-profile embedded \
  --type embedded \
  --build cmake \
  --package none \
  --test catch2 \
  --ci gitlab \
  --editor clion
```

### Using Profiles

```bash
# Use saved profile
cpp-scaffold MyWebService --profile webdev

# Override profile settings
cpp-scaffold MyEmbeddedProject --profile embedded --package conan
```

### Profile Configuration File

```json title="~/.config/cpp-scaffold/profiles.json"
{
    "webdev": {
        "template": "microservice",
        "build_system": "cmake",
        "package_manager": "vcpkg",
        "test_framework": "gtest",
        "ci_system": "github",
        "editor": "vscode",
        "options": {
            "enable_docker": true,
            "api_version": "v1",
            "database_type": "postgresql"
        }
    },
    "embedded": {
        "template": "embedded",
        "build_system": "cmake",
        "package_manager": "none",
        "test_framework": "catch2",
        "ci_system": "gitlab",
        "editor": "clion",
        "options": {
            "target_platform": "arm-cortex-m4",
            "optimization": "size"
        }
    }
}
```

## Part 3: Advanced CI/CD Integration

### Multi-Platform CI Configuration

```yaml title=".github/workflows/ci.yml"
name: CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-and-test:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Debug, Release]
        compiler: [gcc, clang, msvc]
        exclude:
          - os: ubuntu-latest
            compiler: msvc
          - os: macos-latest
            compiler: msvc
          - os: windows-latest
            compiler: gcc

    runs-on: ${{ matrix.os }}

    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive

    - name: Setup vcpkg
      uses: lukka/run-vcpkg@v11
      with:
        vcpkgGitCommitId: 'latest'

    - name: Configure CMake
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
          -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}

    - name: Test
      run: ctest --test-dir build --build-config ${{ matrix.build_type }}

    - name: Package
      if: matrix.build_type == 'Release'
      run: |
        cmake --build build --target package
        
    - name: Upload Artifacts
      if: matrix.build_type == 'Release'
      uses: actions/upload-artifact@v3
      with:
        name: packages-${{ matrix.os }}
        path: build/packages/*
```

### Docker Integration

```dockerfile title="docker/Dockerfile"
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    && rm -rf /var/lib/apt/lists/*

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh

# Set environment
ENV VCPKG_ROOT=/opt/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"

# Copy source
WORKDIR /app
COPY . .

# Build application
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
    && cmake --build build

# Runtime stage
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=0 /app/build/{{service_name}} /usr/local/bin/
EXPOSE 8080
CMD ["{{service_name}}"]
```

## Part 4: Package Manager Integration

### Advanced vcpkg Configuration

```json title="vcpkg.json"
{
    "name": "{{project_name}}",
    "version": "1.0.0",
    "description": "{{project_description}}",
    "dependencies": [
        {
            "name": "cpprestsdk",
            "features": ["websockets"]
        },
        {
            "name": "nlohmann-json",
            "version>=": "3.11.0"
        },
        {
            "name": "spdlog",
            "features": ["fmt_external"]
        }
    ],
    "features": {
        "database": {
            "description": "Database support",
            "dependencies": [
                {
                    "name": "libpqxx",
                    "platform": "!windows"
                },
                {
                    "name": "sqlite3"
                }
            ]
        },
        "testing": {
            "description": "Testing support",
            "dependencies": [
                "gtest",
                "gmock"
            ]
        }
    }
}
```

### Conan Integration

```python title="conanfile.py"
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class {{ProjectName}}Conan(ConanFile):
    name = "{{project_name}}"
    version = "1.0.0"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_database": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_database": True
    }
    
    def requirements(self):
        self.requires("cpprestsdk/2.10.18")
        self.requires("nlohmann_json/3.11.2")
        self.requires("spdlog/1.12.0")
        
        if self.options.with_database:
            self.requires("libpqxx/7.7.4")
    
    def build_requirements(self):
        self.test_requires("gtest/1.14.0")
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["WITH_DATABASE"] = self.options.with_database
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
```

## Part 5: IDE Integration

### Advanced VSCode Configuration

```json title=".vscode/settings.json"
{
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "gcc-x64",
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja",
    "files.associations": {
        "*.template": "cpp"
    },
    "editor.formatOnSave": true,
    "C_Cpp.clang_format_style": "file",
    "cpptools.formatting": "clangFormat"
}
```

## What You've Learned

✅ Created custom project templates  
✅ Managed configuration profiles  
✅ Set up advanced CI/CD pipelines  
✅ Integrated with package managers  
✅ Configured IDE integrations  
✅ Implemented Docker containerization  

## Next Steps

- **Template marketplace**: Share templates with the community
- **Plugin system**: Develop CPP-Scaffold plugins
- **Custom integrations**: Integrate with your organization's tools
- **Performance optimization**: Optimize template generation

## Related Resources

- [Template Development Guide](../developer-guide/extending.md)
- [Configuration Reference](../reference/index.md)
- [API Documentation](../api-reference/api.md)
