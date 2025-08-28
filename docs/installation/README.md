# Installation Guide

This guide provides comprehensive instructions for installing cpp-scaffold across different platforms and using various installation methods.

## Quick Installation

### One-Line Install (Recommended)

**Linux/macOS:**
```bash
curl -sSL https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/web-install.sh | bash
```

**Windows (PowerShell):**
```powershell
iwr -useb https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/install.ps1 | iex
```

### Package Managers

**Homebrew (macOS/Linux):**
```bash
brew install cpp-scaffold
```

**Chocolatey (Windows):**
```cmd
choco install cpp-scaffold
```

**Winget (Windows):**
```cmd
winget install cpp-scaffold
```

**Scoop (Windows):**
```cmd
scoop install cpp-scaffold
```

## Installation Methods

### 1. Automated Installer Scripts

The automated installer scripts provide the most user-friendly installation experience with support for multiple installation methods and platforms.

#### Features
- **Cross-platform**: Works on Windows, macOS, and Linux
- **Multiple methods**: Package manager, binary, or source installation
- **GUI support**: Optional graphical installation interface
- **Flexible**: Customizable installation directory and options
- **Safe**: Dry-run mode and verification checks

#### Usage

**Download and run:**
```bash
# Linux/macOS
curl -O https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/install.sh
chmod +x install.sh
./install.sh --help

# Windows
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/cpp-scaffold/cpp-scaffold/main/scripts/install.ps1" -OutFile "install.ps1"
.\install.ps1 -Help
```

**Common options:**
```bash
# Install to custom directory
./install.sh --dir ~/.local

# Force specific installation method
./install.sh --method binary

# Use GUI mode (if available)
./install.sh --gui

# Dry run (show what would be done)
./install.sh --dry-run

# Verbose output
./install.sh --verbose
```

### 2. Package Managers

Package managers provide the easiest installation and update experience.

#### Homebrew (macOS/Linux)
```bash
# Install
brew install cpp-scaffold

# Update
brew upgrade cpp-scaffold

# Uninstall
brew uninstall cpp-scaffold
```

#### Chocolatey (Windows)
```cmd
# Install
choco install cpp-scaffold

# Update
choco upgrade cpp-scaffold

# Uninstall
choco uninstall cpp-scaffold
```

#### APT (Debian/Ubuntu)
```bash
# Add repository
curl -fsSL https://packages.cpp-scaffold.org/gpg | sudo apt-key add -
echo "deb https://packages.cpp-scaffold.org/apt stable main" | sudo tee /etc/apt/sources.list.d/cpp-scaffold.list

# Install
sudo apt update
sudo apt install cpp-scaffold
```

#### YUM/DNF (RHEL/CentOS/Fedora)
```bash
# Add repository
sudo tee /etc/yum.repos.d/cpp-scaffold.repo << EOF
[cpp-scaffold]
name=CPP-Scaffold Repository
baseurl=https://packages.cpp-scaffold.org/rpm
enabled=1
gpgcheck=1
gpgkey=https://packages.cpp-scaffold.org/gpg
EOF

# Install
sudo yum install cpp-scaffold  # or dnf install cpp-scaffold
```

### 3. Binary Downloads

Pre-compiled binaries are available for direct download and installation.

