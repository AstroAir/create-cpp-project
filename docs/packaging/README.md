# Packaging and Distribution System

This document provides an overview of the comprehensive packaging and distribution system implemented for cpp-scaffold.

## Overview

The packaging system provides professional-grade distribution capabilities with support for multiple platforms, package formats, installation methods, and deployment scenarios.

## Features

### Package Formats
- **Windows**: NSIS installers, MSI packages, ZIP archives
- **macOS**: DMG disk images, Bundle applications, TAR.GZ archives
- **Linux**: DEB packages, RPM packages, AppImage, TAR.GZ archives

### Advanced Capabilities
- **Code Signing**: Authenticode (Windows), Code signing (macOS), GPG signing (Linux)
- **Package Validation**: Automated integrity and functionality testing
- **Installation Scripts**: Cross-platform installers with GUI support
- **Package Managers**: Integration with Homebrew, Chocolatey, APT, YUM, etc.
- **Release Automation**: CI/CD workflows for multi-platform builds

## Architecture

### Core Components

1. **Packaging.cmake**: Enhanced CPack configuration with modern package formats
2. **PackageTesting.cmake**: Comprehensive package validation framework
3. **Installation Scripts**: Cross-platform installation with multiple methods
4. **Signing Infrastructure**: Code signing and verification system
5. **Resource Files**: Icons, metadata, and platform-specific resources

### Directory Structure
```
├── cmake/
│   ├── modules/
│   │   ├── Packaging.cmake          # Main packaging configuration
│   │   └── PackageTesting.cmake     # Testing framework
│   └── scripts/
│       └── create_appimage.cmake    # AppImage creation script
├── docs/
│   ├── installation/
│   │   └── README.md               # Installation guide
│   └── packaging/
│       ├── README.md               # This file
│       ├── code-signing.md         # Code signing guide
│       └── testing.md              # Testing guide
├── packaging/
│   ├── debian/                     # Debian package scripts
│   └── rpm/                        # RPM package scripts
├── resources/                      # Icons, metadata, resources
├── scripts/
│   ├── install.sh                  # Unix installation script
│   ├── install.ps1                 # PowerShell installation script
│   ├── web-install.sh              # Web-based installer
│   ├── setup_signing.sh            # Signing setup (Unix)
│   ├── setup_signing.ps1           # Signing setup (PowerShell)
│   ├── test_packages.sh            # Package testing (Unix)
│   └── test_packages.ps1           # Package testing (PowerShell)
```

## Usage

### Building Packages

#### Basic Package Creation
```bash
# Configure and build
cmake --preset release
cmake --build --preset release

# Create packages
cmake --build build --target package
```

#### Advanced Package Creation
```bash
# Enable package testing
export ENABLE_PACKAGE_TESTING=1

# Enable package signing
export ENABLE_PACKAGE_SIGNING=1
export WINDOWS_CERT_PATH="/path/to/cert.p12"
export MACOS_IDENTITY="Developer ID Application: Your Name"
export LINUX_GPG_KEY="12345678"

# Build and test packages
cmake --build build --target test_packages
```

### Package Testing

#### Automated Testing
```bash
# Run comprehensive tests
./scripts/test_packages.sh --install-test --signature-test --verbose

# Windows
.\scripts\test_packages.ps1 -InstallTest -SignatureTest -Verbose
```

#### Manual Testing
```bash
# Validate packages
cmake --build build --target validate_packages

# Test installation
cmake --build build --target installation_tests

# Verify signatures
cmake --build build --target verify_signatures
```

### Installation

#### End-User Installation
```bash
# One-line install
curl -sSL https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/web-install.sh | bash

# Package managers
brew install cpp-scaffold          # macOS/Linux
choco install cpp-scaffold         # Windows
```

#### Developer Installation
```bash
# Download installer
curl -O https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/install.sh
chmod +x install.sh

# Install with options
./install.sh --dir ~/.local --method binary --verbose
```

## Configuration

### CMake Options
```cmake
# Enable package testing
set(ENABLE_PACKAGE_TESTING ON)

# Configure signing
set(ENABLE_PACKAGE_SIGNING ON)
set(WINDOWS_CERT_PATH "/path/to/cert.p12")
set(MACOS_IDENTITY "Developer ID Application: Your Name")
set(LINUX_GPG_KEY "12345678")

# Package metadata
set(CPACK_PACKAGE_VENDOR "CPP-Scaffold Team")
set(CPACK_PACKAGE_DESCRIPTION "C++ Project Scaffolding Tool")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/cpp-scaffold/cpp-scaffold")
```

### Environment Variables
```bash
# Packaging
export ENABLE_PACKAGE_TESTING=1
export ENABLE_PACKAGE_SIGNING=1

# Signing credentials
export WINDOWS_CERT_PATH="/path/to/cert.p12"
export WINDOWS_CERT_PASSWORD="password"
export MACOS_IDENTITY="Developer ID Application: Your Name"
export APPLE_ID="your@apple.id"
export APPLE_PASSWORD="app-specific-password"
export LINUX_GPG_KEY="12345678"

# Installation
export CPP_SCAFFOLD_CONFIG_DIR="$HOME/.config/cpp-scaffold"
export CPP_SCAFFOLD_CACHE_DIR="$HOME/.cache/cpp-scaffold"
```

