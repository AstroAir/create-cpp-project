#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Setup script for package signing infrastructure

.DESCRIPTION
    This script helps configure code signing for different platforms.
    Supports Windows, macOS, and Linux signing setup and verification.

.PARAMETER Platform
    Target platform (windows, macos, linux, all)

.PARAMETER WindowsCert
    Path to Windows certificate (.p12/.pfx)

.PARAMETER MacOSIdentity
    macOS signing identity

.PARAMETER LinuxGpgKey
    Linux GPG key ID

.PARAMETER Verify
    Verify signing setup only

.EXAMPLE
    .\setup_signing.ps1 -Platform windows -WindowsCert mycert.p12

.EXAMPLE
    .\setup_signing.ps1 -Platform macos -MacOSIdentity "Developer ID Application: My Company"

.EXAMPLE
    .\setup_signing.ps1 -Verify
#>

param(
    [Parameter()]
    [ValidateSet("windows", "macos", "linux", "all")]
    [string]$Platform,
    
    [Parameter()]
    [string]$WindowsCert,
    
    [Parameter()]
    [string]$MacOSIdentity,
    
    [Parameter()]
    [string]$LinuxGpgKey,
    
    [Parameter()]
    [switch]$Verify
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Helper functions
function Write-Status {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Info {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Cyan
}

function Write-Warning {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

# Check platform compatibility
function Test-PlatformCompatibility {
    param([string]$TargetPlatform)
    
    $currentPlatform = if ($IsWindows) { "windows" } elseif ($IsMacOS) { "macos" } else { "linux" }
    
    if ($TargetPlatform -ne "all" -and $TargetPlatform -ne $currentPlatform) {
        Write-Warning "Setting up $TargetPlatform signing on $currentPlatform platform"
        Write-Info "Some verification steps may not work cross-platform"
    }
}

# Setup Windows signing
function Set-WindowsSigning {
    param([string]$CertPath)
    
    Write-Info "Setting up Windows code signing..."
    
    # Check if signtool is available
    $signtool = Get-Command signtool -ErrorAction SilentlyContinue
    if (-not $signtool) {
        Write-Error "signtool not found. Please install Windows SDK or Visual Studio"
        return $false
    }
    
    # Verify certificate exists
    if (-not (Test-Path $CertPath)) {
        Write-Error "Certificate not found: $CertPath"
        return $false
    }
    
    Write-Status "Certificate found: $CertPath"
    
    # Test certificate if password is available
    $certPassword = $env:WINDOWS_CERT_PASSWORD
    if ($certPassword) {
        Write-Info "Testing certificate..."
        
        # Create a dummy file to test signing
        "test" | Out-File -FilePath "test_file.txt" -Encoding ASCII
        
        try {
            $result = & signtool sign /f $CertPath /p $certPassword /t "http://timestamp.digicert.com" "test_file.txt" 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Status "Certificate test successful"
            } else {
                Write-Error "Certificate test failed: $result"
                return $false
            }
        } finally {
            Remove-Item "test_file.txt" -ErrorAction SilentlyContinue
        }
    } else {
        Write-Warning "WINDOWS_CERT_PASSWORD not set, skipping certificate test"
    }
    
    # Set environment variable
    $env:WINDOWS_CERT_PATH = $CertPath
    Write-Status "Windows signing configured"
    return $true
}

# Setup macOS signing
function Set-MacOSSigning {
    param([string]$Identity)
    
    Write-Info "Setting up macOS code signing..."
    
    # Check if we're on macOS
    if (-not $IsMacOS) {
        Write-Warning "macOS signing setup on non-macOS platform"
        $env:MACOS_IDENTITY = $Identity
        Write-Status "macOS signing configured (cross-platform)"
        return $true
    }
    
    # Check if codesign is available
    $codesign = Get-Command codesign -ErrorAction SilentlyContinue
    if (-not $codesign) {
        Write-Error "codesign not found. Please install Xcode command line tools"
        return $false
    }
    
    # Verify identity
    $identities = & security find-identity -v -p codesigning 2>&1
    if ($identities -match [regex]::Escape($Identity)) {
        Write-Status "Signing identity found: $Identity"
        
        # Test signing
        Write-Info "Testing signing identity..."
        "test" | Out-File -FilePath "test_file.txt" -Encoding ASCII
        
        try {
            $result = & codesign --sign $Identity "test_file.txt" 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Status "Identity test successful"
            } else {
                Write-Error "Identity test failed: $result"
                return $false
            }
        } finally {
            Remove-Item "test_file.txt" -ErrorAction SilentlyContinue
        }
        
        # Check for notarization setup
        if ($env:APPLE_ID -and $env:APPLE_PASSWORD) {
            Write-Status "Notarization credentials configured"
        } else {
            Write-Warning "Notarization credentials not set (APPLE_ID, APPLE_PASSWORD)"
        }
        
        # Set environment variable
        $env:MACOS_IDENTITY = $Identity
        Write-Status "macOS signing configured"
        return $true
    } else {
        Write-Error "Signing identity not found: $Identity"
        Write-Info "Available identities:"
        & security find-identity -v -p codesigning
        return $false
    }
}

# Setup Linux signing
function Set-LinuxSigning {
    param([string]$GpgKey)
    
    Write-Info "Setting up Linux package signing..."
    
    # Check if we're on Linux
    if ($IsWindows -or $IsMacOS) {
        Write-Warning "Linux signing setup on non-Linux platform"
        $env:LINUX_GPG_KEY = $GpgKey
        Write-Status "Linux signing configured (cross-platform)"
        return $true
    }
    
    # Check if GPG is available
    $gpg = Get-Command gpg -ErrorAction SilentlyContinue
    if (-not $gpg) {
        Write-Error "gpg not found. Please install GnuPG"
        return $false
    }
    
    # Verify GPG key
    $result = & gpg --list-secret-keys $GpgKey 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Status "GPG key found: $GpgKey"
        
        # Test signing
        Write-Info "Testing GPG key..."
        "test" | Out-File -FilePath "test_file.txt" -Encoding ASCII
        
        try {
            $result = & gpg --armor --detach-sign --default-key $GpgKey "test_file.txt" 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Status "GPG key test successful"
            } else {
                Write-Error "GPG key test failed: $result"
                return $false
            }
        } finally {
            Remove-Item "test_file.txt" -ErrorAction SilentlyContinue
            Remove-Item "test_file.txt.asc" -ErrorAction SilentlyContinue
        }
        
        # Set environment variable
        $env:LINUX_GPG_KEY = $GpgKey
        Write-Status "Linux signing configured"
        return $true
    } else {
        Write-Error "GPG key not found: $GpgKey"
        Write-Info "Available keys:"
        & gpg --list-secret-keys --keyid-format LONG
        return $false
    }
}

