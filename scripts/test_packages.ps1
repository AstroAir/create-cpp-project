#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Comprehensive package testing script for Windows

.DESCRIPTION
    Tests package creation, validation, installation, and functionality
    across different package formats and platforms.

.PARAMETER Platform
    Target platform (windows, macos, linux, current)

.PARAMETER Types
    Package types to test (comma-separated)

.PARAMETER InstallTest
    Run installation tests

.PARAMETER SignatureTest
    Run signature verification tests

.PARAMETER NoCleanup
    Don't cleanup test files

.PARAMETER Verbose
    Enable verbose output

.EXAMPLE
    .\test_packages.ps1 -Platform current -InstallTest

.EXAMPLE
    .\test_packages.ps1 -Platform windows -Types "exe,msi" -SignatureTest
#>

param(
    [Parameter()]
    [ValidateSet("windows", "macos", "linux", "current")]
    [string]$Platform = "current",
    
    [Parameter()]
    [string]$Types,
    
    [Parameter()]
    [switch]$InstallTest,
    
    [Parameter()]
    [switch]$SignatureTest,
    
    [Parameter()]
    [switch]$NoCleanup,
    
    [Parameter()]
    [switch]$Verbose
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Test counters
$script:TestsPassed = 0
$script:TestsFailed = 0
$script:TestsSkipped = 0

# Helper functions
function Write-TestStatus {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-TestInfo {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Cyan
}

function Write-TestWarning {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

function Write-TestError {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

function Write-TestHeader {
    param([string]$Message)
    Write-Host "`n=== $Message ===" -ForegroundColor Cyan
}

function Pass-Test {
    param([string]$Message)
    $script:TestsPassed++
    Write-TestStatus $Message
}

function Fail-Test {
    param([string]$Message)
    $script:TestsFailed++
    Write-TestError $Message
}

function Skip-Test {
    param([string]$Message)
    $script:TestsSkipped++
    Write-TestWarning "$Message (skipped)"
}

# Get script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectRoot "build"

# Detect current platform if not specified
if ($Platform -eq "current") {
    if ($IsWindows) {
        $Platform = "windows"
    } elseif ($IsMacOS) {
        $Platform = "macos"
    } else {
        $Platform = "linux"
    }
}

Write-TestInfo "Testing packages for platform: $Platform"
Write-TestInfo "Build directory: $BuildDir"

# Change to project root
Push-Location $ProjectRoot

try {
    # Test 1: Build packages
    Write-TestHeader "Package Creation Test"
    
    if (-not (Test-Path $BuildDir)) {
        Write-TestInfo "Build directory not found, configuring..."
        $configResult = & cmake --preset default
        if ($LASTEXITCODE -eq 0) {
            Pass-Test "CMake configuration successful"
        } else {
            Fail-Test "CMake configuration failed"
            exit 1
        }
    }
    
    Write-TestInfo "Building packages..."
    $buildResult = & cmake --build --preset default --target package
    if ($LASTEXITCODE -eq 0) {
        Pass-Test "Package creation successful"
    } else {
        Fail-Test "Package creation failed"
        exit 1
    }
    
    # Test 2: Package validation
    Write-TestHeader "Package Validation Test"
    
    # Find created packages
    $PackageExtensions = @("*.exe", "*.msi", "*.dmg", "*.deb", "*.rpm", "*.AppImage", "*.zip", "*.tar.gz")
    $Packages = @()
    
    foreach ($ext in $PackageExtensions) {
        $found = Get-ChildItem -Path $BuildDir -Filter $ext -ErrorAction SilentlyContinue
        if ($found) {
            $Packages += $found
        }
    }
    
    if ($Packages.Count -eq 0) {
        Fail-Test "No packages found"
        exit 1
    } else {
        Pass-Test "Found $($Packages.Count) package(s)"
    }
    
    # Validate each package
    foreach ($package in $Packages) {
        Write-TestInfo "Validating: $($package.Name)"
        
        # Check file size
        $size = $package.Length
        if ($size -gt 1048576) {
            Pass-Test "Package size OK: $size bytes"
        } else {
            Fail-Test "Package too small: $size bytes"
            continue
        }
        
        # Platform-specific validation
        switch -Regex ($package.Extension) {
            "\.(exe|msi)" {
                try {
                    $signature = Get-AuthenticodeSignature $package.FullName
                    if ($signature.Status -ne "NotSigned") {
                        Pass-Test "Valid Windows package: $($package.Name)"
                    } else {
                        Pass-Test "Valid Windows package (unsigned): $($package.Name)"
                    }
                } catch {
                    Fail-Test "Invalid Windows package: $($package.Name)"
                }
            }
            "\.dmg" {
                if ($IsMacOS) {
                    try {
                        $result = & hdiutil verify $package.FullName 2>&1
                        if ($LASTEXITCODE -eq 0) {
                            Pass-Test "Valid DMG package: $($package.Name)"
                        } else {
                            Fail-Test "Invalid DMG package: $($package.Name)"
                        }
                    } catch {
                        Skip-Test "DMG validation (hdiutil not available)"
                    }
                } else {
                    Skip-Test "DMG validation (not on macOS)"
                }
            }
            "\.deb" {
                try {
                    $result = & dpkg --info $package.FullName 2>&1
                    if ($LASTEXITCODE -eq 0) {
                        Pass-Test "Valid DEB package: $($package.Name)"
                    } else {
                        Fail-Test "Invalid DEB package: $($package.Name)"
                    }
                } catch {
                    Skip-Test "DEB validation (dpkg not available)"
                }
            }
            "\.rpm" {
                try {
                    $result = & rpm -qp $package.FullName 2>&1
                    if ($LASTEXITCODE -eq 0) {
                        Pass-Test "Valid RPM package: $($package.Name)"
                    } else {
                        Fail-Test "Invalid RPM package: $($package.Name)"
                    }
                } catch {
                    Skip-Test "RPM validation (rpm not available)"
                }
            }
            "\.AppImage" {
                try {
                    $fileInfo = & file $package.FullName 2>&1
                    if ($fileInfo -match "executable") {
                        Pass-Test "Valid AppImage: $($package.Name)"
                    } else {
                        Fail-Test "Invalid AppImage: $($package.Name)"
                    }
                } catch {
                    Skip-Test "AppImage validation (file command not available)"
                }
            }
            "\.zip" {
                try {
                    Add-Type -AssemblyName System.IO.Compression.FileSystem
                    $zip = [System.IO.Compression.ZipFile]::OpenRead($package.FullName)
                    $zip.Dispose()
                    Pass-Test "Valid ZIP archive: $($package.Name)"
                } catch {
                    Fail-Test "Invalid ZIP archive: $($package.Name)"
                }
            }
            "\.tar\.gz" {
                try {
                    $result = & tar -tzf $package.FullName 2>&1
                    if ($LASTEXITCODE -eq 0) {
                        Pass-Test "Valid TAR.GZ archive: $($package.Name)"
                    } else {
                        Fail-Test "Invalid TAR.GZ archive: $($package.Name)"
                    }
                } catch {
                    Skip-Test "TAR.GZ validation (tar not available)"
                }
            }
        }
    }
    
    # Test 3: Installation test
    if ($InstallTest) {
        Write-TestHeader "Installation Test"
        
        $TestInstallDir = Join-Path $BuildDir "test_install"
        if (Test-Path $TestInstallDir) {
            Remove-Item -Recurse -Force $TestInstallDir
        }
        New-Item -ItemType Directory -Path $TestInstallDir -Force | Out-Null
        
        Write-TestInfo "Testing installation to: $TestInstallDir"
        
        $installResult = & cmake --install $BuildDir --prefix $TestInstallDir
        if ($LASTEXITCODE -eq 0) {
            Pass-Test "Installation successful"
            
            # Check if executable was installed
            $executablePath = Join-Path $TestInstallDir "bin\cpp-scaffold.exe"
            if (Test-Path $executablePath) {
                Pass-Test "Executable installed correctly"
                
                # Test executable
                try {
                    $testResult = & $executablePath --version 2>&1
                    if ($LASTEXITCODE -eq 0) {
                        Pass-Test "Installed executable works"
                    } else {
                        Fail-Test "Installed executable test failed"
                    }
                } catch {
                    Fail-Test "Installed executable test failed with exception"
                }
            } else {
                Fail-Test "Executable not found after installation"
            }
        } else {
            Fail-Test "Installation failed"
        }
        
        # Cleanup installation test
        if (-not $NoCleanup -and (Test-Path $TestInstallDir)) {
            Remove-Item -Recurse -Force $TestInstallDir
        }
    }
    
    # Test 4: Signature verification
    if ($SignatureTest) {
        Write-TestHeader "Signature Verification Test"
        
        if ($env:ENABLE_PACKAGE_SIGNING) {
            $signatureResult = & cmake --build $BuildDir --target verify_signatures
            if ($LASTEXITCODE -eq 0) {
                Pass-Test "Signature verification successful"
            } else {
                Fail-Test "Signature verification failed"
            }
        } else {
            Skip-Test "Package signing not enabled"
        }
    }
    
    # Test summary
    Write-TestHeader "Test Summary"
    Write-Host "Tests passed:  " -NoNewline
    Write-Host $script:TestsPassed -ForegroundColor Green
    Write-Host "Tests failed:  " -NoNewline
    Write-Host $script:TestsFailed -ForegroundColor Red
    Write-Host "Tests skipped: " -NoNewline
    Write-Host $script:TestsSkipped -ForegroundColor Yellow
    
    if ($script:TestsFailed -eq 0) {
        Write-TestStatus "All tests passed!"
        exit 0
    } else {
        Write-TestError "$($script:TestsFailed) test(s) failed"
        exit 1
    }
    
} catch {
    Write-TestError "An error occurred: $($_.Exception.Message)"
    exit 1
} finally {
    Pop-Location
}
