# Project Templates

CPP-Scaffold provides a comprehensive set of project templates designed to jumpstart your C++ development with modern best practices and industry-standard configurations.

## Available Templates

### Console Application (`console`)

**Best for**: Command-line tools, utilities, system applications

**Features**:
- Argument parsing with validation
- Logging configuration
- Cross-platform compatibility
- Signal handling and graceful shutdown
- Configuration file support

**Generated Structure**:
```
MyConsoleApp/
├── src/
│   ├── main.cpp
│   ├── cli/
│   │   ├── argument_parser.h
│   │   └── argument_parser.cpp
│   └── core/
│       ├── application.h
│       └── application.cpp
├── include/
│   └── MyConsoleApp/
│       └── version.h
└── tests/
    └── test_application.cpp
```

**Example Usage**:
```bash
cpp-scaffold MyTool --type console --build cmake --package vcpkg --test gtest
```

### Library (`lib`)

**Best for**: Reusable components, SDKs, shared libraries

**Features**:
- Symbol export/import macros for Windows/Linux
- pkg-config file generation
- Version management and semantic versioning
- Installation and packaging support
- Comprehensive API documentation

**Generated Structure**:
```
MyLibrary/
├── include/
│   └── MyLibrary/
│       ├── MyLibrary.h
│       └── export.h
├── src/
│   └── MyLibrary.cpp
├── cmake/
│   └── MyLibraryConfig.cmake.in
└── tests/
    └── test_library.cpp
```

### Header-Only Library (`header-only-lib`)

**Best for**: Template libraries, utility collections, lightweight libraries

**Features**:
- Single-header distribution option
- Comprehensive documentation with examples
- Benchmark integration for performance testing
- Continuous integration for multiple compilers

**Generated Structure**:
```
MyHeaderLib/
├── include/
│   └── MyHeaderLib/
│       ├── MyHeaderLib.hpp
│       └── detail/
│           └── implementation.hpp
├── examples/
│   └── basic_usage.cpp
├── benchmarks/
│   └── performance_tests.cpp
└── tests/
    └── test_header_lib.cpp
```

### GUI Application (`gui`)

**Best for**: Desktop applications, cross-platform GUI software

**Features**:
- Framework-specific project structure (Qt, SFML, etc.)
- Resource file management
- Cross-platform deployment scripts
- UI testing framework integration

**Supported Frameworks**:
- Qt (recommended)
- SFML
- Dear ImGui
- wxWidgets
- FLTK

**Generated Structure**:
```
MyGuiApp/
├── src/
│   ├── main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   └── mainwindow.ui
├── resources/
│   ├── icons/
│   └── resources.qrc
└── tests/
    └── test_gui.cpp
```

### Network Service (`network`)

**Best for**: Network servers, clients, distributed systems

**Features**:
- Asynchronous I/O with modern C++ patterns
- Protocol buffer integration
- Load balancing and scaling considerations
- Security best practices implementation

**Generated Structure**:
```
MyNetworkService/
├── src/
│   ├── main.cpp
│   ├── server/
│   │   ├── tcp_server.h
│   │   └── tcp_server.cpp
│   └── protocol/
│       ├── message.h
│       └── message.cpp
├── proto/
│   └── service.proto
└── tests/
    └── test_server.cpp
```

### Web Service (`webservice`)

**Best for**: REST APIs, web backends, microservices

**Features**:
- RESTful API structure with routing
- JSON request/response handling
- Authentication and authorization middleware
- Database integration patterns
- OpenAPI/Swagger documentation

**Generated Structure**:
```
MyWebService/
├── src/
│   ├── main.cpp
│   ├── api/
│   │   ├── router.h
│   │   ├── router.cpp
│   │   └── handlers/
│   └── database/
│       ├── connection.h
│       └── models/
├── config/
│   └── service.json
└── docker/
    ├── Dockerfile
    └── docker-compose.yml
```

### Embedded System (`embedded`)

**Best for**: IoT devices, microcontrollers, resource-constrained systems

**Features**:
- Resource-constrained optimizations
- Real-time system support
- Hardware abstraction layer
- Cross-compilation support
- Debugging and profiling tools

**Generated Structure**:
```
MyEmbeddedProject/
├── src/
│   ├── main.cpp
│   ├── hal/
│   │   ├── gpio.h
│   │   └── uart.h
│   └── drivers/
├── linker/
│   └── memory.ld
├── startup/
│   └── startup.s
└── config/
    └── board.h
```