# Verify signing setup
function Test-SigningSetup {
    Write-Info "Verifying signing setup..."
    
    $allGood = $true
    
    # Check Windows
    if ($env:WINDOWS_CERT_PATH) {
        if (Test-Path $env:WINDOWS_CERT_PATH) {
            Write-Status "Windows certificate: $($env:WINDOWS_CERT_PATH)"
        } else {
            Write-Error "Windows certificate not found: $($env:WINDOWS_CERT_PATH)"
            $allGood = $false
        }
    }
    
    # Check macOS
    if ($env:MACOS_IDENTITY) {
        if ($IsMacOS) {
            $identities = & security find-identity -v -p codesigning 2>&1
            if ($identities -match [regex]::Escape($env:MACOS_IDENTITY)) {
                Write-Status "macOS identity: $($env:MACOS_IDENTITY)"
            } else {
                Write-Error "macOS identity not found: $($env:MACOS_IDENTITY)"
                $allGood = $false
            }
        } else {
            Write-Status "macOS identity: $($env:MACOS_IDENTITY) (cross-platform)"
        }
    }
    
    # Check Linux
    if ($env:LINUX_GPG_KEY) {
        if (-not $IsWindows -and -not $IsMacOS) {
            $result = & gpg --list-secret-keys $env:LINUX_GPG_KEY 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Status "Linux GPG key: $($env:LINUX_GPG_KEY)"
            } else {
                Write-Error "Linux GPG key not found: $($env:LINUX_GPG_KEY)"
                $allGood = $false
            }
        } else {
            Write-Status "Linux GPG key: $($env:LINUX_GPG_KEY) (cross-platform)"
        }
    }
    
    if ($allGood) {
        Write-Status "All signing configurations verified"
        return $true
    } else {
        Write-Error "Some signing configurations failed verification"
        return $false
    }
}

# Main script logic
try {
    if ($Verify) {
        $success = Test-SigningSetup
        exit ($success ? 0 : 1)
    }
    
    if (-not $Platform) {
        Write-Error "Platform not specified. Use -Platform parameter."
        exit 1
    }
    
    Test-PlatformCompatibility $Platform
    
    $success = $true
    
    switch ($Platform) {
        "windows" {
            if (-not $WindowsCert) {
                Write-Error "Windows certificate path required. Use -WindowsCert parameter."
                exit 1
            }
            $success = Set-WindowsSigning $WindowsCert
        }
        "macos" {
            if (-not $MacOSIdentity) {
                Write-Error "macOS identity required. Use -MacOSIdentity parameter."
                exit 1
            }
            $success = Set-MacOSSigning $MacOSIdentity
        }
        "linux" {
            if (-not $LinuxGpgKey) {
                Write-Error "Linux GPG key required. Use -LinuxGpgKey parameter."
                exit 1
            }
            $success = Set-LinuxSigning $LinuxGpgKey
        }
        "all" {
            if ($WindowsCert) { $success = $success -and (Set-WindowsSigning $WindowsCert) }
            if ($MacOSIdentity) { $success = $success -and (Set-MacOSSigning $MacOSIdentity) }
            if ($LinuxGpgKey) { $success = $success -and (Set-LinuxSigning $LinuxGpgKey) }
        }
        default {
            Write-Error "Invalid platform: $Platform"
            Write-Info "Valid platforms: windows, macos, linux, all"
            exit 1
        }
    }
    
    if ($success) {
        Write-Status "Signing setup completed for $Platform"
        exit 0
    } else {
        Write-Error "Signing setup failed for $Platform"
        exit 1
    }
    
} catch {
    Write-Error "An error occurred: $($_.Exception.Message)"
    exit 1
}
