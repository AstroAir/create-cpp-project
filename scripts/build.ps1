#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Modern build script for cpp-scaffold project

.DESCRIPTION
    This script provides a modern build interface with support for different
    build configurations, parallel compilation, and development tools.

.PARAMETER Config
    Build configuration: debug, release, fast-build, minimal, or default

.PARAMETER Clean
    Clean build directory before building

.PARAMETER Tests
    Run tests after building

.PARAMETER Install
    Install the project after building

.PARAMETER Verbose
    Enable verbose output

.EXAMPLE
    .\build.ps1 -Config debug -Tests
    .\build.ps1 -Config release -Clean -Install
#>

param(
    [Parameter()]
    [ValidateSet("debug", "release", "fast-build", "minimal", "default")]
    [string]$Config = "default",
    
    [Parameter()]
    [switch]$Clean,
    
    [Parameter()]
    [switch]$Tests,
    
    [Parameter()]
    [switch]$Install,
    
    [Parameter()]
    [switch]$Verbose
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir

Write-Host "🚀 Building cpp-scaffold project..." -ForegroundColor Green
Write-Host "Configuration: $Config" -ForegroundColor Cyan

# Change to project root
Push-Location $ProjectRoot

try {
    # Clean if requested
    if ($Clean) {
        Write-Host "🧹 Cleaning build directories..." -ForegroundColor Yellow
        $BuildDirs = @("build", "build-debug", "build-release", "build-fast", "build-minimal")
        foreach ($dir in $BuildDirs) {
            if (Test-Path $dir) {
                Remove-Item -Recurse -Force $dir
                Write-Host "Cleaned $dir" -ForegroundColor Gray
            }
        }
    }

    # Configure
    Write-Host "⚙️  Configuring build..." -ForegroundColor Yellow
    $ConfigureArgs = @("--preset", $Config)
    if ($Verbose) {
        $ConfigureArgs += "--verbose"
    }
    
    & cmake $ConfigureArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Configuration failed"
    }

    # Build
    Write-Host "🔨 Building..." -ForegroundColor Yellow
    $BuildArgs = @("--build", "--preset", $Config)
    if ($Verbose) {
        $BuildArgs += "--verbose"
    }
    
    # Add parallel jobs
    $Cores = (Get-CimInstance -ClassName Win32_ComputerSystem).NumberOfLogicalProcessors
    $Jobs = [Math]::Min($Cores, 8)  # Cap at 8 to avoid overwhelming
    $BuildArgs += "--parallel", $Jobs
    
    Write-Host "Using $Jobs parallel jobs" -ForegroundColor Gray
    
    & cmake $BuildArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }

    # Run tests if requested
    if ($Tests) {
        Write-Host "🧪 Running tests..." -ForegroundColor Yellow
        & ctest --preset default --output-on-failure
        if ($LASTEXITCODE -ne 0) {
            Write-Warning "Some tests failed"
        }
    }

    # Install if requested
    if ($Install) {
        Write-Host "📦 Installing..." -ForegroundColor Yellow
        & cmake --install "build-$Config" --config $Config
        if ($LASTEXITCODE -ne 0) {
            throw "Installation failed"
        }
    }

    Write-Host "✅ Build completed successfully!" -ForegroundColor Green

} catch {
    Write-Host "❌ Build failed: $_" -ForegroundColor Red
    exit 1
} finally {
    Pop-Location
}