### Multi-Executable (`multi-executable`)

**Best for**: Complex projects, tool suites, modular applications

**Features**:
- Shared libraries and common code
- Modular architecture
- Inter-component communication
- Unified build system

**Generated Structure**:
```
MyProjectSuite/
├── common/
│   ├── include/
│   └── src/
├── tools/
│   ├── tool1/
│   ├── tool2/
│   └── tool3/
├── services/
│   ├── service1/
│   └── service2/
└── tests/
    ├── common/
    ├── tools/
    └── services/
```

## Template Selection Guide

### By Project Type

| Project Type | Recommended Template | Alternative |
|--------------|---------------------|-------------|
| CLI Tool | `console` | `multi-executable` |
| Reusable Library | `lib` | `header-only-lib` |
| Desktop App | `gui` | `console` |
| Web API | `webservice` | `network` |
| Game | `gui` | `console` |
| IoT Device | `embedded` | `console` |
| Microservice | `webservice` | `network` |

### By Complexity

| Complexity | Templates |
|------------|-----------|
| **Simple** | `console`, `header-only-lib` |
| **Medium** | `lib`, `gui`, `network` |
| **Complex** | `webservice`, `embedded`, `multi-executable` |

### By Target Platform

| Platform | Recommended Templates |
|----------|----------------------|
| **Cross-platform** | `console`, `lib`, `gui` |
| **Server/Cloud** | `webservice`, `network` |
| **Desktop** | `gui`, `console` |
| **Embedded** | `embedded`, `console` |
| **Mobile** | `gui` (with appropriate framework) |

## Template Customization

### Template Variables

Most templates support customization through variables:

```bash
# Customize namespace
cpp-scaffold MyLib --type lib --namespace MyCompany::Utils

# Set version
cpp-scaffold MyApp --type console --version 2.1.0

# Configure features
cpp-scaffold MyService --type webservice --database postgresql --auth jwt
```

### Common Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `project_name` | Project name | Command line argument |
| `namespace` | C++ namespace | Project name |
| `version` | Project version | "1.0.0" |
| `author` | Author name | Git user name |
| `email` | Author email | Git user email |
| `description` | Project description | Generated |
| `license` | License type | "MIT" |

### Framework-Specific Options

#### GUI Templates
```bash
# Qt application
cpp-scaffold MyApp --type gui --framework qt --qt-version 6

# SFML game
cpp-scaffold MyGame --type gui --framework sfml --sfml-modules graphics,audio
```

#### Web Service Templates
```bash
# With database
cpp-scaffold MyAPI --type webservice --database postgresql --orm sqlpp11

# With authentication
cpp-scaffold MyAPI --type webservice --auth jwt --cors enabled
```

## Best Practices

### Template Selection
1. **Start simple** - Choose the simplest template that meets your needs
2. **Consider growth** - Think about future requirements
3. **Match your expertise** - Don't choose overly complex templates for learning
4. **Platform alignment** - Ensure template supports your target platforms

### Customization
1. **Use meaningful names** - Choose descriptive project and namespace names
2. **Set proper versions** - Use semantic versioning from the start
3. **Configure licensing** - Choose appropriate license for your project
4. **Plan dependencies** - Consider package manager and dependency strategy

### Post-Generation
1. **Review generated code** - Understand the structure before modifying
2. **Update documentation** - Customize README and docs for your project
3. **Configure CI/CD** - Adapt generated CI configuration to your needs
4. **Test immediately** - Ensure the generated project builds and tests pass

## Creating Custom Templates

For advanced users, CPP-Scaffold supports custom templates. See the [Advanced Features Tutorial](../tutorials/advanced-features.md) for detailed instructions on creating your own templates.

### Template Development
1. **Study existing templates** - Learn from built-in templates
2. **Follow conventions** - Use consistent naming and structure
3. **Test thoroughly** - Ensure templates work across platforms
4. **Document well** - Provide clear usage instructions

## Troubleshooting Templates

### Common Issues

| Issue | Solution |
|-------|----------|
| Template not found | Check spelling, use `--list-templates` |
| Build failures | Verify dependencies are installed |
| Missing features | Check template documentation for supported features |
| Platform issues | Ensure template supports your platform |

### Getting Help

- **Template documentation** - Check template-specific docs
- **Community support** - Ask in GitHub Discussions
- **Issue reporting** - Report bugs in GitHub Issues
- **Custom templates** - See developer guide for creating templates

Ready to create your project? Choose a template and start building!
