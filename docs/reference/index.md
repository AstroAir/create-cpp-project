# Reference

This section contains reference materials, changelogs, and additional resources for CPP-Scaffold.

## What's in This Section

### [Recent Enhancements](enhancements.md)

Detailed information about recent improvements, new features, and changes to CPP-Scaffold.

## Quick Reference

### Command Line Options

```bash
# Core options
cpp-scaffold PROJECT_NAME [OPTIONS]

Options:
  -t, --type <template>     Project template (console, lib, gui, etc.)
  -b, --build <system>      Build system (cmake, meson, bazel, etc.)
  -p, --package <manager>   Package manager (vcpkg, conan, spack, etc.)
  --test <framework>        Testing framework (gtest, catch2, doctest, etc.)
  --ci <system>            CI/CD system (github, gitlab, travis, etc.)
  --editor <config>        Editor configuration (vscode, clion, vs, etc.)
  --lang <language>        Interface language (english, chinese, etc.)
  -v, --verbose            Enable verbose output
  -h, --help              Show help information
  --version               Show version information
```

### Project Templates

| Template           | Description              | Use Case                |
| ------------------ | ------------------------ | ----------------------- |
| `console`          | Command-line application | CLI tools, utilities    |
| `lib`              | Static/shared library    | Reusable components     |
| `header-only-lib`  | Header-only library      | Template libraries      |
| `gui`              | GUI application          | Desktop applications    |
| `network`          | Network service          | Servers, clients        |
| `webservice`       | Web service              | REST APIs, web backends |
| `embedded`         | Embedded system          | IoT, microcontrollers   |
| `multi-executable` | Multiple executables     | Complex projects        |

### Build Systems

| Build System | Description                      | Best For                    |
| ------------ | -------------------------------- | --------------------------- |
| `cmake`      | Cross-platform build system      | Most projects (recommended) |
| `meson`      | Fast, user-friendly build system | Modern projects             |
| `bazel`      | Scalable build system            | Large codebases             |
| `xmake`      | Lua-based build system           | Rapid prototyping           |
| `premake`    | Lua-based project generator      | Game development            |
| `make`       | Traditional make system          | Simple projects             |
| `ninja`      | Fast build system                | Performance-critical builds |

### Package Managers

| Package Manager | Description                          | Platform Support      |
| --------------- | ------------------------------------ | --------------------- |
| `vcpkg`         | Microsoft's C++ package manager      | Windows, Linux, macOS |
| `conan`         | Cross-platform package manager       | Windows, Linux, macOS |
| `spack`         | Scientific computing package manager | Linux, macOS          |
| `hunter`        | CMake-driven package manager         | Windows, Linux, macOS |
| `none`          | No package manager                   | All platforms         |

### Testing Frameworks

| Framework | Description                | Features                   |
| --------- | -------------------------- | -------------------------- |
| `gtest`   | Google Test framework      | Comprehensive, widely used |
| `catch2`  | Modern C++ test framework  | Header-only, BDD style     |
| `doctest` | Lightweight test framework | Fast compilation           |
| `boost`   | Boost.Test framework       | Part of Boost libraries    |
| `none`    | No testing framework       | Manual testing only        |

### CI/CD Systems

| System     | Description    | Platform            |
| ---------- | -------------- | ------------------- |
| `github`   | GitHub Actions | GitHub              |
| `gitlab`   | GitLab CI/CD   | GitLab              |
| `travis`   | Travis CI      | GitHub              |
| `appveyor` | AppVeyor       | Windows focus       |
| `azure`    | Azure DevOps   | Microsoft ecosystem |
| `circleci` | CircleCI       | Cloud-based         |
| `none`     | No CI/CD setup | Manual builds       |

### Editor Configurations

| Editor   | Description             | Features                |
| -------- | ----------------------- | ----------------------- |
| `vscode` | Visual Studio Code      | IntelliSense, debugging |
| `clion`  | JetBrains CLion         | Advanced IDE features   |
| `vs`     | Visual Studio           | Windows development     |
| `vim`    | Vim/Neovim              | Terminal-based editing  |
| `emacs`  | GNU Emacs               | Extensible editor       |
| `none`   | No editor configuration | Manual setup            |

## Environment Variables

### Build Configuration