#### Download Links
- **Windows**: [cpp-scaffold-1.0.0-windows-x86_64.zip](https://github.com/cpp-scaffold/cpp-scaffold/releases/latest/download/cpp-scaffold-1.0.0-windows-x86_64.zip)
- **macOS**: [cpp-scaffold-1.0.0-macos-x86_64.tar.gz](https://github.com/cpp-scaffold/cpp-scaffold/releases/latest/download/cpp-scaffold-1.0.0-macos-x86_64.tar.gz)
- **Linux**: [cpp-scaffold-1.0.0-linux-x86_64.tar.gz](https://github.com/cpp-scaffold/cpp-scaffold/releases/latest/download/cpp-scaffold-1.0.0-linux-x86_64.tar.gz)

#### Manual Installation
```bash
# Download and extract
wget https://github.com/cpp-scaffold/cpp-scaffold/releases/latest/download/cpp-scaffold-1.0.0-linux-x86_64.tar.gz
tar -xzf cpp-scaffold-1.0.0-linux-x86_64.tar.gz

# Install
sudo cp cpp-scaffold /usr/local/bin/
sudo chmod +x /usr/local/bin/cpp-scaffold

# Verify
cpp-scaffold --version
```

### 4. Build from Source

Building from source provides the most flexibility and ensures compatibility with your specific system.

#### Prerequisites
- **CMake** 3.20 or later
- **Ninja** build system
- **Git**
- **C++17** compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)

#### Build Instructions
```bash
# Clone repository
git clone https://github.com/cpp-scaffold/cpp-scaffold.git
cd cpp-scaffold

# Configure build
cmake --preset release

# Build
cmake --build --preset release

# Install
cmake --install build --prefix /usr/local

# Verify
cpp-scaffold --version
```

## Platform-Specific Instructions

### Windows

#### System Requirements
- Windows 10 or later
- PowerShell 5.1 or later (PowerShell Core 6+ recommended)

#### Installation Options
1. **Chocolatey** (Recommended)
2. **Winget**
3. **Scoop**
4. **Direct download**
5. **PowerShell installer script**

#### PATH Configuration
The installer automatically adds cpp-scaffold to your PATH. If needed, manually add:
```
C:\Program Files\cpp-scaffold\bin
```

### macOS

#### System Requirements
- macOS 10.14 (Mojave) or later
- Xcode Command Line Tools

#### Installation Options
1. **Homebrew** (Recommended)
2. **Direct download**
3. **Shell installer script**

#### Installation via Homebrew
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install cpp-scaffold
brew install cpp-scaffold
```

### Linux

#### System Requirements
- Modern Linux distribution (Ubuntu 18.04+, CentOS 7+, etc.)
- glibc 2.17 or later

#### Installation Options
1. **Package manager** (APT, YUM, DNF, Pacman)
2. **AppImage** (Universal)
3. **Direct download**
4. **Shell installer script**

#### AppImage Installation
```bash
# Download AppImage
wget https://github.com/cpp-scaffold/cpp-scaffold/releases/latest/download/cpp-scaffold-1.0.0-x86_64.AppImage

# Make executable
chmod +x cpp-scaffold-1.0.0-x86_64.AppImage

# Run
./cpp-scaffold-1.0.0-x86_64.AppImage

# Optional: Install system-wide
sudo mv cpp-scaffold-1.0.0-x86_64.AppImage /usr/local/bin/cpp-scaffold
```

## Verification

After installation, verify that cpp-scaffold is working correctly:

```bash
# Check version
cpp-scaffold --version

# Check help
cpp-scaffold --help

# Test basic functionality
cpp-scaffold --list-templates
```

## Uninstallation

### Using Installer Scripts
```bash
# Linux/macOS
./install.sh --uninstall

# Windows
.\install.ps1 -Uninstall
```

### Using Package Managers
```bash
# Homebrew
brew uninstall cpp-scaffold

# Chocolatey
choco uninstall cpp-scaffold

# APT
sudo apt remove cpp-scaffold

# YUM/DNF
sudo yum remove cpp-scaffold
```

### Manual Removal
```bash
# Remove executable
sudo rm /usr/local/bin/cpp-scaffold

# Remove documentation
sudo rm -rf /usr/local/share/cpp-scaffold

# Remove user configuration (optional)
rm -rf ~/.config/cpp-scaffold
rm -rf ~/.cache/cpp-scaffold
```

## Troubleshooting

### Common Issues

#### Permission Denied
```bash
# Make sure the executable is in your PATH and has execute permissions
which cpp-scaffold
ls -la $(which cpp-scaffold)
```

#### Command Not Found
```bash
# Check if installation directory is in PATH
echo $PATH

# Add to PATH if needed (Linux/macOS)
export PATH="/usr/local/bin:$PATH"

# Add to PATH if needed (Windows)
$env:PATH += ";C:\Program Files\cpp-scaffold\bin"
```

#### SSL/TLS Errors
```bash
# Update certificates (Linux)
sudo apt update && sudo apt install ca-certificates

# Update certificates (macOS)
brew install ca-certificates
```

### Getting Help

1. **Documentation**: Check the [official documentation](https://cpp-scaffold.org/docs)
2. **Issues**: Report problems on [GitHub Issues](https://github.com/cpp-scaffold/cpp-scaffold/issues)
3. **Discussions**: Join the [GitHub Discussions](https://github.com/cpp-scaffold/cpp-scaffold/discussions)
4. **Support**: Contact support at support@cpp-scaffold.org

## Advanced Configuration

### Environment Variables
```bash
# Custom configuration directory
export CPP_SCAFFOLD_CONFIG_DIR="$HOME/.config/cpp-scaffold"

# Custom cache directory
export CPP_SCAFFOLD_CACHE_DIR="$HOME/.cache/cpp-scaffold"

# Enable debug logging
export CPP_SCAFFOLD_DEBUG=1
```

### Configuration File
Create `~/.config/cpp-scaffold/config.json`:
```json
{
  "default_template": "console",
  "default_build_system": "cmake",
  "default_package_manager": "vcpkg",
  "auto_update_check": true,
  "telemetry_enabled": false
}
```

## Updates

### Automatic Updates
cpp-scaffold can check for updates automatically:
```bash
cpp-scaffold --check-updates
cpp-scaffold --update
```

### Manual Updates
Follow the same installation process to update to the latest version. The installer will detect existing installations and offer to upgrade.