## Platform-Specific Details

### Windows
- **Formats**: NSIS (.exe), WiX MSI (.msi), ZIP (.zip)
- **Signing**: Authenticode with timestamp servers
- **Features**: Registry entries, PATH modification, Start Menu shortcuts
- **Requirements**: Windows 10+, PowerShell 5.1+

### macOS
- **Formats**: DMG (.dmg), Bundle (.app), TAR.GZ (.tar.gz)
- **Signing**: Code signing with Developer ID, notarization
- **Features**: Drag-and-drop installation, Gatekeeper compatibility
- **Requirements**: macOS 10.14+, Xcode Command Line Tools

### Linux
- **Formats**: DEB (.deb), RPM (.rpm), AppImage (.AppImage), TAR.GZ (.tar.gz)
- **Signing**: GPG signatures for packages
- **Features**: Desktop integration, system service support
- **Requirements**: glibc 2.17+, modern Linux distribution

## CI/CD Integration

### GitHub Actions
The packaging system integrates with GitHub Actions for automated builds:

```yaml
name: Build and Package

on: [push, pull_request, release]

jobs:
  package:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup signing
      run: |
        if [ "$RUNNER_OS" == "Linux" ]; then
          ./scripts/setup_signing.sh --platform linux --linux-gpg-key "${{ secrets.LINUX_GPG_KEY }}"
        elif [ "$RUNNER_OS" == "macOS" ]; then
          ./scripts/setup_signing.sh --platform macos --macos-identity "${{ secrets.MACOS_IDENTITY }}"
        else
          .\scripts\setup_signing.ps1 -Platform windows -WindowsCert "${{ secrets.WINDOWS_CERT_PATH }}"
        fi
      shell: bash
    
    - name: Build packages
      run: |
        cmake --preset release
        cmake --build --preset release --target package
    
    - name: Test packages
      run: |
        if [ "$RUNNER_OS" == "Windows" ]; then
          .\scripts\test_packages.ps1 -InstallTest
        else
          ./scripts/test_packages.sh --install-test
        fi
      shell: bash
    
    - name: Upload packages
      uses: actions/upload-artifact@v3
      with:
        name: packages-${{ matrix.os }}
        path: build/*.{exe,msi,dmg,deb,rpm,AppImage,zip,tar.gz}
```

### Release Process
1. **Automated Builds**: Triggered on tags and releases
2. **Multi-Platform**: Builds for Windows, macOS, and Linux
3. **Testing**: Comprehensive package validation
4. **Signing**: Automatic code signing with stored credentials
5. **Distribution**: Upload to GitHub Releases and package repositories

## Security

### Code Signing
- **Windows**: Authenticode signatures with trusted certificates
- **macOS**: Developer ID signing and notarization
- **Linux**: GPG signatures for package integrity

### Best Practices
- Secure credential storage in CI/CD systems
- Regular key rotation and certificate renewal
- Signature verification in installation scripts
- Secure distribution channels (HTTPS, signed repositories)

## Troubleshooting

### Common Issues
1. **Build Failures**: Check CMake configuration and dependencies
2. **Signing Errors**: Verify certificates and credentials
3. **Package Validation**: Review package format requirements
4. **Installation Issues**: Check platform compatibility and permissions

### Debug Mode
Enable verbose output for troubleshooting:
```bash
export CPACK_VERBATIM_VARIABLES=ON
cmake --build build --target package --verbose
```

### Support Resources
- [Installation Guide](../installation/README.md)
- [Code Signing Guide](code-signing.md)
- [Testing Guide](testing.md)
- [GitHub Issues](https://github.com/cpp-scaffold/cpp-scaffold/issues)

## Contributing

### Adding New Package Formats
1. Extend `Packaging.cmake` with new CPack generator
2. Add platform-specific configuration
3. Update testing framework
4. Add documentation and examples

### Improving Installation Scripts
1. Test on target platforms
2. Follow cross-platform best practices
3. Add comprehensive error handling
4. Update documentation

### Enhancing CI/CD
1. Add new platform support
2. Improve testing coverage
3. Optimize build times
4. Enhance security measures

## Future Enhancements

### Planned Features
- **Container Images**: Docker and Podman support
- **Cloud Packages**: AWS, Azure, GCP marketplace integration
- **Mobile Platforms**: iOS and Android support
- **Package Repositories**: Self-hosted repository management
- **Update System**: Automatic update mechanisms

### Community Requests
- Additional package managers (Snap, Flatpak, etc.)
- Enhanced GUI installers
- Improved dependency management
- Better integration with IDEs and editors

---

For detailed information on specific aspects of the packaging system, refer to the specialized guides:
- [Installation Guide](../installation/README.md)
- [Code Signing Guide](code-signing.md)
- [Testing Guide](testing.md)
