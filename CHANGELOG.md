# Changelog

All notable changes to CPP-Scaffold will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Enhanced .gitignore with comprehensive C++ development exclusions
- .clang-format configuration for consistent code formatting
- GitHub Actions CI/CD workflows for automated testing and building
- CONTRIBUTORS.md file for recognizing project contributors
- CHANGELOG.md file for tracking project changes

### Changed

- Improved project structure with modern development practices

### Deprecated

- Nothing

### Removed

- Nothing

### Fixed

- Nothing

### Security

- Added CodeQL security analysis workflow

## [1.0.0] - YYYY-MM-DD

### Added

- Initial release of CPP-Scaffold
- Multiple project templates (console, library, GUI, network, embedded, web service)
- Build system support (CMake, Meson, Bazel, XMake, Premake, Make, Ninja)
- Package manager integration (vcpkg, Conan, Spack, Hunter)
- Testing framework support (Google Test, Catch2, Doctest, Boost.Test)
- IDE integration (VSCode, CLion, Visual Studio, Vim, Emacs)
- Multi-language support (English, Chinese, Spanish, Japanese, German, French)
- Interactive wizard with styled terminal output
- Comprehensive documentation system

### Changed

- Nothing (initial release)

### Deprecated

- Nothing

### Removed

- Nothing

### Fixed

- Nothing

### Security

- Nothing

---

## Release Notes Format

Each release should include the following sections as applicable:

### Added

- New features and capabilities

### Changed

- Changes in existing functionality

### Deprecated

- Soon-to-be removed features

### Removed

- Features removed in this release

### Fixed

- Bug fixes

### Security

- Security improvements and vulnerability fixes

## Version Numbering

This project follows [Semantic Versioning](https://semver.org/):

- **MAJOR** version when you make incompatible API changes
- **MINOR** version when you add functionality in a backwards compatible manner
- **PATCH** version when you make backwards compatible bug fixes

## Release Process

1. Update this CHANGELOG.md with the new version and changes
2. Update version numbers in CMakeLists.txt and other relevant files
3. Create a git tag with the version number (e.g., `v1.0.0`)
4. Push the tag to trigger the release workflow
5. GitHub Actions will automatically create a release with binaries

## Contributing to the Changelog

When contributing to the project:

1. Add your changes to the [Unreleased] section
2. Follow the format established in this file
3. Include a brief description of your changes
4. Reference any related issues or pull requests

For more information about contributing, see [CONTRIBUTING.md](documents/developer-guide/CONTRIBUTING.md).
