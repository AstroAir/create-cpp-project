# Code Signing Guide

This guide explains how to set up and use code signing for cpp-scaffold packages across different platforms.

## Overview

Code signing provides authenticity and integrity verification for distributed packages. It helps users trust that the software comes from a verified source and hasn't been tampered with.

## Platform Support

- **Windows**: Authenticode signing for executables and MSI packages
- **macOS**: Code signing and notarization for applications and DMG packages
- **Linux**: GPG signing for DEB and RPM packages

## Prerequisites

### Windows
- Windows SDK or Visual Studio (for signtool)
- Code signing certificate (.p12 or .pfx file)
- Certificate password

### macOS
- Xcode command line tools
- Apple Developer account
- Developer ID certificate installed in Keychain
- App-specific password for notarization (optional)

### Linux
- GnuPG installed
- GPG key pair for signing
- Key uploaded to public keyservers (recommended)

## Setup

### Automated Setup

Use the provided setup scripts to configure signing:

```bash
# Linux/macOS
./scripts/setup_signing.sh --platform all \
  --windows-cert path/to/cert.p12 \
  --macos-identity "Developer ID Application: Your Name" \
  --linux-gpg-key "12345678"

# Windows PowerShell
.\scripts\setup_signing.ps1 -Platform all `
  -WindowsCert "path\to\cert.p12" `
  -MacOSIdentity "Developer ID Application: Your Name" `
  -LinuxGpgKey "12345678"
```

### Manual Setup

#### Windows

1. Install Windows SDK or Visual Studio
2. Obtain a code signing certificate
3. Set environment variables:
   ```cmd
   set WINDOWS_CERT_PATH=C:\path\to\certificate.p12
   set WINDOWS_CERT_PASSWORD=your_password
   set ENABLE_PACKAGE_SIGNING=1
   ```

#### macOS

1. Install Xcode command line tools:
   ```bash
   xcode-select --install
   ```

2. Import your Developer ID certificate into Keychain

3. Set environment variables:
   ```bash
   export MACOS_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"
   export APPLE_ID="your@apple.id"
   export APPLE_PASSWORD="app-specific-password"
   export ENABLE_PACKAGE_SIGNING=1
   ```

#### Linux

1. Install GnuPG:
   ```bash
   # Ubuntu/Debian
   sudo apt install gnupg
   
   # CentOS/RHEL
   sudo yum install gnupg2
   ```

2. Generate or import GPG key:
   ```bash
   # Generate new key
   gpg --full-generate-key
   
   # Or import existing key
   gpg --import private-key.asc
   ```

3. Set environment variables:
   ```bash
   export LINUX_GPG_KEY="12345678"  # Your key ID
   export ENABLE_PACKAGE_SIGNING=1
   ```

## Building Signed Packages

Once signing is configured, build packages normally:

```bash
# Configure and build
cmake --preset release
cmake --build --preset release

# Create packages (will be automatically signed)
cpack --config build/CPackConfig.cmake

# Verify signatures
cmake --build build --target verify_signatures
```

## Verification

### Verify Setup
```bash
# Linux/macOS
./scripts/setup_signing.sh --verify

# Windows
.\scripts\setup_signing.ps1 -Verify
```

### Verify Package Signatures

#### Windows
```cmd
# Verify executable
signtool verify /pa cpp-scaffold.exe

# Verify MSI package
signtool verify /pa cpp-scaffold-1.0.0-win64.msi
```

#### macOS
```bash
# Verify application
codesign --verify --verbose cpp-scaffold.app

# Verify DMG
codesign --verify --verbose cpp-scaffold-1.0.0-macOS.dmg

# Check notarization
spctl --assess --verbose cpp-scaffold.app
```

#### Linux
```bash
# Verify DEB package
dpkg-sig --verify cpp-scaffold_1.0.0_amd64.deb

# Verify RPM package
rpm --checksig cpp-scaffold-1.0.0-1.x86_64.rpm
```

## Troubleshooting

### Common Issues

#### Windows
- **"signtool not found"**: Install Windows SDK or Visual Studio
- **"Certificate not found"**: Check certificate path and format
- **"Invalid password"**: Verify certificate password
- **"Timestamp server unavailable"**: Try different timestamp server

#### macOS
- **"Identity not found"**: Import certificate into Keychain
- **"Notarization failed"**: Check Apple ID credentials and app-specific password
- **"Hardened runtime issues"**: Review entitlements.plist

#### Linux
- **"GPG key not found"**: Import or generate GPG key
- **"Permission denied"**: Check GPG key permissions
- **"Package signing failed"**: Verify GPG key has signing capability

### Debug Mode

Enable verbose output for debugging:

```bash
# Set debug environment variable
export CPACK_VERBATIM_VARIABLES=ON

# Build with verbose output
cmake --build build --verbose
cpack --config build/CPackConfig.cmake --verbose
```

## Security Best Practices

1. **Protect Private Keys**: Store certificates and private keys securely
2. **Use Strong Passwords**: Use complex passwords for certificate protection
3. **Regular Key Rotation**: Rotate signing keys periodically
4. **Timestamp Signing**: Always use timestamp servers for long-term validity
5. **Verify Before Distribution**: Always verify signatures before releasing
6. **Backup Keys**: Maintain secure backups of signing keys
7. **Access Control**: Limit access to signing infrastructure

## CI/CD Integration

For automated builds, store signing credentials securely:

### GitHub Actions
```yaml
env:
  WINDOWS_CERT_PATH: ${{ secrets.WINDOWS_CERT_PATH }}
  WINDOWS_CERT_PASSWORD: ${{ secrets.WINDOWS_CERT_PASSWORD }}
  MACOS_IDENTITY: ${{ secrets.MACOS_IDENTITY }}
  APPLE_ID: ${{ secrets.APPLE_ID }}
  APPLE_PASSWORD: ${{ secrets.APPLE_PASSWORD }}
  LINUX_GPG_KEY: ${{ secrets.LINUX_GPG_KEY }}
  ENABLE_PACKAGE_SIGNING: 1
```

### Security Considerations
- Use encrypted secrets for sensitive data
- Limit signing to release branches only
- Implement approval workflows for releases
- Monitor signing activity and logs

## Support

For signing-related issues:
1. Check this documentation
2. Review error messages and logs
3. Verify platform-specific requirements
4. Test with minimal examples
5. Contact support with detailed error information