```bash
# Compiler selection
export CC=gcc
export CXX=g++

# CMake configuration
export CMAKE_BUILD_TYPE=Release
export CMAKE_INSTALL_PREFIX=/usr/local

# vcpkg configuration
export VCPKG_ROOT=/path/to/vcpkg
export VCPKG_DEFAULT_TRIPLET=x64-linux
```

### CPP-Scaffold Configuration

```bash
# Default template
export CPP_SCAFFOLD_DEFAULT_TEMPLATE=console

# Default build system
export CPP_SCAFFOLD_DEFAULT_BUILD=cmake

# Configuration directory
export CPP_SCAFFOLD_CONFIG_DIR=~/.config/cpp-scaffold

# Cache directory
export CPP_SCAFFOLD_CACHE_DIR=~/.cache/cpp-scaffold
```

## File Locations

### Configuration Files

| Platform | Location                                      |
| -------- | --------------------------------------------- |
| Linux    | `~/.config/cpp-scaffold/`                     |
| macOS    | `~/Library/Application Support/cpp-scaffold/` |
| Windows  | `%APPDATA%\cpp-scaffold\`                     |

### Cache Files

| Platform | Location                             |
| -------- | ------------------------------------ |
| Linux    | `~/.cache/cpp-scaffold/`             |
| macOS    | `~/Library/Caches/cpp-scaffold/`     |
| Windows  | `%LOCALAPPDATA%\cpp-scaffold\Cache\` |

### Template Files

| Platform | Location                                             |
| -------- | ---------------------------------------------------- |
| System   | `/usr/share/cpp-scaffold/templates/` (Linux)         |
| System   | `/usr/local/share/cpp-scaffold/templates/` (macOS)   |
| System   | `C:\Program Files\cpp-scaffold\templates\` (Windows) |
| User     | `~/.local/share/cpp-scaffold/templates/`             |

## Exit Codes

| Code | Description           |
| ---- | --------------------- |
| 0    | Success               |
| 1    | General error         |
| 2    | Invalid arguments     |
| 3    | Template not found    |
| 4    | Configuration error   |
| 5    | File system error     |
| 6    | Build system error    |
| 7    | Package manager error |

## Supported Platforms

### Operating Systems

| OS      | Versions                 | Architecture |
| ------- | ------------------------ | ------------ |
| Windows | 10, 11                   | x64, ARM64   |
| macOS   | 10.15+                   | x64, ARM64   |
| Linux   | Ubuntu 18.04+, CentOS 7+ | x64, ARM64   |

### Compilers

| Compiler | Minimum Version | C++ Standard |
| -------- | --------------- | ------------ |
| GCC      | 7.0             | C++17        |
| Clang    | 6.0             | C++17        |
| MSVC     | 2017 (15.7)     | C++17        |

### Dependencies

| Dependency | Minimum Version | Purpose                       |
| ---------- | --------------- | ----------------------------- |
| CMake      | 3.14            | Build system                  |
| Git        | 2.0             | Version control               |
| Ninja      | 1.8             | Build acceleration (optional) |

## Troubleshooting Quick Reference

### Common Issues

| Issue                 | Solution                             |
| --------------------- | ------------------------------------ |
| Template not found    | Check template name spelling         |
| Build system error    | Verify CMake/build tool installation |
| Package manager error | Check vcpkg/conan setup              |
| Permission denied     | Run with appropriate permissions     |
| Out of disk space     | Free up disk space                   |

### Debug Commands

```bash
# Enable verbose output
cpp-scaffold --verbose MyProject

# Check version
cpp-scaffold --version

# List available templates
cpp-scaffold --list-templates

# Validate configuration
cpp-scaffold --validate-config
```

## Additional Resources

### Documentation

- [Getting Started Guide](../getting-started/index.md)
- [User Guide](../user-guide/index.md)
- [Developer Guide](../developer-guide/index.md)
- [API Reference](../api-reference/index.md)

### Community

- [GitHub Repository](https://github.com/cpp-scaffold/cpp-scaffold)
- [Issue Tracker](https://github.com/cpp-scaffold/cpp-scaffold/issues)
- [Discussions](https://github.com/cpp-scaffold/cpp-scaffold/discussions)

### External Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [vcpkg Documentation](https://vcpkg.io/en/docs.html)
- [Conan Documentation](https://docs.conan.io/)
- [Modern C++ Guidelines](https://github.com/isocpp/CppCoreGuidelines)
